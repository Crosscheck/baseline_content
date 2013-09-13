<?php

/**
 * @file
 * Imports files to be used by nodes.
 *
 * See example data in examples/import/baseline_content_example.files.csv.
 */

class BaselineContentFilesMigration extends Migration {

  /**
   * Path to the CSV file containing the file names to import, relative to the
   *
   * @var string
   */
  protected $import_path;

  /**
   * Path to the directory containing the files that will be imported.
   *
   * @var string
   */
  protected $files_path;

  /**
   * BaselineContentFilesMigration constructor.
   *
   * @param string $import_path
   *   Path to the CSV file containing the file names to import, relative to the
   *   drupal directory. Note that
   * @param string $files_path
   *   Path to the directory containing the files that will be imported.
   */
  public function __construct($import_path, $files_path) {
    parent::__construct();

    $this->import_path = $import_path;
    $this->files_path = $files_path;
    $this->description = t('Import files.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'filename' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationFile::getKeySchema()
    );

    $this->source = new MigrateSourceCSV($this->import_path, $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationFile();
    $this->addFieldMapping('value', 'filepath');
  }

  /**
   * Implements prepareRow().
   */
  public function prepareRow($row) {
    // Set the file path for image import.
    $row->filepath = $this->files_path . '/' . $row->filename;
  }

  function csvcolumns() {
    $columns[] = array('filename', 'File');
    return $columns;
  }
}
