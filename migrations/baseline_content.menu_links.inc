<?php

/**
 * @file
 *  Base Migrations for Menu Links.
 */

class BaselineContentMenuLinksMigration extends Migration {

  /**
   * BaselineContentMenuLinksMigration constructor.
   *
   * @param string $import_path
   *   Path to the CSV file containing the menu links, relative to the drupal
   *   directory.
   * @param string $content_source
   *   Optional. The class name of the migration containing nodes this migration
   *   references.
   */
  public function __construct($import_path, $content_source = '') {
    parent::__construct();
    $this->description = t('Import menu links.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'machine_name' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationMenuLinks::getKeySchema()
    );

    $this->source = new MigrateSourceCSV($import_path, $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationMenuLinks();
    $this->addFieldMapping('menu_name', 'menu_name');
    $this->addFieldMapping('link_path', 'link_path');
    $this->addFieldMapping('link_title', 'link_title');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('module', 'module');
    $this->addFieldMapping('plid', 'parent')->sourceMigration(array($this->machineName));
    if (!empty($content_source)) {
      $this->addFieldMapping('node', 'node')->sourceMigration(array($content_source));
    }
  }

  /**
   * Implements prepareRow().
   *
   * - Add the menu link weight.
   * - Set module to menu to support menu_firstchild.
   */
  public function prepareRow($row) {
    static $weight;
    if (empty($weight)) {
      $weight = 0;
    }
    $row->weight = ++$weight;
    $row->module = 'menu';
  }

  function csvcolumns() {
    $columns[] = array('menu_name', 'Menu');
    $columns[] = array('machine_name', 'Machine name');
    $columns[] = array('link_path', 'Path');
    $columns[] = array('link_title', 'Title');
    $columns[] = array('parent', 'Parent');
    $columns[] = array('node', 'Node');
    return $columns;
  }

  /**
   * Implements MigrateDestination::prepare().
   *
   * Set up the node.
   */
  public function prepare($menu_link, stdClass $row) {
    if (!empty($menu_link->node)) {
      $menu_link->link_path = 'node/' . $menu_link->node;
      unset($menu_link->node);
    }
  }
}
