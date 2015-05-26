<?php

/**
 * Implements hook_drush_command().
 */
function baseline_content_drush_command() {
  $items['baseline-content-import-node'] = array(
    'description' => 'Import nodes from files. Note that, while this will work, it\s much better (and easy) to implement a Migrate class. See the example files contained in the baseline_content module.',
    'aliases' => array('bcin'),
    'options' => array(
      'force' => "Skip update-check and import regardless of whether the content has changed since last import",
    ),
  );
  $items['baseline-content-export-node'] = array(
    'description' => 'Export nodes to files.',
    'aliases' => array('bcen'),
    'arguments' => array(
      'nids' => 'A space delimited list of node IDs.',
    ),
    'options' => array(
      'force' => "Skip update-check and export regardless of whether the content has changed since last import",
      'all' => 'Also export content that does not yet exist in destination files',
    ),
  );
  return $items;
}

/**
 * Callback for the baseline-content-import-node command.
 */
function drush_baseline_content_import_node() {
  $force = drush_get_option('force');
  $uuids = func_get_args();
  baseline_content_import_nodes($uuids, $force);
}

/**
 * Callback for the baseline-content-export-node command.
 */
function drush_baseline_content_export_node() {
  $force = drush_get_option('force');
  $all = drush_get_option('all');
  $nids = func_get_args();
  if (!empty($nids) && !empty($all)) {
    drush_print('You must specify either a list of node IDs, or the --all flag.');
    drush_user_abort();
    exit(0);
  }
  baseline_content_export_nodes($nids, $force, $all);
}
