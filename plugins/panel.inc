<?php

/**
 * @file
 * Defines base for migration destinations implemented as Drupal entities.
 */

/**
 * Base class for panel-based destination handling.
 */
class MigrateDestinationPanel extends MigrateDestination {
  public function fields() {
    return array(
      'id' => t('ID'),
      'arguments' => t('Arguments'),
    );
  }

  static public function getKeySchema() {
    return array(
      'title' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Delete a panel.
   */
  public function rollback(array $panel_names) {
    // load API functions.
    require_once drupal_get_path('module', 'page_manager') . '/plugins/tasks/page.inc';
    foreach ($panel_names as $panel_name) {
      // Store a $deleted boolean to make sure we're only executing
      // the delete operation once for this (mini) panel.
      $deleted = FALSE;
      if (!$deleted && module_exists('panels')) {
        if ($existing_page = page_manager_page_load($panel_name)) {
          page_manager_page_delete($existing_page);
          $deleted = TRUE;
        }
      }
      if (!$deleted && module_exists('panels_mini')) {
        if ($existing_page = panels_mini_load($panel_name)) {
          panels_mini_delete($existing_page);
          $deleted = TRUE;
        }
      }
    }
  }

  /**
   * Import a single panel.
   */
  public function import(stdClass $object, stdClass $row) {
    // sourceMigration() has already taken place at this point.
    // Data retrieved this way is stored in the $object, while the
    // $row still contains the title/ID before the sourceMigration()
    // process.
    // Since we'll be retrieving the panel page from the database later,
    // the sourceMigration() ID will get lost. To prevent this from
    // happening, we'll store the IDs in the $row.
    // Note that this means that, as far as Panels are concerned,
    // the implementation deviates slightly from standard Migrate behavior.
    foreach ($object as $field => $value) {
      $row->$field = $value;
    }

    // load API functions.
    require_once drupal_get_path('module', 'page_manager') . '/plugins/tasks/page.inc';

    $panel_name = '' . $row->xml->id;

    $migration = Migration::currentMigration();

    $panel_data = $migration->getPanel($panel_name);
    $page = $panel_data['panel'];
    $type = $panel_data['type'];
    $object = $page;
    $this->prepare($page, $row);

    if ($type == 'panel' && module_exists('panels')) {
      // If the page already exists, delete it and re-create it.
      if ($existing_page = page_manager_page_load($panel_name)) {
        page_manager_page_delete($existing_page);
      }

      if (!empty($page)) {
        // save the updated page object into DB.
        $page = page_manager_page_save($page);
        $this->numCreated++;
      }
    }
    else if ($type == 'mini' && module_exists('panels_mini')) {
      // If the page already exists, delete it and re-create it.
      if ($existing_page = panels_mini_load($panel_name)) {
        panels_mini_delete($existing_page);
      }

      if (!empty($page)) {
        // save the updated page object into DB.
        $page = panels_mini_save($page);
        $this->numCreated++;
      }
    }
    return array($panel_name);
  }

  public function prepare(stdClass $page, stdClass $source_row) {
    $migration = Migration::currentMigration();
    // Then call any prepare handler for this specific Migration.
    if (method_exists($migration, 'prepare')) {
      $migration->prepare($page, $source_row);
    }
  }
  public function complete(stdClass $page, stdClass $source_row) {
    $migration = Migration::currentMigration();
    if (method_exists($migration, 'complete')) {
      $migration->complete($page, $source_row);
    }
  }

  /**
   * Derived classes must implement __toString().
   *
   * @return string
   *  Description of the destination being migrated into
   */
  public function __toString() {
    return 'MigrateDestinationPanel';
  }
}
