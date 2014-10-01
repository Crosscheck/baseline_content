<?php

/**
 * @file
 * Infobox CT migration.
 */

class FooContentPanelizerDefault extends BaselineContentPanelizerDefaultMigration {


  public function __construct() {
    $module_path = drupal_get_path('module', 'foo_content');
    $import_path = $module_path . '/import/foo_content.panelizer.default.xml';
    parent::__construct($import_path);
    $this->description = t('Import Panelizer default settings.');

    $this->dependencies = array(
    );
  }

  public function prepare(stdClass $page, stdClass $row) {

  }

  public function complete(stdClass $page, stdClass $row) {

  }
}
