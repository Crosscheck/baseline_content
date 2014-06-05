Baseline Content
================

Drupal module for providing a content baseline using migrate.module. See the
examples directory for examples on using this in your own project.

## Suggested workflow

* Create a module containing migrations with all your test content. Use the same
  group for all test content migrations. This allows your to rollback all test
  content instantly.
* Create a module containing production content migrations.

## Requirements

* migrate module
* migrate_extras module
* node_export (for exporting webform configurations)
* The following patches:
  * http://drupal.org/files/migrate_extras_entity_api_entity_keys_name.patch
  * http://drupal.org/files/field_collection-migrate-1175082-222.patch

## Supported migrations

* Nodes from XML files
  * webforms (via node_export)
  * nodequeues
* Beans from XML or CSV
* Taxonomy terms from CSV
* Menu links from CSV
* Files and Media from CSV

## To Do

* Don't use titles as node keys, use a custom value (see menu items).
* Add examples for all abstract migrations.
