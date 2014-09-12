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
      'panel_settings'          => t('Panel settings'),
      'panelizer_settings'          => t('Panelizer settings'),
    );
  }

  static public function getKeySchema() {
    return array(
      'description' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Delete Panelizer settings for an entity.
   */
  public function rollback(array $panel_ids) {
    foreach ($panel_ids as $panel_id) {
      db_delete('panelizer_entity')
        ->condition('did', $panel_id)
        ->execute();
      db_delete('panels_pane')
        ->condition('did', $panel_id)
        ->execute();
      db_delete('panels_display')
        ->condition('did', $panel_id)
        ->execute();
    }
  }

  /**
   * Import a single panelized entity.
   */
  public function import(stdClass $object, stdClass $row) {
    $node_id = $object->entity_title;
    $node = node_load($node_id);
    $revision_id = $node->vid;
    // Updates aren't entirely supported yet, so rollback any previous
    // migrations first.
    if (!empty($row->migrate_map_destid1)) {
      $this->rollback(array($row->migrate_map_destid1));
    }
    $panelizer_description = '' . $row->xml->description;

    $migration = Migration::currentMigration();

    $this->prepare($object, $row);

    $fields = array(
      'panels_display' => array(),
      'panels_pane' => array(),
      'panelizer_entity' => array(),
    );
    // Insert the panel display.
    $panels_display_columns = (array) $row->xml->panel_settings->panels_display;
    foreach ($panels_display_columns as $column_name => $value) {
      $fields['panels_display'][$column_name] = $value;
    }
    $did = db_insert('panels_display')
      ->fields($fields['panels_display'])
      ->execute();

    // This gives us a panel ID which we need for the panel panes.
    $panes = (array) $row->xml->panel_settings->panels_pane;
    foreach ($panes['pane'] as $pane) {
      $id = count($fields['panels_pane']);
      $fields['panels_pane'][$id] = array();
      $pane_fields = (array) $pane;
      foreach ($pane as $column_name => $value) {
        $fields['panels_pane'][$id][$column_name] = $value;
      }
      $fields['panels_pane'][$id]['did'] = $did;
      db_insert('panels_pane')
        ->fields($fields['panels_pane'][$id])
        ->execute();
    }
    // The panel ID also lets us insert the Panelizer entity.
    $panelizer_entity_columns = (array) $row->xml->panelizer_settings->panelizer_entity;
    foreach ($panelizer_entity_columns as $column_name => $value) {
      $fields['panelizer_entity'][$column_name] = $value;
    }
    $fields['panelizer_entity']['did'] = $did;
    $fields['panelizer_entity']['entity_id'] = $node_id;
    $fields['panelizer_entity']['revision_id'] = $revision_id;
    db_insert('panelizer_entity')
      ->fields($fields['panelizer_entity'])
      ->execute();

    $this->numCreated++;

    return array($did);
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
