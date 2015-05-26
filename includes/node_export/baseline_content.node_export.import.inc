<?php

/**
 * Implements hook_node_export_after_import_alter().
 */
function baseline_content_node_export_after_import_alter($new_nodes, $used_format, $save) {
  // Field Collection Deploy re-saves the node, which causes issues with Panelizer.
  // For this reason, we need to remove the entry from the panelizer_entity table before
  // attempting to re-save.
  if (getenv('BASELINE_CONTENT_IMPORT')) {
    foreach ($new_nodes as $key => $node) {
      if (isset($node->field_collection_deploy)) {
        $query = db_delete('panelizer_entity');
        $query->condition('entity_id', $node->nid);
        $query->execute();
      }
    }
  }
}

/**
 * Implements hook_node_export_node_import_alter().
 */
function baseline_content_node_export_node_import_alter(&$node, &$original_node, &$save) {
  if (getenv('BASELINE_CONTENT_IMPORT')) {
    // When importing a node that already exists in the database, overwrite it and preserve the node ID.
    if (!empty($original_node->uuid)) {
      $nid = _baseline_content_get_nid_by_uuid($original_node->uuid);
      if (!empty($nid)) {
        $node->is_new = FALSE;
        $node->uuid = $original_node->uuid;
        $node->nid = $nid;
        $node->revision = 1;
        $save = TRUE;
      }
    }
    // When importing Panelizer, save the display first so we have a display ID to work with.
    if (!empty($node->panelizer)) {
      foreach ($node->panelizer as $key => &$panelizer) {
        panels_save_display($panelizer->display);
        $panelizer->did = $panelizer->display->did;
      }
    }
    // Map entity reference UUIDs.
    $entityreference_fields = _baseline_content_get_supported_entityreference_fields('node', $node->type);
    foreach ($entityreference_fields as $field => $field_info) {
      switch ($field_info['settings']['target_type']) {
        case 'taxonomy_term':
          _baseline_content_populate_entityreference_fields($node, 'taxonomy_term_data', 'tid');
          break;
        case 'node':
          _baseline_content_populate_entityreference_fields($node, 'node', 'nid');
          break;
      }
    }
    // Map File UUIDs.
    $file_fields = _baseline_content_get_supported_file_fields('node', $node->type);
    foreach ($file_fields as $field => $field_info) {
      if (isset($node->{$field}[LANGUAGE_NONE])) {
        foreach ($node->{$field}[LANGUAGE_NONE] as &$file) {
          $fid = db_select('file_managed', 'f')->fields('f', array('fid'))->condition('f.uuid', $file['uuid'])->execute()->fetchField();
          $file['fid'] = $fid;
        }
      }
    }
  }
}

/**
 * Map the entityreference UUID onto entityreference "target_id".
 */
function _baseline_content_populate_entityreference_fields(&$node, $table, $field) {
  if (isset($node->{$field}[LANGUAGE_NONE])) {
    foreach ($node->{$field}[LANGUAGE_NONE] as &$reference) {
      $reference['target_id'] = db_select($table, 't')
        ->fields('t', array($field))
        ->condition('t.uuid', $reference['uuid'])
        ->execute()
        ->fetchField();
    }
  }
}

/**
 * @param array $uuids A list of UUIDs to import. The UUIDs need to be specified
 * in files on disk.
 * @param bool $force Force import regardless of whether or not the node has been
 * modified since last import.
 */
function baseline_content_import_nodes($uuids, $force = FALSE) {
  putenv('BASELINE_CONTENT_IMPORT=TRUE');
  IF (!is_array($uuids)) {
    $uuids = array($uuids => $uuids);
  }
  $path = baseline_content_get_api_module_path();
  $records = _baseline_content_read_master_file();
  if ($uuids) {
    _baseline_content_unset_invalid_uuids($uuids, $records);
  }
  if (!empty($records)) {
    // Import source XMLs as Drupal nodes.
    if (!empty($path)) {
      $files = _baseline_content_get_metadata_from_files($path);
      foreach ($records as $uuid => $record) {
        if (empty($force)) {
          $record_is_newer = _baseline_content_importfile_is_newer($uuid, $records);
          if (!$record_is_newer) {
            drush_print('Skipping unmodified ' . $record['node_type'] . ' "' . $record['title'] . '".');
            continue;
          }
        }
        $filename = _baseline_content_get_xml_filename_from_node_data($record['node_type'], $record['title'], $record['delta']);
        _baseline_content_import_single_node($files[$filename], $record['uuid'], $record['last_updated']);
      }
    }
  }
  else {
    drush_print('Baseline Content could not find any new nodes to import.');
  }
}

function _baseline_content_unset_invalid_uuids($uuids, &$records) {
// If UUIDs are specified, validate them first.
  $uuids = array_combine($uuids, $uuids);
  foreach ($uuids as $uuid) {
    if (!array_key_exists($uuid, $records)) {
      drush_print('UUID "' . $uuid . '" not found in the master file.');
      unset($uuids[$uuid]);
    }
  }
  // ...then remove from the import all records that weren't specified.
  foreach ($records as $uuid => $record) {
    if (!array_key_exists($uuid, $uuids)) {
      unset($records[$uuid]);
    }
  }
}

/**
 * Implements hook_node_node_export_decode_alter().
 */
function baseline_content_node_export_decode_alter(&$code_string) {
  if (getenv('BASELINE_CONTENT_IMPORT')) {
    // Do not import the Migrate settings.
    $code_string = preg_replace('/\<migrate\>(.)*\<\/migrate\>/Us', '', $code_string);
  }
}

/**
 * Imports a single node.
 */
function _baseline_content_import_single_node($import_file, $uuid, $last_updated) {
  $file_path = drupal_realpath($import_file->uri);

  _baseline_content_node_export_import_callback($file_path);

  // Import twice for stupid fieldcollection bug.
  // TODO: fix this somehow since it's causing the import to take almost twice as long.
  _baseline_content_node_export_import_callback($file_path, TRUE);

  // Since newly imported files get a "last updated" timestamp set to "now", we need
  // to update the database with the "last updated" timestamp from the source XML.
  // This is needed so bcen doesn't produce false positives.
  db_update('node')
    ->fields(array('changed' => $last_updated, 'created' => $last_updated,))
    ->condition('uuid', $uuid)
    ->execute();
}

/**
 * Rewrite of the Node Export "import" callback.
 */
function _baseline_content_node_export_import_callback($file, $second_time = FALSE) {
  // Switch to admin user so imported nodes are not anonymous.
  global $user;
  $user = user_load(1);
  if ($file) {
    $node_code = file_get_contents($file, "r");
  }
  else {
    $node_code = file_get_contents("php://stdin", "r");
  }
  if (!empty($node_code)) {
    $result = node_export_import($node_code, 'dt');
    if (!$second_time) {
      if (!$result['success']) {
        drush_set_error('DRUSH_NOT_COMPLETED', strip_tags(implode("\n", $result['output'])));
      }
    }
  }
}
