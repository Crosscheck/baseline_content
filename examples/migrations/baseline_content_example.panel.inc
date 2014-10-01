<?php

/**
 * @file
 * Panel migration.
 */

class FooContentPanel extends BaselineContentPanelMigration {


  public function __construct() {
    $module_path = drupal_get_path('module', 'foo_content');
    $import_path = $module_path . '/import/foo_content.panel.xml';
    parent::__construct($import_path);
    $this->description = t('Import Panels.');

    $this->addFieldMapping('node_id', 'node_id')
      ->xpath('arguments/source_infobox/title')
      ->sourceMigration('FooContentNodeInfobox');

    $this->addFieldMapping(NULL, 'handler')
      ->xpath('arguments/source_infobox/handler');

    $this->addFieldMapping(NULL, 'pane_id')
      ->xpath('arguments/source_infobox/pane_id');

    $this->dependencies = array(
      'FooContentHTMLBean',
      'FooContentNodeInfobox',
    );
  }

  public function prepare($page, $row) {
    if (!empty($row->handler) && !empty($row->pane_id) && !empty($row->node_id)) {
      // The specified pane contains a teaser of a particular node we want to show.
      // We need to replace the placeholder nid with the one from our source migration.
      $page->default_handlers[$row->handler]->conf['display']->content[$row->pane_id]->configuration['nid'] = $row->node_id;
    }
  }

  public function complete(stdClass $page, stdClass $row) {

  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    $fields = parent::getSourceFields();
    $fields['arguments'] = t('Arguments');
    return $fields;
  }
}
