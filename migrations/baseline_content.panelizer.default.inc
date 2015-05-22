<?php

/**
 * @file
 *  Base Migrations for Panelizer defaults.
 */

abstract class BaselineContentPanelizerDefaultMigration extends XMLMigration {

  public function __construct($import_file) {
    parent::__construct();
    $this->description = t('Import panelizer default settings.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'description' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationPanelizerDefault::getKeySchema()
    );

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/panelizer/panelize', 'description', $fields);
    $this->destination = new MigrateDestinationPanelizerDefault();

    $this->addFieldMapping('panelizer_defaults_export', 'panelizer_defaults_export')->xpath('panelizer_defaults_export');
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
      'description'          => t('Description'),
      'panelizer_defaults_export'          => t('Panelizer Defaults Export'),
    );
  }
}
