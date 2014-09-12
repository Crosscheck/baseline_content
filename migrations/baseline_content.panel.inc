<?php

/**
 * @file
 *  Base Migrations for panels.
 */

/**
 * Base Migration for panels containing complex data.
 *
 * Features:
 *
 * - Use XML as format to allow complex data.
 */
abstract class BaselineContentPanelMigration extends XMLMigration {


  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   Path to the file containing the source data relative to the drupal
   *   directory.
   */
  public function __construct($import_file) {
    parent::__construct();
    $this->description = t('Import panels.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'title' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationPanel::getKeySchema()
    );

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/panels/panel', 'id', $fields);
    $this->destination = new MigrateDestinationPanel();
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
      'id'          => t('ID'),
      'arguments'          => t('Arguments'),
    );
  }

  abstract function getPanel($id);
}
