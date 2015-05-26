<?php

/**
 * @file
 * Baseline content example: Imports data from exported nodes.
 * Nodes can be exported using "drush bcen nid1 nid2 nid3".
 */

class BaselineContentExampleNodeExport extends BaselineContentNodeExportMigration {

  public function __construct() {
    // NOTE: your baseline content implementation module MUST implement hook_baseline_content_api().
    $module_path = drupal_get_path('module', 'baseline_content_example');
    $import_path = $module_path . '/node_export/node_export.csv';
    $files_path = $module_path . '/node_export';
    parent::__construct($import_path, $files_path);
    $this->force_update = TRUE;
  }

  /**
   * Everything in the import file's <migrate> tag is converted to an array which we can work
   * with in the migration handler.
   * In the following example, the import files contain an <xmlsitemap_priority> tag:
   * <migrate>
   *   <xmlsitemap_priority>0.3</xmlsitemap_priority>
   * </migrate>
   */
  public function prepare($entity, $row) {
    if (!empty($row->migrate['xmlsitemap_priority'])) {
      $entity->xmlsitemap['priority_override'] = 1;
      $entity->xmlsitemap['priority'] = $row->migrate['xmlsitemap_priority'];
    }
  }
}
