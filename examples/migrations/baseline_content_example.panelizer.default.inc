<?php

class BaselineContentExamplePanelizerDefaultMigration extends BaselineContentPanelizerDefaultMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content_example');
    $import_path = $module_path . '/examples/import/baseline_content_example.panelizer.default.xml';
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
