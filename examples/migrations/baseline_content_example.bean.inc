<?php

/**
 * @file
 * Bean example migration.
 */

class BaselineContentExampleBeanMigration extends BaselineContentBeanMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.bean.csv';
    parent::__construct($import_path, 'content');
    $this->description = t('Import bean example.');
    $this->addFieldMapping('field_block_body', 'body');
    $this->addFieldMapping('field_block_body:format')->defaultValue('full_html');
  }

  function csvcolumns() {
    $columns = parent::csvcolumns();
    $columns[] = array('body', 'Body');
    return $columns;
  }
}
