<?php

/**
 * @file
 * Imports files to be used by nodes.
 *
 * See example data in examples/import/baseline_content_example.files.csv.
 */

abstract class BaselineContentNodeExportMigration extends Migration {
  protected $import_path;
  protected $files_path;

  protected $force_update = TRUE;

  public function __construct($import_path, $files_path = '') {
    parent::__construct();

    $this->import_path = $import_path;
    $this->files_path = $files_path;
    $this->description = t('Import nodes from node_export files, based on a single master file.');

    $schema = MigrateDestinationNode::getKeySchema();
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'uuid' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      $schema
    );

    $this->source = new MigrateSourceCSV($this->import_path, $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationNodeExport();
  }

  public function prepareRow($row) {
    // Skip rows if they're not more recent than what's already in the database.
    if (!$this->force_update) {
      if (!_baseline_content_importfile_is_newer($row->uuid)) {
        return FALSE;
      }
    }
    // Add the XML's Migrate settings to the row.
    $row->migrate = baseline_content_get_migrate_settings($row->uuid);
  }

  public function complete($entity, $row) {
    // Since newly imported files get a "last updated" timestamp set to "now", we need
    // to update the database with the "last updated" timestamp from the source XML.
    // This is needed so bcen doesn't produce false positives.
    db_update('node')
      ->fields(array('changed' => $row->timestamp, 'created' => $row->timestamp))
      ->condition('uuid', $row->uuid)
      ->execute();
  }

  function csvcolumns() {
    $columns[] = array('uuid','last updated','title','node type','delta');
    return $columns;
  }
}
