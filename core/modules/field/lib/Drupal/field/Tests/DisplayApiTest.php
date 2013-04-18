<?php

/**
 * @file
 * Definition of Drupal\field\Tests\DisplayApiTest.
 */

namespace Drupal\field\Tests;

class DisplayApiTest extends FieldUnitTestBase {

  public static function getInfo() {
    return array(
      'name' => 'Field Display API tests',
      'description' => 'Test the display API.',
      'group' => 'Field API',
    );
  }

  function setUp() {
    parent::setUp();

    $this->installSchema('user', array('role_permission'));

    // Create a field and instance.
    $this->field_name = 'test_field';
    $this->label = $this->randomName();
    $this->cardinality = 4;

    $this->field = array(
      'field_name' => $this->field_name,
      'type' => 'test_field',
      'cardinality' => $this->cardinality,
    );
    $this->instance = array(
      'field_name' => $this->field_name,
      'entity_type' => 'test_entity',
      'bundle' => 'test_bundle',
      'label' => $this->label,
    );

    $this->display_options = array(
      'default' => array(
        'type' => 'field_test_default',
        'settings' => array(
          'test_formatter_setting' => $this->randomName(),
        ),
      ),
      'teaser' => array(
        'type' => 'field_test_default',
        'settings' => array(
          'test_formatter_setting' => $this->randomName(),
        ),
      ),
    );

    field_create_field($this->field);
    field_create_instance($this->instance);
    // Create a display for the default view mode.
    entity_get_display($this->instance['entity_type'], $this->instance['bundle'], 'default')
      ->setComponent($this->field_name, $this->display_options['default'])
      ->save();
    // Create a display for the teaser view mode.
    entity_get_display($this->instance['entity_type'], $this->instance['bundle'], 'teaser')
      ->setComponent($this->field_name, $this->display_options['teaser'])
      ->save();

    // Create an entity with values.
    $this->values = $this->_generateTestFieldValues($this->cardinality);
    $this->entity = field_test_create_entity();
    $this->is_new = TRUE;
    $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED] = $this->values;
    field_test_entity_save($this->entity);
  }

  /**
   * Test the field_view_field() function.
   */
  function testFieldViewField() {
    // No display settings: check that default display settings are used.
    $output = field_view_field($this->entity, $this->field_name);
    $this->content = drupal_render($output);
    $settings = field_info_formatter_settings('field_test_default');
    $setting = $settings['test_formatter_setting'];
    $this->assertText($this->label, 'Label was displayed.');
    foreach ($this->values as $delta => $value) {
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // Check that explicit display settings are used.
    $display = array(
      'label' => 'hidden',
      'type' => 'field_test_multiple',
      'settings' => array(
        'test_formatter_setting_multiple' => $this->randomName(),
        'alter' => TRUE,
      ),
    );
    $output = field_view_field($this->entity, $this->field_name, $display);
    $this->content = drupal_render($output);
    $setting = $display['settings']['test_formatter_setting_multiple'];
    $this->assertNoText($this->label, 'Label was not displayed.');
    $this->assertText('field_test_field_attach_view_alter', 'Alter fired, display passed.');
    $array = array();
    foreach ($this->values as $delta => $value) {
      $array[] = $delta . ':' . $value['value'];
    }
    $this->assertText($setting . '|' . implode('|', $array), 'Values were displayed with expected setting.');

    // Check the prepare_view steps are invoked.
    $display = array(
      'label' => 'hidden',
      'type' => 'field_test_with_prepare_view',
      'settings' => array(
        'test_formatter_setting_additional' => $this->randomName(),
      ),
    );
    $output = field_view_field($this->entity, $this->field_name, $display);
    $view = drupal_render($output);
    $this->content = $view;
    $setting = $display['settings']['test_formatter_setting_additional'];
    $this->assertNoText($this->label, 'Label was not displayed.');
    $this->assertNoText('field_test_field_attach_view_alter', 'Alter not fired.');
    foreach ($this->values as $delta => $value) {
      $this->assertText($setting . '|' . $value['value'] . '|' . ($value['value'] + 1), format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // View mode: check that display settings specified in the display object
    // are used.
    $output = field_view_field($this->entity, $this->field_name, 'teaser');
    $this->content = drupal_render($output);
    $setting = $this->display_options['teaser']['settings']['test_formatter_setting'];
    $this->assertText($this->label, 'Label was displayed.');
    foreach ($this->values as $delta => $value) {
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // Unknown view mode: check that display settings for 'default' view mode
    // are used.
    $output = field_view_field($this->entity, $this->field_name, 'unknown_view_mode');
    $this->content = drupal_render($output);
    $setting = $this->display_options['default']['settings']['test_formatter_setting'];
    $this->assertText($this->label, 'Label was displayed.');
    foreach ($this->values as $delta => $value) {
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }
  }

  /**
   * Test the field_view_value() function.
   */
  function testFieldViewValue() {
    // No display settings: check that default display settings are used.
    $settings = field_info_formatter_settings('field_test_default');
    $setting = $settings['test_formatter_setting'];
    foreach ($this->values as $delta => $value) {
      $item = $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED][$delta];
      $output = field_view_value($this->entity, $this->field_name, $item);
      $this->content = drupal_render($output);
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // Check that explicit display settings are used.
    $display = array(
      'type' => 'field_test_multiple',
      'settings' => array(
        'test_formatter_setting_multiple' => $this->randomName(),
      ),
    );
    $setting = $display['settings']['test_formatter_setting_multiple'];
    $array = array();
    foreach ($this->values as $delta => $value) {
      $item = $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED][$delta];
      $output = field_view_value($this->entity, $this->field_name, $item, $display);
      $this->content = drupal_render($output);
      $this->assertText($setting . '|0:' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // Check that prepare_view steps are invoked.
    $display = array(
      'type' => 'field_test_with_prepare_view',
      'settings' => array(
        'test_formatter_setting_additional' => $this->randomName(),
      ),
    );
    $setting = $display['settings']['test_formatter_setting_additional'];
    $array = array();
    foreach ($this->values as $delta => $value) {
      $item = $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED][$delta];
      $output = field_view_value($this->entity, $this->field_name, $item, $display);
      $this->content = drupal_render($output);
      $this->assertText($setting . '|' . $value['value'] . '|' . ($value['value'] + 1), format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // View mode: check that display settings specified in the instance are
    // used.
    $setting = $this->display_options['teaser']['settings']['test_formatter_setting'];
    foreach ($this->values as $delta => $value) {
      $item = $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED][$delta];
      $output = field_view_value($this->entity, $this->field_name, $item, 'teaser');
      $this->content = drupal_render($output);
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }

    // Unknown view mode: check that display settings for 'default' view mode
    // are used.
    $setting = $this->display_options['default']['settings']['test_formatter_setting'];
    foreach ($this->values as $delta => $value) {
      $item = $this->entity->{$this->field_name}[LANGUAGE_NOT_SPECIFIED][$delta];
      $output = field_view_value($this->entity, $this->field_name, $item, 'unknown_view_mode');
      $this->content = drupal_render($output);
      $this->assertText($setting . '|' . $value['value'], format_string('Value @delta was displayed with expected setting.', array('@delta' => $delta)));
    }
  }
}
