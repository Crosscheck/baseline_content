<?php

/**
 * Abstract base class for Panelizer Default destination handling.
 */
class MigrateDestinationPanelizerDefault extends MigrateDestination {

  public function fields() {
    return array(
      'description' => t('Description'),
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
   * Delete Panelizer Default settings.
   */
  public function rollback(array $panelizer_names) {
    foreach ($panelizer_names as $panelizer_name) {
      // Unset any variables that may have been set during import.
      // panelizer_defaults_node_article
      $variable_key_parts = explode(":", $panelizer_name);
      $variable_key = 'panelizer_defaults_' . $variable_key_parts[0] . '_' . $variable_key_parts[1];
      variable_del($variable_key);
      $this->delete_panelizer_defaults($panelizer_name);
    }
  }

  /**
   * Import a single panelizer default configuration.
   */
  public function import(stdClass $object, stdClass $row) {

    $panelizer_name = '' . $row->xml->description;

    // Get the exported panelizer and append it with "return $page" to assign it to a variable.
    $panelizer_code = $row->panelizer_defaults_export . ' return $panelizer;';
    // Remove newlines.
    // TODO: do not remove newlines in custom textfields.
    $panelizer_code = trim(preg_replace('/\s\s+/', ' ', $panelizer_code));
    // Execute the exported panel as PHP code, giving us an object that represents the panel.
    $panelizer = eval($panelizer_code);

    $this->prepare($panelizer, $row);
    // If the page already exists, delete it.
    $panelizer_deleted = $this->delete_panelizer_defaults($panelizer_name);
    if (!empty($panelizer)) {
      // save the updated page object into DB.
      panelizer_export_save_callback($panelizer);
      // TODO: figure out why this variable isn't exported through the export UI, and see if there's a way
      // to remove it from the import file.
      if (!empty($row->xml->variables)) {
        foreach ($row->xml->variables as $variables) {
          $array = (array) $variables;
          foreach ($array as $name => $variable) {
            $variable = unserialize($variable);
            variable_set($name, $variable);
          }
        }
      }
      if (!$panelizer_deleted) {
        $this->numCreated++;
      }
      else {
        $this->numUpdated++;
      }
    }
    return array($panelizer_name);
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
    return 'MigrateDestinationPanelizerDefault';
  }

  /**
   * @param $panelizer_name
   * @return boolean Whether or not the delete was successful.
   */
  private function delete_panelizer_defaults($panelizer_name) {
    $existing_panelizer = panelizer_context_cache_get('default', $panelizer_name);
    if (!empty($existing_panelizer->pnid)) {
      panelizer_export_delete_callback($existing_panelizer);
      return TRUE;
    }
    return FALSE;
  }
}
