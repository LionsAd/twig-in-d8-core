<?php

/**
 * @file
 * Contains \Drupal\block\Tests\BlockTitleXSSTest.
 */

namespace Drupal\block\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Tests block XSS in title.
 */
class BlockTitleXSSTest extends WebTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('block', 'block_test');

  public static function getInfo() {
    return array(
      'name' => 'Block XSS Title',
      'description' => 'Test block XSS in title.',
      'group' => 'Block',
    );
  }

  protected function setUp() {
    parent::setUp();

    $this->drupalPlaceBlock('test_xss_title', array(
      'label' => '<script>alert("XSS label");</script>',
      'machine_name' => 'test_xss_block',
    ));
  }

  /**
   * Test XSS in title.
   */
  function testXSSInTitle() {
    state()->set('block_test.content', $this->randomName());
    $this->drupalGet('');
    $this->assertNoRaw('<script>alert("XSS label");</script>', 'The block title was properly sanitized when rendered.');

    $this->drupalLogin($this->drupalCreateUser(array('administer blocks', 'access administration pages')));
    $default_theme = config('system.theme')->get('default');
    $this->drupalGet('admin/structure/block/list/block_plugin_ui:' . $default_theme . '/add');
    $this->assertNoRaw("<script>alert('XSS subject');</script>", 'The block title was properly sanitized in Block Plugin UI Admin page.');
  }

}
