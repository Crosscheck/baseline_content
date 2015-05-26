<?php

function baseline_content_export_nodes($nids, $force = FALSE, $all = FALSE) {
  putenv('BASELINE_CONTENT_EXPORT=TRUE');
  if (!is_array($nids)) {
    $nids = array($nids => $nids);
  }
  $records = _baseline_content_read_master_file();
  if (empty($nids)) {
    if (empty($all)) {
      // If no node IDs are specified and the --all flag is not specified,
      // register all tracked nodes for export.
      foreach ($records as $record) {
        $nid = _baseline_content_get_nid_by_uuid($record['uuid']);
        if (!empty($nid)) {
          $nids[$nid] = $nid;
        }
        else {
          drush_print('Node "' . $record['title'] . '" was not found in the database. Cannot export.');
        }
      }
    }
    else {
      // If the --all flag is passed, include all nodes known to Drupal.
      $result = db_select('node', 'n')->fields('n', array('nid'))->execute();
      $nids_in_db = array();
      foreach ($result as $record) {
        $nids_in_db[$record->nid] = $record->nid;
      }
      $new_nodes = count($nids_in_db) - count($records);
      if ($new_nodes > 0) {
        if (drush_confirm('Are you sure you want to add ' . $new_nodes . ' new nodes to the export? (total: ' . count($nids_in_db) . ')')) {
          $nids = array_merge($nids, $nids_in_db);
        }
        else {
          drush_user_abort();
          exit(0);
        }
      }
    }
  }
  if (!empty($nids)) {
    $nodes = node_load_multiple($nids);
    // If the --force flag is not given, do not export unmodified nodes.
    if (empty($force)) {
      _baseline_content_skip_unmodified_nodes($nodes, $records);
    }
    foreach ($nodes as $node) {
      _baseline_content_export_single_node($node);
    }
    _baseline_content_update_master_file_order();
  }
  else {
    drush_print('Baseline content could not find any nodes to export.');
  }
}

/**
 * Compares a list of provided nodes to what's stored in the master file.
 * If the timestamp is the same in the database as in the master file, the node
 * is considered unmodified and is removed from the list.
 */
function _baseline_content_skip_unmodified_nodes(&$nodes, $records = NULL) {
  if (!$records) {
    $records = _baseline_content_read_master_file();
  }
  foreach ($nodes as $node) {
    if (!empty($records[$node->uuid])) {
      $last_updated_in_db = _baseline_content_get_node_last_updated_by_uuid($node->uuid);
      $last_updated_in_file = $records[$node->uuid]['last_updated'];
      if ($last_updated_in_db == $last_updated_in_file) {
        drush_print('Skipping unmodified ' . $node->type . ' "' . $records[$node->uuid]['title'] . '".');
        unset($nodes[$node->nid]);
      }
    }
  }
}

/**
 * Loop over all the nodes referenced in the master file.
 * If one of the nodes is referenced by another node (based on UUID),
 * change the order of that record so it gets imported first.
 */
function _baseline_content_update_master_file_order() {
  $records = _baseline_content_read_master_file();
  $weight = 0;
  foreach ($records as &$record) {
    $record['file_contents'] = _baseline_content_get_file_contents_by_uuid($record['uuid']);
    $record['weight'] = $weight++;
  }
  usort($records, '_baseline_content_sort_import_order');
  foreach ($records as &$record) {
    unset($record['file_contents']);
    unset($record['weight']);
  }
  _baseline_content_write_master_file($records);
}

/**
 * Custom sorting function for _baseline_content_update_master_file_order().
 */
function _baseline_content_sort_import_order($a, $b) {
  if (strpos($b['file_contents'], $a['uuid']) !== FALSE) {
    return -1;
  }
  else {
    return $a['weight'] - $b['weight'];
  }
}

/**
 * Exports a single node.
 */
function _baseline_content_export_single_node($node) {
  $path = baseline_content_get_api_module_path();
  // Export the node to an XML file.
  $delta = 0; // TODO: handle delta (nodes with same title and type).
  $filename = _baseline_content_get_xml_filename_from_node_data($node->type, $node->title, $delta);
  $file_contents = _baseline_content_get_file_contents_by_uuid($node->uuid);
  putenv('BASELINE_CONTENT_EXPORT_FILE_CONTENTS=' . $file_contents);

  _baseline_content_node_export_export_callback($node->nid, $path . '/' . $filename);

  $records = _baseline_content_read_master_file();
  // (Over)write the record in the master file with the exported node's info.
  $records[$node->uuid] = array($node->uuid, $node->changed, $node->title, $node->type, $delta,);
  _baseline_content_write_master_file($records);
  drush_print($node->type . ' "' . $node->title . '" exported.');
}

/**
 * Given an object type, returns a list of blacklisted fields for export.
 */
function _baseline_content_get_fields_to_unset_during_export($object) {
  switch ($object) {
    case 'node':
      return array(
        'vid',
        'vuuid',
        'created', // Can be set using master file, to be identical to last updated.
        'changed', // Can be set using master file.
        'revision_timestamp',
        'path', // TODO: handle paths that deviate from pathauto pattern.
        'menu',  // Menu can be set using other, more appropriate migrations.
      );
      break;
    case 'panelizer':
      return array(
        'access',
        'contexts',
        'css',
        'css_class',
        'css_id',
        'extra',
        'name',
        'no_blocks',
        'panelizer_key',
        'panelizer_type',
        'pipeline',
        'relationships',
        'title',
        'title_element',
        'view_mode',
      );
      break;
    case 'display':
      return array(
        'cache',
      );
      break;
    case 'panel_pane':
      return array(
        'cache',
        'style',
        'css',
        'extras',
        'position',
        'locks',
      );
      break;
  }
}

/**
 * Remove all references to panel pane IDs in the Panelizer object and replace them with UUIDs.
 * Also unset some empty fields which randomly get added/removed by Panelizer.
 */
function _baseline_content_cleanup_panelizer_export(&$panelizer) {
// Get the exported panelizer and append it with "return $panelizer" to assign it to a variable.
  $panelizer_code = panelizer_export_export_callback($panelizer, NULL);
  $panelizer_code .= ' return $panelizer;';
  // Execute the exported panel as PHP code, giving us an object that represents the panel.
  $panelizer = eval($panelizer_code);

  // Unset unwanted empty fields.
  _baseline_content_unset_empty_fields_from_object('panelizer', $panelizer);
  foreach ($panelizer as &$display) {
    _baseline_content_unset_empty_fields_from_object('display', $display);
    if (!empty($display->content)) {
      foreach ($display->content as &$panel_pane) {
        _baseline_content_unset_empty_fields_from_object('panel_pane', $panel_pane);
      }
    }
  }
}

/**
 * Trim the export by unsetting certain empty fields from an object.
 */
function _baseline_content_unset_empty_fields_from_object($object_type, &$object) {
  $fields = '';
  switch ($object_type) {
    case 'panelizer':
      $fields = _baseline_content_get_fields_to_unset_during_export('panelizer');
      break;
    case 'display':
      $fields = _baseline_content_get_fields_to_unset_during_export('display');
      break;
    case 'panel_pane':
      $fields = _baseline_content_get_fields_to_unset_during_export('panel_pane');
      break;
  }
  foreach ($fields as $u) {
    // I forgot what this if-statement is for. But I'm sure it does something. I think.
    if (isset($object->{$u}) && empty($object->{$u}) && $object->{$u} !== NULL && $object->{$u} !== FALSE && $object->{$u} !== '0' && $object->{$u} !== 0) {
      unset($object->{$u});
    }
  }
}

/**
 * Implements hook_node_export_node_alter().
 */
function baseline_content_node_export_node_alter(&$node, &$original_node) {
  if (getenv('BASELINE_CONTENT_EXPORT')) {
    // When exporting a panelized node, take the Panelizer Entity object and run it through an
    // export cleanup function. This removes all references to panel-pane IDs and display IDs, relying
    // solely on UUIDs to glue everything together.
    if (!empty($node->panelizer)) {
      foreach ($node->panelizer as $key => &$panelizer) {
        _baseline_content_cleanup_panelizer_export($panelizer);
      }
    }
    // Unset fields which produce unwanted diff lines.
    $unwanted_node_fields = _baseline_content_get_fields_to_unset_during_export('node');
    foreach ($unwanted_node_fields as $u) {
      unset($node->{$u});
    }
    $entityreference_fields = _baseline_content_get_supported_entityreference_fields('node', $node->type);
    foreach ($entityreference_fields as $field => $field_info) {
      if (isset($node->{$field}[LANGUAGE_NONE])) {
        foreach ($node->{$field}[LANGUAGE_NONE] as &$entityreference) {
          unset($entityreference['target_id']);
        }
      }
    }
    $file_fields = _baseline_content_get_supported_file_fields('node', $node->type);
    foreach ($file_fields as $field => $field_info) {
      if (isset($node->{$field}[LANGUAGE_NONE])) {
        foreach ($node->{$field}[LANGUAGE_NONE] as &$file) {
          unset($file['fid']);
          // unset($file['uid']); TODO: replace user ID with UUID.
        }
      }
    }
  }
}

/**
 * Implements hook_node_export_encode_alter().
 */
function baseline_content_node_export_encode_alter(&$code_string, &$nodes, &$format_handler) {
  if (getenv('BASELINE_CONTENT_EXPORT')) {
    $unset = array(
      'nid',
      'revision_id',
    );
    foreach ($unset as $u) {
      $code_string = preg_replace('/(\n){1}(.)*\<' . $u . '\>(.)*\<\/' . $u . '\>/', '', $code_string);
    }
    // Remove the "export created on" timestamp.
    $code_string = preg_replace('/\<node_export created\=\"(.)*\">/', '<node_export>', $code_string);
  }
  // Add any existing Migrate settings.
  $file_contents = getenv('BASELINE_CONTENT_EXPORT_FILE_CONTENTS');
  if ($migrate_settings = _baseline_content_get_migrate_settings_xml_node($file_contents)) {
    $code_string = preg_replace('/\<node_export>/', '<node_export>' . PHP_EOL . $migrate_settings, $code_string);
  }
}

/**
 * Rewrite of the Node Export "export" callback.
 */
function _baseline_content_node_export_export_callback($nids_from_arg, $filename) {
  if (!is_array($nids_from_arg)) {
    $nids_from_arg = array($nids_from_arg => $nids_from_arg);
  }
  // Set up an array of nid_filters.
  $nid_filters = array();
  // The base nids.
  $args = array_filter($nids_from_arg, 'is_numeric');
  if ($args) {
    $nid_filters['base'] = $args;
  }
  $query = db_select('node', 'n')->fields('n', array('nid'));
  $execute = FALSE;
  if ($execute) {
    $result = $query->execute();
    foreach ($result as $row) {
      $nid_filters['filters'][] = $row->nid;
    }
  }
  if (count($nid_filters) > 1) {
    // Compute the intersect of all $nid_filters if there are more than one.
    $nids = call_user_func_array('array_intersect', $nid_filters);
  }
  elseif (count($nid_filters) == 1) {
    // Use the only filter if there is only one.
    $nids = reset($nid_filters);
  }
  else {
    // Is there are no filters at all, do a query to get all nids.
    $result = db_select('node', 'n')->fields('n', array('nid'))->execute();
    $nids = array();
    foreach ($result as $row) {
      $nids[] = $row->nid;
    }
  }
  // Handle format option.
  $format = 'xml';
  if (empty($nids)) {
    drush_set_error('DRUSH_NOT_COMPLETED', "No nodes found.");
  }
  $result = node_export($nids, $format, 'dt');
  if ($result['success']) {
    file_put_contents($filename, $result['output']);
  }
  else {
    // We have received an error message.
    drush_set_error('DRUSH_NOT_COMPLETED', strip_tags(implode("\n", $result['output'])));
  }
}