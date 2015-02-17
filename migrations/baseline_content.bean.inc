<?php

/**
 * @file
 *  Base Migrations for beans.
 */

abstract class BaselineContentBeanMigration extends Migration {

  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   The path of the file containing the bean data.
   * @param string $type
   *   The bean type to import to.
   */
  public function __construct(array $arguments, $import_file, $type) {
    parent::__construct($arguments);
    $this->description = t('Import bean blocks.');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'label' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('bean')
    );

    $this->source = new MigrateSourceCSV($import_file, $this->csvcolumns(), array('header_rows' => 1));
    $this->destination = new MigrateDestinationEntityAPI('bean', $type);
    $this->addFieldMapping('label', 'label');
    $this->addFieldMapping('title', 'title');
    $this->addUnmigratedSources(array('region', 'pages', 'visibility', 'theme'), t('Do Not Migrate'));
  }

  function csvcolumns() {
    $columns[] = array('label', 'Label');
    $columns[] = array('title', 'Title');
    $columns[] = array('region', 'Region');
    $columns[] = array('pages', 'Pages');
    $columns[] = array('visibility', 'Visibility');
    $columns[] = array('theme', 'Theme');
    return $columns;
  }

  /**
   * Implements MigrateDestinationEntity::complete().
   *
   * Assigns the block to a region region.
   */
  public function complete($entity, stdClass $source_row) {
    $block = array(
      'module' => 'bean',
      'delta' => $entity->delta,
      'theme' => $source_row->theme,
      'status' => 1,
      'weight' => 0,
      'region' => isset($source_row->region) ? $source_row->region : BLOCK_REGION_NONE,
      'title' => $source_row->title,
      'pages' => isset($source_row->pages) ? $source_row->pages : '',
      'visibility' => isset($source_row->visibility) ? $source_row->visibility : 0,
      'cache' => 1,
    );

    db_merge('block')
    ->key(array('module' => $block['module'], 'delta' => $block['delta'], 'theme' => $block['theme']))
    ->fields($block)
    ->execute();
  }
}
