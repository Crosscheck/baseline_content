<?php

/**
 * @file
 * Panel migration.
 *
 * Tutorial: How to export / import (mini-) panels.
 *
 * You may notice that the bulk of the Panel data is in this Migration handler
 * class instead of the import XML file. While counter-intuitive, the simple reason
 * behind this is that the Panels module provides a tab that allows you to "Export"
 * a panel, which generates the required PHP code to re-import the Panel elsewhere.
 * This approach allows you to simple copy-paste the Panel export code directly
 * from the UI into the appropriate function (see examples below).
 *
 * Your Panel migration handler will need to implement getPanel(), which takes
 * as parameter the Panel ID from the XML import file. Based on that ID, this class
 * will return an object representing the Panel page.
 *
 * NOTE: the function MUST "return" the panel, else this won't work!
 * Do a search for "return $page" in this file to understand what is meant by this.
 */

class BaselineContentExamplePanelMigration extends BaselineContentPanelMigration {

/**
 * This example Panel migration imports a Panel object, the code for which can be
 * exported using Panels UI. The two Panels in this example both contain a
 * Panel pane which holds a "teaser" of a particular node we want to show.
 *
 * Since the ID of this node in the Panel export will almost always be different
 * from the ID in the destination environment, we will make use of a source migration
 * to inject the correct node ID.
 */

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content_example');
    $import_path = $module_path . '/examples/mport/baseline_content_example.panel.xml';
    parent::__construct($import_path);
    $this->description = t('Import Panels.');

    $this->addFieldMapping('node_id', 'node_id')
      ->xpath('arguments/source_infobox/title')
      ->sourceMigration('BaselineContentExampleNodeMigration');

    $this->addFieldMapping(NULL, 'handler')
      ->xpath('arguments/source_infobox/handler');

    $this->addFieldMapping(NULL, 'pane_id')
      ->xpath('arguments/source_infobox/pane_id');

    // Declare all taxonomy and file migrations as dependencies.
    $this->dependencies = array(
      'BaselineContentExampleNodeMigration',
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
 * Takes a Panel ID as parameter and returns a fully fleshed-out Panel Page
 * (the code for which can be exported from the UI).
 * @param  string $id The Panel ID from the import XML.
 * @return array      An array which holds two elements:
 * "panel" => the Page object representing the Panel.
 * "type" => can either be "panel" (in the case of a regular panel)
 * or "mini" (in the case of a mini panel).
 */
  public function getPanel($id) {
    if ($id == 'publications') {
      return array(
        'panel' => $this->_panel_publications(),
        'type' => 'panel', // Can be "panel" or "mini" in the case of Mini Panels.
      );
    }
    else if ($id == 'frequently_asked_questions') {
      return array(
        'panel' => $this->_panel_frequently_asked_questions(),
        'type' => 'panel',
      );
    }
    return NULL;
  }

/**
 * The code in this function has been copy-pasted directly from what Panels UI
 * generates when you click on the "Export" tab.
 * Note the "return $page;" line at the end of the function!
 */
  public function _panel_publications() {
    $page = new stdClass();
    $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
    $page->api_version = 1;
    $page->name = 'publications';
    $page->task = 'page';
    $page->admin_title = 'Publications';
    $page->admin_description = '';
    $page->path = 'publications';
    $page->access = array();
    $page->menu = array();
    $page->arguments = array();
    $page->conf = array(
      'admin_paths' => FALSE,
    );
    $page->default_handlers = array();
    $handler = new stdClass();
    $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
    $handler->api_version = 1;
    $handler->name = 'page_publications_panel_context';
    $handler->task = 'page';
    $handler->subtask = 'publications';
    $handler->handler = 'panel_context';
    $handler->weight = 0;
    $handler->conf = array(
      'title' => 'Panel',
      'no_blocks' => 0,
      'pipeline' => 'ipe',
      'body_classes_to_remove' => '',
      'body_classes_to_add' => '',
      'css_id' => '',
      'css' => '',
      'contexts' => array(),
      'relationships' => array(),
    );
    $display = new panels_display();
    $display->layout = 'knowledge_center';
    $display->layout_settings = array();
    $display->panel_settings = array(
      'style_settings' => array(
        'default' => NULL,
        'top_left' => NULL,
        'top_right' => NULL,
        'bottom' => NULL,
        'main_content' => NULL,
        'widgets' => NULL,
        'more' => NULL,
      ),
    );
    $display->cache = array();
    $display->title = 'Publications';
    $display->uuid = 'f3cf0e98-df50-479e-8eb8-516c707d3fff';
    $display->content = array();
    $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-7688aa9f-7ca4-46d7-adab-88c1336fec92';
    $pane->panel = 'bottom_left';
    $pane->type = 'block';
    $pane->subtype = 'bean-looking-for-info';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '7688aa9f-7ca4-46d7-adab-88c1336fec92';
    $display->content['new-7688aa9f-7ca4-46d7-adab-88c1336fec92'] = $pane;
    $display->panels['bottom_left'][0] = 'new-7688aa9f-7ca4-46d7-adab-88c1336fec92';
    $pane = new stdClass();
    $pane->pid = 'new-6a1def86-fbff-46a6-9939-ce680c6ebd48';
    $pane->panel = 'bottom_right';
    $pane->type = 'block';
    $pane->subtype = 'bean-complaint-or-remark';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '6a1def86-fbff-46a6-9939-ce680c6ebd48';
    $display->content['new-6a1def86-fbff-46a6-9939-ce680c6ebd48'] = $pane;
    $display->panels['bottom_right'][0] = 'new-6a1def86-fbff-46a6-9939-ce680c6ebd48';
    $pane = new stdClass();
    $pane->pid = 'new-6729a394-8754-4d87-9410-b7ca7bd353a3';
    $pane->panel = 'bottom_right';
    $pane->type = 'block';
    $pane->subtype = 'bean-frequently-asked-questions';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '6729a394-8754-4d87-9410-b7ca7bd353a3';
    $display->content['new-6729a394-8754-4d87-9410-b7ca7bd353a3'] = $pane;
    $display->panels['bottom_right'][1] = 'new-6729a394-8754-4d87-9410-b7ca7bd353a3';
    $pane = new stdClass();
    $pane->pid = 'new-2ce685a5-48b2-479c-b771-5b5160199bc4';
    $pane->panel = 'main_content';
    $pane->type = 'views_panes';
    $pane->subtype = 'publications-publications_top';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '2ce685a5-48b2-479c-b771-5b5160199bc4';
    $display->content['new-2ce685a5-48b2-479c-b771-5b5160199bc4'] = $pane;
    $display->panels['main_content'][0] = 'new-2ce685a5-48b2-479c-b771-5b5160199bc4';
    $pane = new stdClass();
    $pane->pid = 'new-d8a185ba-7274-4eb0-85e8-b03989445a68';
    $pane->panel = 'more';
    $pane->type = 'views';
    $pane->subtype = 'articles';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_pager_settings' => 0,
      'use_pager' => 0,
      'nodes_per_page' => '4',
      'pager_id' => '0',
      'offset' => '0',
      'more_link' => 0,
      'feed_icons' => 0,
      'panel_args' => 0,
      'link_to_view' => 0,
      'args' => '',
      'url' => '',
      'display' => 'sitewide_latest_articles',
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = 'd8a185ba-7274-4eb0-85e8-b03989445a68';
    $display->content['new-d8a185ba-7274-4eb0-85e8-b03989445a68'] = $pane;
    $display->panels['more'][0] = 'new-d8a185ba-7274-4eb0-85e8-b03989445a68';
    $pane = new stdClass();
    $pane->pid = 'new-74a4bdfd-ff66-4282-8c1d-1ffe93080b5a';
    $pane->panel = 'widgets';
    $pane->type = 'block';
    $pane->subtype = 'views-0ca45cb4186463c59321d7adbf35ce7f';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'inherit_path' => 0,
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '74a4bdfd-ff66-4282-8c1d-1ffe93080b5a';
    $display->content['new-74a4bdfd-ff66-4282-8c1d-1ffe93080b5a'] = $pane;
    $display->panels['widgets'][0] = 'new-74a4bdfd-ff66-4282-8c1d-1ffe93080b5a';
    $pane = new stdClass();
    $pane->pid = 'new-79dbeba6-9178-4d14-8f78-7e3ed6586f31';
    $pane->panel = 'widgets';
    $pane->type = 'node';
    $pane->subtype = 'node';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'nid' => '6084',
      'links' => 1,
      'leave_node_title' => 1,
      'identifier' => '',
      'build_mode' => 'teaser',
      'link_node_title' => 0,
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '79dbeba6-9178-4d14-8f78-7e3ed6586f31';
    $display->content['new-79dbeba6-9178-4d14-8f78-7e3ed6586f31'] = $pane;
    $display->panels['widgets'][1] = 'new-79dbeba6-9178-4d14-8f78-7e3ed6586f31';
    $pane = new stdClass();
    $pane->pid = 'new-258aa202-b3bd-452d-89bc-25dee29db1b8';
    $pane->panel = 'widgets';
    $pane->type = 'block';
    $pane->subtype = 'feature_publication-feature_publication_block_tags';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '258aa202-b3bd-452d-89bc-25dee29db1b8';
    $display->content['new-258aa202-b3bd-452d-89bc-25dee29db1b8'] = $pane;
    $display->panels['widgets'][2] = 'new-258aa202-b3bd-452d-89bc-25dee29db1b8';
    $display->hide_title = PANELS_TITLE_FIXED;
    $display->title_pane = '0';
    $handler->conf['display'] = $display;
    $page->default_handlers[$handler->name] = $handler;

    return $page;
  }

/**
 * The code in this function has been copy-pasted directly from what Panels UI
 * generates when you click on the "Export" tab.
 * Note the "return $page;" line at the end of the function!
 */
  public function _panel_frequently_asked_questions() {
    $page = new stdClass();
    $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
    $page->api_version = 1;
    $page->name = 'frequently_asked_questions';
    $page->task = 'page';
    $page->admin_title = 'Frequently asked questions';
    $page->admin_description = '';
    $page->path = 'frequently-asked-questions';
    $page->access = array();
    $page->menu = array();
    $page->arguments = array();
    $page->conf = array(
      'admin_paths' => FALSE,
    );
    $page->default_handlers = array();
    $handler = new stdClass();
    $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
    $handler->api_version = 1;
    $handler->name = 'page_frequently_asked_questions_panel_context';
    $handler->task = 'page';
    $handler->subtask = 'frequently_asked_questions';
    $handler->handler = 'panel_context';
    $handler->weight = 0;
    $handler->conf = array(
      'title' => 'Panel',
      'no_blocks' => 0,
      'pipeline' => 'ipe',
      'body_classes_to_remove' => '',
      'body_classes_to_add' => '',
      'css_id' => '',
      'css' => '',
      'contexts' => array(),
      'relationships' => array(),
    );
    $display = new panels_display();
    $display->layout = 'knowledge_center';
    $display->layout_settings = array();
    $display->panel_settings = array(
      'style_settings' => array(
        'default' => NULL,
        'main_content' => NULL,
        'widgets' => NULL,
        'more' => NULL,
        'bottom' => NULL,
      ),
    );
    $display->cache = array();
    $display->title = '';
    $display->uuid = '084f06c3-beea-4cee-917f-6061144137f5';
    $display->content = array();
    $display->panels = array();
    $pane = new stdClass();
    $pane->pid = 'new-4ec2055a-030c-42a7-ac4b-9cd467374c99';
    $pane->panel = 'bottom_left';
    $pane->type = 'block';
    $pane->subtype = 'bean-looking-for-info';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '4ec2055a-030c-42a7-ac4b-9cd467374c99';
    $display->content['new-4ec2055a-030c-42a7-ac4b-9cd467374c99'] = $pane;
    $display->panels['bottom_left'][0] = 'new-4ec2055a-030c-42a7-ac4b-9cd467374c99';
    $pane = new stdClass();
    $pane->pid = 'new-7dce46a1-9c17-4610-b8bd-b2a7c28a775b';
    $pane->panel = 'bottom_right';
    $pane->type = 'block';
    $pane->subtype = 'bean-complaint-or-remark';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '7dce46a1-9c17-4610-b8bd-b2a7c28a775b';
    $display->content['new-7dce46a1-9c17-4610-b8bd-b2a7c28a775b'] = $pane;
    $display->panels['bottom_right'][0] = 'new-7dce46a1-9c17-4610-b8bd-b2a7c28a775b';
    $pane = new stdClass();
    $pane->pid = 'new-2b9bc76a-8d1d-4fb7-a6b5-ed69e8a60275';
    $pane->panel = 'bottom_right';
    $pane->type = 'block';
    $pane->subtype = 'bean-frequently-asked-questions';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = '2b9bc76a-8d1d-4fb7-a6b5-ed69e8a60275';
    $display->content['new-2b9bc76a-8d1d-4fb7-a6b5-ed69e8a60275'] = $pane;
    $display->panels['bottom_right'][1] = 'new-2b9bc76a-8d1d-4fb7-a6b5-ed69e8a60275';
    $pane = new stdClass();
    $pane->pid = 'new-1e4eecdb-7ffa-415b-8071-799e1775c4d7';
    $pane->panel = 'main_content';
    $pane->type = 'views_panes';
    $pane->subtype = 'faq-faq_top';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array();
    $pane->cache = array();
    $pane->style = array(
      'style' => 'block',
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '1e4eecdb-7ffa-415b-8071-799e1775c4d7';
    $display->content['new-1e4eecdb-7ffa-415b-8071-799e1775c4d7'] = $pane;
    $display->panels['main_content'][0] = 'new-1e4eecdb-7ffa-415b-8071-799e1775c4d7';
    $pane = new stdClass();
    $pane->pid = 'new-76429303-75c8-49ff-9bde-ba6dae7f99ed';
    $pane->panel = 'widgets';
    $pane->type = 'block';
    $pane->subtype = 'views-4501fc41180cb361199dee91d6707535';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'inherit_path' => 0,
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 0;
    $pane->locks = array();
    $pane->uuid = '76429303-75c8-49ff-9bde-ba6dae7f99ed';
    $display->content['new-76429303-75c8-49ff-9bde-ba6dae7f99ed'] = $pane;
    $display->panels['widgets'][0] = 'new-76429303-75c8-49ff-9bde-ba6dae7f99ed';
    $pane = new stdClass();
    $pane->pid = 'new-e14165ed-3df2-4563-8cc1-b99bd034b3eb';
    $pane->panel = 'widgets';
    $pane->type = 'node';
    $pane->subtype = 'node';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'nid' => '6084',
      'links' => 1,
      'leave_node_title' => 1,
      'identifier' => '',
      'build_mode' => 'teaser',
      'link_node_title' => 0,
      'override_title' => 1,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 1;
    $pane->locks = array();
    $pane->uuid = 'e14165ed-3df2-4563-8cc1-b99bd034b3eb';
    $display->content['new-e14165ed-3df2-4563-8cc1-b99bd034b3eb'] = $pane;
    $display->panels['widgets'][1] = 'new-e14165ed-3df2-4563-8cc1-b99bd034b3eb';
    $pane = new stdClass();
    $pane->pid = 'new-538292ab-3cf0-4206-95f7-4bf554bc5dd4';
    $pane->panel = 'widgets';
    $pane->type = 'block';
    $pane->subtype = 'feature_faq-feature_faq_block_tags';
    $pane->shown = TRUE;
    $pane->access = array();
    $pane->configuration = array(
      'override_title' => 0,
      'override_title_text' => '',
    );
    $pane->cache = array();
    $pane->style = array(
      'settings' => NULL,
    );
    $pane->css = array();
    $pane->extras = array();
    $pane->position = 2;
    $pane->locks = array();
    $pane->uuid = '538292ab-3cf0-4206-95f7-4bf554bc5dd4';
    $display->content['new-538292ab-3cf0-4206-95f7-4bf554bc5dd4'] = $pane;
    $display->panels['widgets'][2] = 'new-538292ab-3cf0-4206-95f7-4bf554bc5dd4';
    $display->hide_title = PANELS_TITLE_FIXED;
    $display->title_pane = '0';
    $handler->conf['display'] = $display;
    $page->default_handlers[$handler->name] = $handler;

    return $page;
  }
}
