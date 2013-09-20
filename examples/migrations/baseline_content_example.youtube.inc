<?php

/**
 * @file
 * Imports files to be used by nodes.
 */

class BaselineContentExampleYoutubeMigration extends BaselineContentFilesMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.youtube.csv';
    $destination = new MigrateDestinationMedia('video',
      'MigrateExtrasFileYoutube');
    parent::__construct($import_path, '', $destination);
  }
}
