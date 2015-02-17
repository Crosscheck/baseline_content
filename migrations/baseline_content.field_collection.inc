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
   * Entity type of the field collection host.
   * @var string
   */
  protected $host_type;

  /**
   * Name of the migration containing the host entities.
   * @var string
   */
  protected $host_migration;

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
   * @param string $host_type
   *   The host entity type (defaults to 'node').
   */
  public function __construct(array $arguments, $import_file, $type, $source, $host_type = 'node') {
    parent::__construct($arguments);
    if (is_array($source)) {
      $this->dependencies = array_merge($this->dependencies, $source);
    }
    else {
      $this->dependencies[] = $source;
    }
    $this->host_type = $host_type;
    $this->host_migration = $source;

    $file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields();
    $this->source = new MigrateSourceXML($file, '/field_collections/field_collection', 'name', $fields);

    $this->destination = new MigrateDestinationFieldCollection(
      $type,
      array('host_entity_type' => $this->host_type)
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
    $this->addFieldMapping('host_entity_id', 'host_entity')->xpath('host_entity')->sourceMigration($this->host_migration);
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

  /**
   * Overrides Migration::handleSourceMigration().
   *
   * Make sure the host_entity_id is not an array.
   */
  protected function handleSourceMigration($source_migrations, $source_keys, $default = NULL, $migration = NULL) {
    $return_keys = parent::handleSourceMigration($source_migrations, $source_keys, $default, $migration);
    if ($this->host_type == 'bean' && $this->host_migration == $source_migrations) {
      $return_keys = isset($return_keys['destid1']) ? $return_keys['destid1'] : $return_keys;
    }
    return $return_keys;
  }
}
