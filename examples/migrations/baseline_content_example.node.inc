<?php

/**
 * @file
 * Example node migration.
 */

class BaselineContentExampleNodeMigration extends BaselineContentNodeMigration {


  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.node.xml';
    parent::__construct($import_path, 'page');
    $this->description = t('Import pages.');

    // Declare all taxonomy and file migrations as dependencies.
    $this->dependencies = array(
      'ExcentisImportDivisions',
      'ExcentisImportFiles',
      'ExcentisImportReferences',
      'ExcentisImportResources',
      'ExcentisImportTutorials',
      'ExcentisImportTags',
      'ExcentisImportResourceNodes',
    );

    $this->addFieldMapping('field_taxonomy_division', 'division')
      ->xpath('division')
      ->sourceMigration('ExcentisImportDivisions');
    $this->addFieldMapping('field_taxonomy_division:source_type')
       ->defaultValue('tid');
    $this->addFieldMapping('field_taxonomy_reference', 'references')
      ->xpath('references')
      ->separator(',')
      ->sourceMigration('ExcentisImportReferences');
    $this->addFieldMapping('field_taxonomy_reference:source_type')
       ->defaultValue('tid');
    $this->addFieldMapping('field_taxonomy_tutorials', 'tutorials')
      ->xpath('tutorials')
      ->separator(',')
      ->sourceMigration('ExcentisImportTutorials');
    $this->addFieldMapping('field_taxonomy_tutorials:source_type')
       ->defaultValue('tid');
    $this->addFieldMapping('field_taxonomy_tags', 'tags')
      ->xpath('tags')
      ->separator(',')
      ->sourceMigration('ExcentisImportTags');
    $this->addFieldMapping('field_taxonomy_tags:source_type')
       ->defaultValue('tid');

    $this->addFieldMapping('field_reference_resources', 'resources')
      ->xpath('resources')
      ->separator(',')
      ->sourceMigration('ExcentisImportResourceNodes');
  }
}
