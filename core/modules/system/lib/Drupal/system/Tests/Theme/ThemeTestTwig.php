<?php

/**
 * @file
 * Contains \Drupal\system\Tests\Theme\ThemeTestTwig.
 */

namespace Drupal\system\Tests\Theme;

use Drupal\simpletest\WebTestBase;

/**
 * Tests theme functions with the Twig engine.
 */
class ThemeTestTwig extends WebTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('theme_test', 'twig_theme_test');

  public static function getInfo() {
    return array(
      'name' => 'Twig Engine',
      'description' => 'Test theme functions with twig.',
      'group' => 'Theme',
    );
  }

  function setUp() {
    parent::setUp();
    theme_enable(array('test_theme_twig'));
  }

  /**
   * Ensures a themes template is overrideable based on the 'template' filename.
   */
  function testTemplateOverride() {
    config('system.theme')
      ->set('default', 'test_theme_twig')
      ->save();
    $this->drupalGet('theme-test/template-test');
    $this->assertText('Success: Template overridden.', t('Template overridden by defined \'template\' filename.'));
  }

  /**
   * Tests drupal_find_theme_templates
   */
  function testFindThemeTemplates() {

    $cache = array();

    // Prime the theme cache
    foreach (module_implements('theme') as $module) {
      _theme_process_registry($cache, $module, 'module', $module, drupal_get_path('module', $module));
    }

    // Check for correct content
    // @todo Remove this tests once double engine code is removed

    $this->assertEqual($cache['node']['template_file'], 'core/modules/node/templates/node.html.twig', 'Node is using node.html.twig as template file');
    $this->assertEqual($cache['node']['engine'], 'twig', 'Node is using twig engine');

    $this->assertEqual($cache['theme_test_template_test']['template_file'], 'core/modules/system/tests/modules/theme_test/templates/theme_test.template_test.html.twig', 'theme_test is using theme_test.template_test.html.twig as template file');
    $this->assertEqual($cache['theme_test_template_test']['engine'], 'twig', 'theme_test is using twig as engine.');

    $templates = drupal_find_theme_templates($cache, '.html.twig', drupal_get_path('theme', 'test_theme_twig'));
    $this->assertEqual($templates['node__1']['template'], 'node--1', 'Template node--1.html.twig was found in test_theme_twig.');
  }

  /**
   * Tests that the Twig engine handles PHP data correctly.
   */
  function testTwigVariableDataTypes() {
    config('system.theme')
      ->set('default', 'test_theme_twig')
      ->save();
    $this->drupalGet('twig-theme-test/php-variables');
    foreach (_test_theme_twig_php_values() as $type => $value) {
      $this->assertRaw('<li>' . $type . ': ' . $value['expected'] . '</li>');
    }
  }

}
