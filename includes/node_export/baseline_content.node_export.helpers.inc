<?php

/**
 * @param string $uuid The UUID of the entity to inspect.
 * @param null|array $records The set of import files to inspect.
 * @return bool TRUE if the file on disk has a newer "last updated" date than
 * the corresponding entity in the database.
 */
function _baseline_content_importfile_is_newer($uuid, $records = NULL) {
  if (!$records) {
    $records = _baseline_content_read_master_file();
  }
  $nid = _baseline_content_get_nid_by_uuid($uuid);
  if (!empty($nid)) {
    $last_updated_in_db = _baseline_content_get_node_last_updated_by_uuid($uuid);
    if ($last_updated_in_db == $records[$uuid]['last_updated']) {
      return FALSE;
    }
    return TRUE;
  }
  return TRUE;
}

/**
 * @param string $uuid The UUID of the node for which you want to get the Migrate XML tag.
 * @return array The Migrate XML settings as defined in the node import file.
 */
function baseline_content_get_migrate_settings($uuid) {
  $file_contents = _baseline_content_get_file_contents_by_uuid($uuid);
  $migrate_settings = trim(_baseline_content_get_migrate_settings_xml_node($file_contents));
  $xml = simplexml_load_string($migrate_settings);
  $json = json_encode($xml);
  $array = json_decode($json,TRUE);
  return $array;
}

/**
 * @param string $uuid The UUID of the node to inspect.
 * @return int|null The node ID, if set.
 */
function _baseline_content_get_nid_by_uuid($uuid) {
  $nids = entity_get_id_by_uuid('node', array($uuid));
  if (!empty($nids[$uuid])) {
    return $nids[$uuid];
  }
  return NULL;
}

/**
 * Returns a list of the entityreference fields in the current site which are
 * currently supported by Baseline Content's entityreference_uuid integration.
 */
function _baseline_content_get_supported_entityreference_fields($entity_type, $bundle_name) {
  $supported_entities = array(
    'taxonomy_term',
    'node',
  );
  $entityreference_fields = array();
  $field_info_instances = field_info_instances($entity_type, $bundle_name);
  foreach ($field_info_instances as $field => $instance) {
    $field_info = field_info_field($field);
    if ($field_info['type'] == 'entityreference' && in_array($field_info['settings']['target_type'], $supported_entities)) {
      $entityreference_fields[$field] = $field_info;
    }
  }
  return $entityreference_fields;
}

/**
 * Returns a list of the file fields in the current site which are
 * currently supported by Baseline Content's file UUID integration.
 */
function _baseline_content_get_supported_file_fields($entity_type, $bundle_name) {
  $file_fields = array();
  $field_info_instances = field_info_instances($entity_type, $bundle_name);
  foreach ($field_info_instances as $field => $instance) {
    $field_info = field_info_field($field);
    if (isset($field_info['foreign keys']['fid']['table']) && $field_info['foreign keys']['fid']['table'] == 'file_managed') {
      $file_fields[$field] = $field_info;
    }
  }
  return $file_fields;
}

/**
 * @param $file_contents string The contents of an XML import file.
 * @return array|string The contents of the <migrate> tag, or an empty tag if not found.
 */
function _baseline_content_get_migrate_settings_xml_node($file_contents) {
  $migrate_settings = array();
  preg_match('/\<migrate\>(.)*\<\/migrate\>/Us', $file_contents, $migrate_settings);
  $migrate_settings = $migrate_settings[0];
  if (empty($migrate_settings)) {
    $migrate_settings = '<migrate></migrate>';
  }
  $migrate_settings = '  ' . $migrate_settings;
  return $migrate_settings;
}

/**
 * @param string $path The path to the folder in which the import files are located.
 * @param string $file_type
 * @return array
 */
function _baseline_content_get_metadata_from_files($path, $file_type = 'xml') {
  $scanned_files = file_scan_directory($path, '/.*\.' . $file_type . '$/');
  $files = array();
  foreach ($scanned_files as $scanned_file) {
    $files[$scanned_file->name . '.' . $file_type] = $scanned_file;
  }
  return $files;
}

/**
 * Takes some information about a node and converts it into a filename.
 */
function _baseline_content_get_xml_filename_from_node_data($node_type, $node_title, $delta = 0) {
  $filename = $node_type . '.' . drupal_clean_css_identifier($node_title);
  if ($delta != 0) {
    $filename .= '.' . $delta;
  }
  $filename .= '.export.xml';
  return $filename;
}

/**
 * @param string $uuid The UUID of the entity to inspect.
 * Get the last updated timestamp of a node based on UUID.
 */
function _baseline_content_get_node_last_updated_by_uuid($uuid) {
  return db_select('node', 'n')
    ->fields('n', array('changed'))
    ->condition('uuid', $uuid)
    ->execute()
    ->fetchField();
}

/**
 * Get the path to the folder that contains all the import XMLs.
 */
function baseline_content_get_api_module_path($folder = 'node_export') {
  foreach (module_implements('baseline_content_api') as $module) {
    return drupal_get_path('module', $module) . '/' . $folder;
  }
  drush_print('No modules were found that implement hook_baseline_content_api(). Check the baseline_content_example.node_export.inc file for more info.');
  exit(1);
}

/**
 * Read all records (minus heading) from the master file.
 */
function _baseline_content_read_master_file() {
  $records = array();
  $path = baseline_content_get_api_module_path();
  $file_path = $path . '/node_export.csv';
  // If the master file has not yet been initialized, do so now.
  if (!file_exists(drupal_realpath($path))) {
    drupal_mkdir($path);
  }
  if (!file_exists(drupal_realpath($file_path))) {
    file_unmanaged_save_data('uuid,"last updated",title,"node type",delta', $file_path);
  }
  // Read the contents from the master file.
  $master_file = fopen(realpath($file_path), "r");
  while ($record = fgetcsv($master_file)) {
    $uuid = $record[0];
    $records[$uuid] = array(
      'uuid' => $uuid,
      'last_updated' => $record[1],
      'title' => $record[2],
      'node_type' => $record[3],
      'delta' => $record[4],
    );
  }
  fclose($master_file);
  // Unset heading.
  $uuid_column_name = 'uuid';
  unset($records[$uuid_column_name]);
  return $records;
}

/**
 * Takes a UUID and returns the contents of the associated file on disk (if it exists).
 */
function _baseline_content_get_file_contents_by_uuid($uuid) {
  $records = _baseline_content_read_master_file();
  if (!empty($records[$uuid])) {
    $record = $records[$uuid];
    $path = baseline_content_get_api_module_path();
    $path = drupal_realpath($path);
    $filename = _baseline_content_get_xml_filename_from_node_data($record['node_type'], $record['title'], $record['delta']);
    $handle = fopen($path . '/' . $filename, "r");
    watchdog('test', $path . '/' . $filename);
    $code_string = fread($handle, filesize($path . '/' . $filename));
    fclose($handle);
    return $code_string;
  }
  return FALSE;
}

/**
 * Write a set of records to the master file.
 */
function _baseline_content_write_master_file($records) {
  $path = baseline_content_get_api_module_path();
  $master_file = fopen(drupal_realpath($path) . '/node_export.csv', "w");
  $heading = array('uuid', 'last updated', 'title', 'node type', 'delta');
  fputcsv($master_file, $heading);
  foreach ($records as $record) {
    fputcsv($master_file, $record);
  }
  fclose($master_file);
}