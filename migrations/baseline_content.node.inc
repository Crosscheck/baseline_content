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
abstract class BaselineContentNodeMigration extends BaselineContentXMLMigration {


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
    parent::__construct($import_file, '/nodes/node', 'title');
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

    $this->destination = new MigrateDestinationNode($type);

    $this->addFieldMapping('uid', 'uid')->defaultValue(1);
    $this->addFieldMapping('sticky', 'sticky')->defaultValue(0)->xpath('sticky');
    $this->addFieldMapping('promote', 'promote')->defaultValue(0)->xpath('promote');
    $this->addFieldMapping('status', 'status')->defaultValue(1);
    $this->addFieldMapping('title', 'title')->xpath('title');
    $this->addFieldMapping('node_export', 'node_export')->xpath('node_export');
    $this->addFieldMapping('nodequeue', 'nodequeue')->xpath('nodequeue');
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
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
