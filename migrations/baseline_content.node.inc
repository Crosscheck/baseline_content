<?php

/**
 * @file
 *  Base Migrations for nodes.
 */

/**
 * Base Migration for nodes containing complex data.
 *
 * Features:
 *
 * - Use XML as format to allow complex body field values.
 * - nodequeue XML element to assign the node to a nodequeue after import.
 * - node_export XML element to load additional exported node data. Useful
 *   with webforms.
 */
abstract class BaselineContentNodeMigration extends XMLMigration {


  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   Path to the file containing the source data relative to the drupal
   *   directory.
   * @param string $type
   *   The node type to import to.
   */
  public function __construct($import_file, $type) {
    parent::__construct();
    $this->description = t('Import nodes.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'title' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/nodes/node', 'title', $fields);
    $this->destination = new MigrateDestinationNode($type);

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
    $this->addFieldMapping('sticky', 'sticky')->defaultValue(0)->xpath('sticky');
    $this->addFieldMapping('promote', 'promote')->defaultValue(0)->xpath('promote');
    $this->addFieldMapping('status', 'status')->defaultValue(1);
    $this->addFieldMapping('title', 'title')->xpath('title');
    $this->addFieldMapping('node_export', 'node_export')->xpath('node_export');
    $this->addFieldMapping('body', 'body')->xpath('body');
    $this->addFieldMapping('nodequeue', 'nodequeue')->xpath('nodequeue');
    $this->addFieldMapping('body:format')->defaultValue('full_html');
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
      'body'          => t('Body'),
      'title'         => t('Title'),
      'nodequeue'     => t('Nodequeue'),
      'node_export'   => t('Node Export'),
    );
  }

  /**
   * Implements MigrateDestinationEntity::prepare().
   *
   * Adds node_export code.
   */
  public function prepare($entity, stdClass $source_row) {
    if (isset($entity->node_export)) {
      baseline_content_node_export_attach($entity->node_export, $entity);
      unset($entity->node_export);
    }
  }

  /**
   * Implements MigrateDestinationEntity::complete().
   *
   * Assigns the node to a nodequeue.
   */
  public function complete($entity, stdClass $source_row) {
    if (!empty($source_row->nodequeue) && ($qids = nodequeue_load_queue_by_name($source_row->nodequeue))) {
      $context = array('qids' => $qids);
      nodequeue_add_action($entity, $context);
    }
  }
}
