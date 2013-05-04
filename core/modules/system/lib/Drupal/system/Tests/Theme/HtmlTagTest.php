<?php

/**
 * @file
 * Contains \Drupal\system\Tests\Theme\HtmlTagTest.
 */

namespace Drupal\system\Tests\Theme;

use Drupal\simpletest\WebTestBase;

/**
 * Tests HTML tag output.
 */
class HtmlTagTest extends WebTestBase {
  public static function getInfo() {
    return array(
      'name' => 'HTML tag output',
      'description' => "Tests output of '#theme' => 'html_tag'.",
      'group' => 'Theme',
    );
  }

  /**
   * Tests the output of '#theme' => 'html_tag'.
   */
  function testThemeHtmlTag() {
    // Test auto-closure meta tag generation.
    $tag = array('#theme' => 'html_tag', '#tag' => 'meta', '#attributes' => array('name' => 'description', 'content' => 'Drupal test'));
    $this->assertEqual('<meta name="description" content="Drupal test" />' . "\n", drupal_render($tag), 'Test auto-closure meta tag generation.');

    // Test title tag generation.
    $tag = array('#theme' => 'html_tag', '#tag' => 'title', '#value' => 'title test');
    $this->assertEqual('<title>title test</title>' . "\n", drupal_render($tag), 'Test title tag generation.');
  }
}
