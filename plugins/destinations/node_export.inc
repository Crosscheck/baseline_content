<?php

/**
 * @file
 * Support for node destinations.
 */

// TODO:
// Make sure this works with updates, explicit destination keys

/**
 * Destination class implementing migration into nodes.
 */
class MigrateDestinationNodeExport extends MigrateDestinationEntity {
  static public function getKeySchema() {
    return array(
      'nid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'description' => 'ID of destination node',
      ),
    );
  }

  /**
   * Return an options array for node destinations.
   *
   * @param string $language
   *  Default language for nodes created via this destination class.
   * @param string $text_format
   *  Default text format for nodes created via this destination class.
   */
  static public function options($language, $text_format) {
    return compact('language', 'text_format');
  }

  /**
   * Basic initialization
   *
   * @param string $bundle
   *  A.k.a. the content type (page, article, etc.) of the node.
   * @param array $options
   *  Options applied to nodes.
   */
  public function __construct(array $options = array()) {
    parent::__construct('node', NULL, $options);
  }

  /**
   * Returns a list of fields available to be mapped for the node type (bundle)
   *
   * @param Migration $migration
   *  Optionally, the migration containing this destination.
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields($migration = NULL) {
    return array();
  }

  /**
   * Delete a batch of nodes at once.
   *
   * @param $nids
   *  Array of node IDs to be deleted.
   */
  public function bulkRollback(array $nids) {
    migrate_instrument_start('node_delete_multiple');
    $this->prepareRollback($nids);
    node_delete_multiple($nids);
    $this->completeRollback($nids);
    migrate_instrument_stop('node_delete_multiple');
  }

  public function import(stdClass $node, stdClass $row) {
    $node = node_load(_baseline_content_get_nid_by_uuid($row->uuid));
    if (isset($node->nid) && !(isset($node->is_new) && $node->is_new)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }
    migrate_instrument_start('node_save');
    baseline_content_import_nodes($row->uuid, TRUE);
    migrate_instrument_stop('node_save');
    $node = node_load(_baseline_content_get_nid_by_uuid($row->uuid));
    $row->timestamp = $node->changed;
    if (isset($node->nid)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }
      $return = array($node->nid);
      $this->prepare($node, $row);
      // TODO: compare $node before and after prepare(). If the object has remained
      // unchanged, do not trigger another node_save().
      node_save($node);
      $this->complete($node, $row);
    }
    else {
      $return = FALSE;
    }
    return $return;
  }
}
