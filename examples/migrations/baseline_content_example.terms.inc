<?php

/**
 * @file
 * Example migration for terms.
 */

class BaselineContentExampleTermMigration extends BaselineContentTermMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.terms.csv';
    parent::__construct($import_path, 'tags');
    $this->description = t('Import tags.');
  }
}
