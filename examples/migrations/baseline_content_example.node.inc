<?php

/**
 * @file
 * Example node migration.
 */

class BaselineContentExampleNodeMigration extends BaselineContentNodeMigration {

  public function __construct() {
    // Set up the path to the source file.
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.node.xml';

    // Call the BaselineContentNodeMigration constructor which will set up the
    // basic node migration.
    parent::__construct($import_path, 'page');

    // Group migrations. This enables you to distinguish between test and
    // production content.
    $this->group = MigrateGroup::getInstance('Test content');

    $this->description = t('Import pages.');

    // Declare all migrations that contain data you're referring to as
    // dependencies. Examples: files, terms, entity references, users, etc.
    $this->dependencies = array(
      'ExcentisImportDivisions',
      'ExcentisImportFiles',
      'ExcentisImportReferences',
      'ExcentisImportResourceNodes',
    );

    // Example of mapping a body field and settings its format.
    $this->addFieldMapping('body', 'body')->xpath('body');
    $this->addFieldMapping('body:format')->defaultValue('full_html');

    // Example of mapping a taxonomy term.
    $this->addFieldMapping('field_taxonomy_division', 'division')
      ->xpath('division')
      ->sourceMigration('ExcentisImportDivisions');
    $this->addFieldMapping('field_taxonomy_division:source_type')
       ->defaultValue('tid');

    // Exampple of using a helper function to map a taxonomy term.
    $this->addXMLTermFieldMapping('field_taxonomy_reference', 'references', 'ExcentisImportReferences')

    // Example of mapping an entity reference.
    $this->addFieldMapping('field_reference_resources', 'resources')
      ->xpath('resources')
      ->sourceMigration('ExcentisImportResourceNodes');
  }

  /**
   * Map fields that aren't directly supported by migrate.
   */
  function prepare($entity, stdClass $source_row) {
    parent::prepare($entity, $source_row);

    // Add link fields with title attributes.
    if (!empty($source_row->xml->website)) {
      foreach ($source_row->xml->website as $link) {
        if (isset($link->url)) {
          $field = array('url' => $link->url);
          if (isset($link->title)) {
            $field['title'] = $link->title;
          }
          $entity->field_contact_website[LANGUAGE_NONE][] = $field;
        }
      }
    }

    // Add addressfields.
    if (!empty($source_row->xml->address)) {
      foreach ($source_row->xml->address as $address) {
        $field = array(
          'country' => 'BE',
        );
        if (isset($address->name)) {
          $field['name_line'] = $address->name;
        }
        if (isset($address->address_line_1)) {
          $field['thoroughfare'] = $address->address_line_1;
        }
        if (isset($address->address_line_2)) {
          $field['premise'] = $address->address_line_2;
        }
        if (isset($address->postal)) {
          $field['postal_code'] = $address->postal;
        }
        if (isset($address->city)) {
          $field['locality'] = $address->city;
        }
        $entity->field_contact_address[LANGUAGE_NONE][] = $field;
      }
    }
  }
}
