<?php

/**
 * @file
 * Baseline content example: Imports files to be used by nodes.
 */

class BaselineContentExampleFilesMigration extends BaselineContentFilesMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.files.csv';
    $files_path = $module_path . '/examples/import/files';
    parent::__construct($import_path, $files_path);
  }
}
