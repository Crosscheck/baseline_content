<?php

class BaselineContentExamplePanelizerEntityMigration extends BaselineContentPanelizerEntityMigration {


  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content_example');
    $import_path = $module_path . '/examples/import/baseline_content_example.panelizer.entity.xml';
    parent::__construct($import_path);
    $this->description = t('Import Panelizer entity settings.');

    $this->dependencies = array(
      'BaselineContentExamplePanelizerDefaultMigration',
      'BaselineContentExampleNodeMigration',
    );

    $this->addFieldMapping('entity_title', 'entity_title')
      ->xpath('entity_title')
      ->sourceMigration($this->dependencies);
  }

  public function prepare(stdClass $page, stdClass $row) {

  }

  public function complete(stdClass $page, stdClass $row) {

  }
}
