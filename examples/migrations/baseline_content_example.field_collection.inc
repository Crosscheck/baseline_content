<?php

/**
 * @file
 * Baseline content example: Imports field collections.
 */

/**
 * Imports training feeds.
 */
class BaselineContentExampleFieldCollectionMigration extends BaselineContentFieldCollectionMigration {

  public function __construct() {
    $module_path = drupal_get_path('module', 'baseline_content');
    $import_path = $module_path . '/examples/import/baseline_content_example.field_collection.xml';
    parent::__construct($import_path, 'field_training_tarif', 'BaselineContentExampleNodeMigration');

    $this->addFieldMapping('field_tarif_participants', 'participants')->xpath('participants');
    $this->addFieldMapping('field_tarif_price', 'price')->xpath('price');
  }

  protected function getSourceFields() {
    $fields = parent::getSourceFields();
    $fields['participants'] = t('Participants');
    $fields['price'] = t('Price');
    return $fields;
  }
}
