<?php

/**
 * @file
 * Defines base for migration destinations implemented as Drupal entities.
 */

/**
 * Abstract base class for Panelizer Entity destination handling.
 */
class MigrateDestinationPanelizerEntity extends MigrateDestination {

  public function fields() {
    return array(
      'description' => t('Description'),
      'entity_title'          => t('Entity title'),
      'panelizer_entity_export'          => t('Panelizer Entity Export'),
    );
  }

  static public function getKeySchema() {
    return array(
      'entity_title' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Delete Panelizer settings for an entity.
   */
  public function rollback(array $node_ids) {
    foreach ($node_ids as $node_id) {
      $panelizer_deleted = $this->delete_panelizer_entity(node_load($node_id));
    }
  }

  /**
   * Import a single panelizer entity configuration.
   */
  public function import(stdClass $object, stdClass $row) {
    $node_id = $object->entity_title;
    $node = node_load($node_id);
    $panelizer_name = '' . $row->xml->description;

    // Get the exported panelizer and append it with "return $page" to assign it to a variable.
    $panelizer_code = $row->panelizer_entity_export . ' return $panelizer;';
    // Remove newlines.
    // TODO: do not remove newlines in custom textfields.
    $panelizer_code = trim(preg_replace('/\s\s+/', ' ', $panelizer_code));
    // Execute the exported panel as PHP code, giving us an object that represents the panel.
    $panelizer = eval($panelizer_code);

    $this->prepare($panelizer, $row);
    // If the page already exists, delete it.
    $panelizer_deleted = $this->delete_panelizer_entity($node);
    if (!empty($panelizer)) {
      // Name needs to be unset, else $panelizer->did will be 0, and the panel panes will not get created correctly.
      // Don't ask why.
      unset($panelizer->name);
      $node->panelizer = $panelizer;
      panelizer_entity_insert($node, 'node');
      if (!$panelizer_deleted) {
        $this->numCreated++;
      }
      else {
        $this->numUpdated++;
      }
    }
    return array($node_id);
  }

  /**
   * @param $panelizer_name
   * @return boolean Whether or not the delete was successful.
   */
  private function delete_panelizer_entity($node) {
    if (empty($node->panelizer['page_manager']->name) && !empty($node->panelizer['page_manager']->display)) {
      panelizer_entity_delete($node, 'node');
      return TRUE;
    }
    return FALSE;
  }

  public function prepare(stdClass $object, stdClass $row) {
    $migration = Migration::currentMigration();
    // Then call any prepare handler for this specific Migration.
    if (method_exists($migration, 'prepare')) {
      $migration->prepare($object, $row);
    }
  }

  public function complete(stdClass $object, stdClass $source_row) {
    $migration = Migration::currentMigration();
    if (method_exists($migration, 'complete')) {
      $migration->complete($object, $source_row);
    }
  }

  /**
   * Derived classes must implement __toString().
   *
   * @return string
   *  Description of the destination being migrated into
   */
  public function __toString() {
    return 'MigrateDestinationPanelizerEntity';
  }
}
