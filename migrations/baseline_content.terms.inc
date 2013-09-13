<?php

/**
 * @file
 *  Base Migrations for Taxonomy Terms.
 */

abstract class BaselineContentTermMigration extends Migration {

  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   Path to the file containing the import data, relative to the drupal
   *   directory.
   * @param string $vocabulary_name
   *   The vocabulary to import to.
   */
  public function __construct($import_file, $vocabulary_name) {
    parent::__construct();
    $this->description = t('Import terms.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'name' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    $this->source = new MigrateSourceCSV($import_file, $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationTerm($vocabulary_name);
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('description', 'description');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('parent', 'parent')->sourceMigration(array(get_class($this)));
  }

  function csvcolumns() {
    $columns[] = array('name', 'Name');
    $columns[] = array('description', 'Description');
    $columns[] = array('parent', 'Parent');
    return $columns;
  }

  /**
   * Implements prepareRow().
   *
   * Set the term weight.
   */
  public function prepareRow($row) {
    static $weight;
    if (empty($weight)) {
      $weight = 0;
    }
    $row->weight = ++$weight;
  }
}
