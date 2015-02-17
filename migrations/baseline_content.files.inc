<?php

/**
 * @file
 * Imports files to be used by nodes.
 *
 * See example data in examples/import/baseline_content_example.files.csv.
 */

abstract class BaselineContentFilesMigration extends Migration {

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
   * @param object $destination
   *   An optional Migrate destination object. Defaults to
   *   MigrateDestinationFile.
   */
  public function __construct(array $arguments, $import_path, $files_path = '', $destination = NULL) {
    parent::__construct($arguments);

    $this->import_path = $import_path;
    $this->files_path = $files_path;
    $this->description = t('Import files.');

    $schema = MigrateDestinationFile::getKeySchema();
    if (!empty($destination)) {
      $destination_class = get_class($destination);
      $schema = call_user_func(array($destination_class, 'getKeySchema'));
    }

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'filename' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      $schema
    );

    $this->source = new MigrateSourceCSV($this->import_path, $this->csvcolumns(), array('header_rows' => 1));

    $this->destination = $destination;
    if (empty($destination)) {
      $this->destination = new MigrateDestinationFile();
    }

    $this->addFieldMapping('value', 'filepath');
  }

  /**
   * Implements prepareRow().
   */
  public function prepareRow($row) {
    $row->filepath = $row->filename;
    // Prepend the files path if we're importing from the file system.
    if (strpos($row->filename, '://') === FALSE) {
      $row->filepath = $this->files_path . '/' . $row->filepath;
    }
  }

  function csvcolumns() {
    $columns[] = array('filename', 'File');
    return $columns;
  }
}
