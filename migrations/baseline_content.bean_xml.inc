<?php

/**
 * @file
 * Imports content blocks.
 */

/**
 * Base Migration for beans from an XML file.
 */
abstract class BaselineContentBeanXMLMigration extends BaselineContentXMLMigration {

  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   Path to the file containing the source data relative to the drupal
   *   directory.
   * @param string $type
   *   The bean type to import to.
   */
  public function __construct($import_file, $type) {
    parent::__construct($import_file, '/blocks/block', 'label');

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

    $this->destination = new MigrateDestinationEntityAPI('bean', $type);

    foreach ($this->getSourceFields() as $element) {
      $this->addXMLFieldMapping($element);
    }
  }

  /**
   * Overrides BaselineContentXMLMigration::getSourceFields().
   */
  protected function getSourceFields() {
    return array('label', 'title', 'region', 'pages', 'visibility', 'body', 'delta');
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
      'theme' => variable_get('theme_default'),
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
