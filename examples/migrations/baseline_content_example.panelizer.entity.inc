<?php

/**
 * @file
 * Infobox CT migration.
 */

class FooContentPanelizerEntity extends BaselineContentPanelizerEntityMigration {


  public function __construct() {
    $module_path = drupal_get_path('module', 'foo_content');
    $import_path = $module_path . '/import/foo_content.panelizer.entity.xml';
    parent::__construct($import_path);
    $this->description = t('Import Panelizer entity settings.');

    $this->dependencies = array(
      'FooContentPanelizerDefault',
      'FooContentNodeLandingpage',
      'FooContentNodeThemepage',
      'FooContentNodePage',
      'FooContentNodeArticle',
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
