<?php

/**
 * @file
 *  Base Migrations for XML based source content.
 */

/**
 * Base Migration for content in XML files.
 */
abstract class BaselineContentXMLMigration extends XMLMigration {

  /**
   * Path to the XML file containing the data.
   * @var string
   */
  protected $import_file;


  /**
   * Migration constructor.
   *
   * @param string $import_file
   *   Path to the file containing the source data relative to the drupal
   *   directory.
   * @param string $item_path
   *   The Xpath to an individual item in the migration. Defaults to
   *   /items/item.
   * @param string $key
   *   The element name of the unique key of each item. Defaults to key.
   */
  public function __construct($import_file, $item_path = '/items/item', $key = 'key') {
    parent::__construct();

    $this->import_file = DRUPAL_ROOT . '/' . $import_file;
    $fields = $this->getSourceFields($key);
    $this->source = new MigrateSourceXML($file, $item_path, $key, $fields);
  }

  /**
   * Returns an array of resource field definitions.
   */
  protected function getSourceFields($key) {
    return array(
      $key => t('Key'),
    );
  }

  /**
   * Map an XML element to a drupal field.
   *
   * @param string $field_name
   *   The drupal field name.
   * @param string $element
   *   The XML element name.
   */
  protected function addXMLFieldMapping($field_name, $element = NULL, $xpath = NULL) {
    if (is_null($element)) {
      $element = $field_name;
    }
    if (is_null($xpath)) {
      $xpath = $element;
    }
    return $this->addFieldMapping($field_name, $element)->xpath($element);
  }

  /**
   * Map an XML element to a drupal term reference field.
   *
   * @param string $field_name
   *   The drupal field name.
   * @param string $element
   *   The XML element name.
   * @param string $source
   *   The name of the source migration of the term.
   */
  protected function addXMLTermFieldMapping($field_name, $element, $source) {
    $mapping = $this->addFieldMapping($field_name, $element)
      ->xpath($element)
      ->separator(',')
      ->sourceMigration($source);
    $this->addFieldMapping($field_name . ':source_type')->defaultValue('tid');

    return $mapping;
  }
}
