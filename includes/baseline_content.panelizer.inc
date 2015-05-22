<?php

/**
 * The form where users can select the Panelizer settings they wish to export.
 */
function baseline_content_panelizer_input_form($form, &$form_state) {
  $form['help_text'] = array(
    '#markup' => "<p>This form lets you export this Drupal site's Panelizer settings, for re-import on another environment using the Baseline Content module. <br/>
    Note that you will need to properly configure the source migration for your Panelizer Entities' migration handler.<br/>
    For an example on how this is done, see the \"examples\" folder in the baseline_content module.</p>"
  );
  $default_options = baseline_content_panelizer_get_defaults_options();
  $entities_options = baseline_content_panelizer_get_entities_options();
  $form['panelizer_defaults'] = array(
    '#type' => 'checkboxes',
    '#options' => $default_options,
    '#title' => t('Panelizer Defaults'),
    '#default_value' => array_flip($default_options),
  );
  $form['panelizer_entity'] = array(
    '#type' => 'checkboxes',
    '#options' => $entities_options,
    '#title' => t('Panelizer Entities'),
    '#default_value' => array_flip($entities_options),
  );
  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Export selected settings'),
  );
  return $form;
}

/**
 * @return array A list of Panelizer defaults. Key is the panel's did,
 * value is the name of the default settings (such as node:page:default).
 */
function baseline_content_panelizer_get_defaults_options() {
  $query = db_select('panelizer_defaults', 'pd');
  $query->fields('pd', array('name', 'did'));
  $query->condition('pd.name', 'NULL', '<>');
  $result = $query->execute();
  $output = array();
  // Panelizer sometimes saves multiple versions of the same default layout.
  // We're only interested in the latest one.
  foreach ($result as $record) {
    $output[$record->name] = $record->did;
  }
  return array_flip($output);
}

/**
 * @return array A list of Panelizer entities. Key is the entity's ID,
 * value is the content type of the entity followed by its title.
 */
function baseline_content_panelizer_get_entities_options() {
  $query = db_select('panelizer_entity', 'pe');
  $query->fields('pe', array('entity_id'));
  $result = $query->execute();
  $output = array();
  // Panelizer sometimes saves multiple versions of the same default layout.
  // We're only interested in the latest one.
  foreach ($result as $record) {
    $node = node_load($record->entity_id);

    $output['[' . $node->type . '] ' . $node->title] = $record->entity_id;
  }
  return array_flip($output);
}

/**
 * Make sure at least one checkbox is checked.
 */
function baseline_content_panelizer_input_form_validate($form, &$form_state) {
  $at_least_one_value = FALSE;
  foreach ($form_state['values']['panelizer_defaults'] as $value) {
    if (!empty($value)) {
      $at_least_one_value = TRUE;
    }
  }
  foreach ($form_state['values']['panelizer_entity'] as $value) {
    if (!empty($value)) {
      $at_least_one_value = TRUE;
    }
  }
  if(!$at_least_one_value) {
    form_error($form, t('Please select at least one value.'));
  }
}

/**
 * Redirect the user to the export page. Panelizer defaults and entities
 * are passed as a querystring, seperated by underscores.
 */
function baseline_content_panelizer_input_form_submit($form, &$form_state) {
  $panelizer_defaults = baseline_content_panelizer_parse_input_to_querystring('panelizer_defaults', $form_state);
  $panelizer_entity = baseline_content_panelizer_parse_input_to_querystring('panelizer_entity', $form_state);
  drupal_goto('admin/config/content/baseline_content/panelizer/export/xml', array(
    'query' => array(
      'panelizer_defaults' => $panelizer_defaults,
      'panelizer_entity' => $panelizer_entity,
    ),
  ));
}

/**
 * Grab the IDs from the given inputform and turn them into querystring parameters.
 * Adds underscores when there's more than one ID.
 */
function baseline_content_panelizer_parse_input_to_querystring($output_type, &$form_state) {
  $output = '';
  $first = 0;
  foreach ($form_state['values'][$output_type] as $value) {
    if (!empty($value)) {
      if ($first++ != 0) {
        $output .= '_';
      }
      $output .= $value;
    }
  }
  return $output;
}

/**
 * The output page. Output is displayed in textareas, which makes it
 * easier to copy-paste the text.
 */
function baseline_content_panelizer_export_form($form, &$form_state) {
  $form['help_text'] = array(
    '#markup' => '<p>Copy-paste the output into the import file used by your Baseline Content migration process. <br/>
    For an example of how these are used, see the "examples" folder in the baseline_content module.</p>'
  );
  $output = baseline_content_panelizer_generate_output('panelizer_defaults');
  baseline_content_panelizer_render_output($output, 'panelizer_defaults', $form);

  $output = baseline_content_panelizer_generate_output('panelizer_entity');
  baseline_content_panelizer_render_output($output, 'panelizer_entity', $form);
  return $form;
}

/**
 * Base function to generate XML for both defaults and entities.
 * @param $output_type string Can be either 'panelizer_defaults' or 'panelizer_entity'.
 * @return array A list of XML nodes.
 */
function baseline_content_panelizer_generate_output($output_type) {
  $output = array();
  $querystring = drupal_get_query_parameters();
  if (!empty($querystring[$output_type])) {
    $ids = explode('_', $querystring[$output_type]);
  }
  if (!empty($ids)) {
    $output[] = '<?xml version="1.0" encoding="UTF-8"?>';
    $output[] = '<panelizer>';
    foreach ($ids as $id) {
      $output[] = '<panelize>';
      if ($output_type == 'panelizer_defaults') {
        baseline_content_panelizer_add_defaults($output, $id);
      }
      else if ($output_type == 'panelizer_entity') {
        baseline_content_panelizer_add_entities($output, $id);
      }
      $output[] = '</panelize>';
    }
    $output[] = '</panelizer>';
  }
  return $output;
}

/**
 * Takes a list of XML nodes and renders them on a form field.
 * Indentation is added to make the output (slightly) easier to read.
 */
function baseline_content_panelizer_render_output($output_array, $output_type, &$form) {
  $indent = 0;
  $output_text = '';
  foreach ($output_array as $index => $node) {
    if ($index == 1) {
      // Do not indent the second line, since the <?xml> tag is above it.
    }
    else {
      if (strpos($node, '</') === 0) {
        // If this XML node is a closing tag, reduce the indent.
        $indent -= 2;
      }
      else if ($index != 0) {
        // If this XML node is not a closing tag and is not the first line:
        if (strpos($output_array[$index - 1], '</') === 0 && strpos($output_array[$index], '<') === 0 && strpos($output_array[$index], '</') !== 0) {
          // If the previous XML node contains a closing tag and this node contains an opening tag,
          // keep the indent the same.
        }
        elseif (strpos($output_array[$index - 1], '</') === 0) {
          // If the previous XML node is a closing tag, reduce the indent.
          $indent -= 2;
        }
        elseif (strpos($output_array[$index - 1], '</')) {
          // If the previous XML node contains both an opening and a closing tag,
          // (such as <key>value</key>), keep the indent the same.
        }
        elseif (strpos($output_array[$index - 1], '<') === 0){
          // If the previous XML node is an opening tag, increase the indent.
          $indent += 2;
        }
      }
      $count = $indent;
      $spaces = '';
      if ($count > 0) {
        while ($count--) {
          $spaces .= ' ';
        }
      }
    }
    $output_text .= $spaces . $node . "\n";
  }
  if ($output_type == 'panelizer_entity') {
    $form['panelizer_entity'] = array(
      '#type' => 'textarea',
      '#title' => t('Panelizer Entity output'),
      '#default_value' => $output_text,
    );
  }
  if ($output_type == 'panelizer_defaults') {
    $form['panelizer_defaults'] = array(
      '#type' => 'textarea',
      '#title' => t('Panelizer Defaults output'),
      '#default_value' => $output_text,
    );
  }
}

/**
 * Adds the <variables> and the <panelizer_settings> XML
 * to the Panelizer Defaults output.
 */
function baseline_content_panelizer_add_defaults(&$output, $did) {
  $query = db_select('panelizer_defaults', 'pdef');
  $query->fields('pdef', array('name', 'panelizer_key'));
  $query->condition('pdef.did', $did);
  $query->condition('pdef.name', 'NULL', '<>');
  $result = $query->execute();
  foreach ($result as $record) {
    // Should only be 1.
    baseline_content_xml_add_single($output, 'description', $record->name);
    baseline_content_panelizer_add_variables($output, $record);
    baseline_content_add_defaults_export($output, $record);
  }
}

function baseline_content_add_defaults_export(&$output, $record) {
  $output[] = '<panelizer_defaults_export><![CDATA[';
  $existing_panelizer = panelizer_context_cache_get('default', $record->name);
  $export = panelizer_export_export_callback($existing_panelizer, NULL);
  $export_array = explode("\n", $export);
  $output = array_merge($output, $export_array);
  $output[] = ']]>';
  $output[] = '</panelizer_defaults_export>';
}

/**
 * Adds the <panelizer_settings> XML to the Panelizer Entities output.
 */
function baseline_content_panelizer_add_entities(&$output, $entity_id) {
  $query = db_select('panelizer_entity', 'pe');
  $query->fields('pe', array('entity_id', 'revision_id'));
  $query->condition('pe.entity_id', $entity_id);
  $result = $query->execute();
  // Only grab the latest revision.
  // TODO: Add revision support?
  $highest_revision = array();
  foreach ($result as $record) {
    $highest_revision[$record->entity_id] = $record->revision_id;
  }
  // TODO: don't execute this query twice.
  $result = $query->execute();
  foreach ($result as $record) {
    foreach ($highest_revision as $nid => $rid) {
      if ($record->entity_id == $nid && $record->revision_id == $rid) {
        $node = node_load($record->entity_id);
        baseline_content_xml_add_single($output, 'description', '[' . $node->type . '] ' . $node->title);
        baseline_content_xml_add_single($output, 'entity_title', $node->title);
        baseline_content_add_entities_export($output, $node);
      }
    }
  }
}

function baseline_content_add_entities_export(&$output, $node) {
  $output[] = '<panelizer_entity_export><![CDATA[';
  $existing_panelizer = $node->panelizer['page_manager'];
  $export = panelizer_export_export_callback($existing_panelizer, NULL);
  $export_array = explode("\n", $export);
  $output = array_merge($output, $export_array);
  $output[] = ']]>';
  $output[] = '</panelizer_entity_export>';
}

/**
 * Adds the <variables> XML to the Panelizer Defaults output.
 */
function baseline_content_panelizer_add_variables(&$output, $record) {
  $output[] = '<variables>';
  // Panelizer stores a variable for Defaults, which further dictates their settings.
  // We need to include this in the export file.
  $variable = serialize(variable_get('panelizer_defaults_node_' . $record->panelizer_key));
  $output[] = '<panelizer_defaults_node_' . $record->panelizer_key . '>' . $variable . '</panelizer_defaults_node_' . $record->panelizer_key . '>';
  $output[] = '</variables>';
}
