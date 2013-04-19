<?php

/**
 * @file
 * Contains \Drupal\system\Tests\Theme\PreprocessHtmlTagUnitTest.
 */

namespace Drupal\system\Tests\Theme;

use Drupal\simpletest\UnitTestBase;

/**
 * Unit tests for template_preprocess_html_tag().
 */
class PreprocessHtmlTagUnitTest extends UnitTestBase {
  public static function getInfo() {
    return array(
      'name' => 'Preprocess HTML Tag',
      'description' => 'Tests template_preprocess_html_tag() function.',
      'group' => 'Theme',
    );
  }

  /**
   * Tests template_preprocess_html_tag().
   */
  function testPreprocessHtmlTag() {
    // Test running sample meta tag through the preprocess function.
    $variables['element'] = array('#tag' => 'meta', '#attributes' => array('name' => 'description', 'content' => 'Drupal test'));
    template_preprocess_html_tag($variables);
    $this->assertEqual($variables['tag'], 'meta', "'tag' variable is set to 'meta'.");
    $this->assertTrue(!isset($variables['value']), "'value' variable is not set when no value is passed in.");

    // Test running sample title tag through the preprocess function.
    $variables = array();
    $variables['element'] = array('#tag' => 'title', '#value' => 'title test');
    template_preprocess_html_tag($variables);
    $this->assertEqual($variables['tag'], 'title', "'tag' variable is set to 'title'.");
    $this->assertEqual($variables['value'], 'title test', "'value' variable is equal to the value of '#value' in the element array.");
  }
}
