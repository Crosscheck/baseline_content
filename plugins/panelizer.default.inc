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
  public function rollback(array $panel_ids) {
    foreach ($panel_ids as $panel_id) {
      // Unset any variables that may have been set during import.
      $variable_string = variable_get('baseline_content_variables_for_panel_' . $panel_id, '');
      if (!empty($variable_string)) {
        $variables = explode(',', $variable_string);
        foreach ($variables as $variable) {
          variable_del($variable);
        }
        variable_del('baseline_content_variables_for_panel_' . $panel_id);
      }
      db_delete('panelizer_defaults')
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
   * Import a single panel.
   */
  public function import(stdClass $object, stdClass $row) {

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
      'panelizer_defaults' => array(),
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
    // The panel ID also lets us insert the Panelizer defaults.
    $panelizer_defaults_columns = (array) $row->xml->panelizer_settings->panelizer_defaults;
    foreach ($panelizer_defaults_columns as $column_name => $value) {
      $fields['panelizer_defaults'][$column_name] = $value;
    }
    $fields['panelizer_defaults']['did'] = $did;
    db_insert('panelizer_defaults')
      ->fields($fields['panelizer_defaults'])
      ->execute();

    // Set the panel's default settings.
    if (!empty($row->xml->variables)) {
      foreach ($row->xml->variables as $variables) {
        $array = (array) $variables;
        foreach ($array as $name => $variable) {
          $variable = unserialize($variable);
          variable_set($name, $variable);

          // Set a variable to point to the imported panel's variables. This
          // is needed to unset these variables upon rollback.
          // TODO: refactor so an extra variable isn't needed.
          $old_variable = variable_get('baseline_content_variables_for_panel_' . $did, '');
          if (!empty($old_variable)) {
            $old_variable = $old_variable . ',';
          }
          $new_variable = $old_variable . $name;
          variable_set('baseline_content_variables_for_panel_' . $did, $new_variable);
        }
      }
    }

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
    return 'MigrateDestinationPanelizerDefault';
  }
}
