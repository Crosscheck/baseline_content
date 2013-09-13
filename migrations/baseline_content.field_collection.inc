<?php

/**
 * @file
 *  Base Migrations for field collections.
 */

/**
 * Base Migration field collections.
 *
 * A field collection can be imported with an XML file with the following
 * structure:
 *
 * <field_collections>
 *  <field_collection>
 *    <name>Required: Unique name, used in the map.</name>
 *    <host_entity>Required: Unique ID (node title) of the host entity.
 *    </host_entity>
 *  </field_collection>
 * </field_collections>
 */
abstract class BaselineContentFieldCollectionMigration extends XMLMigration {

  /**
   * Migration constructor.
   *
   * @param string $file
   *   Path to the source XML to import, relative to the drupal directory.
   * @param string $type
   *   The field collection type.
   * @param string $source
   *   The name of the source class containing the nodes this field collection
   *   will be attached to.
   */
  public function __construct($import_file, $type, $source) {
    parent::__construct();
    $this->dependencies = array($source);

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/field_collections/field_collection', 'name', $fields);

    $this->destination = new MigrateDestinationFieldCollection(
      $type,
      array('host_entity_type' => 'node')
    );

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'name' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationFieldCollection::getKeySchema()
    );

    // Attach the field collection data
    $this->addFieldMapping('host_entity_id', 'host_entity')->xpath('host_entity')->sourceMigration($source);
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields() {
    return array(
      'name'          => t('Machine name'),
      'host_entity'   => t('Host entity machine name'),
    );
  }
}
