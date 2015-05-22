<?php

/**
 * @file
 *  Base Migrations for Panelizer Entities.
 */

abstract class BaselineContentPanelizerEntityMigration extends XMLMigration {

  public function __construct($import_file) {
    parent::__construct();
    $this->description = t('Import panelizer entity settings.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'description' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationPanelizerEntity::getKeySchema()
    );

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/panelizer/panelize', 'description', $fields);
    $this->destination = new MigrateDestinationPanelizerEntity();

    $this->addFieldMapping('panelizer_entity_export', 'panelizer_entity_export')->xpath('panelizer_entity_export');
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
      'description'          => t('Description'),
      'entity_title'          => t('Entity title'),
      'panelizer_entity_export'          => t('Panelizer Entity Export'),
    );
  }
}
