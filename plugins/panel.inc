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
      'panel_type'          => t('Panel type (panel/mini)'),
      'panel_export'          => t('Panel export'),
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

    // load API functions.
    require_once drupal_get_path('module', 'page_manager') . '/plugins/tasks/page.inc';

    $panel_name = '' . $row->xml->id;

    // Get the exported panel and append it with "return $page" to assign it to a variable.
    $panel_code = $row->panel_export . ' return $page;';
    // Remove newlines.
    // TODO: do not remove newlines in custom textfields.
    $panel_code = trim(preg_replace('/\s\s+/', ' ', $panel_code));
    // Execute the exported panel as PHP code, giving us an object that represents the panel.
    $panel = eval($panel_code);

    $type = $row->panel_type;
    $this->prepare($panel, $row);

    if ($type == 'panel' && module_exists('panels')) {
      // If the page already exists, get its ID.
      if ($existing_page = page_manager_page_load($panel_name)) {
        $panel->pid = $existing_page->pid;
      }

      if (!empty($panel)) {
        // save the updated page object into DB.
        $panel = page_manager_page_save($panel);
        if (empty($existing_page)) {
          $this->numCreated++;
        }
        else {
          $this->numUpdated++;
        }
      }
    }
    else if ($type == 'mini' && module_exists('panels_mini')) {
      // If the page already exists, get its ID.
      if ($existing_page = panels_mini_load($panel_name)) {
        $panel->pid = $existing_page->pid;
      }

      if (!empty($panel)) {
        // save the updated page object into DB.
        $panel = panels_mini_save($panel);
        if (empty($existing_page)) {
          $this->numCreated++;
        }
        else {
          $this->numUpdated++;
        }
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
