<?php

/**
 * @file
 * Definition of Drupal\block\Tests\BlockTemplateSuggestionsUnitTest.
 */

namespace Drupal\block\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Unit tests for template_preprocess_block().
 */
class BlockTemplateSuggestionsUnitTest extends WebTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('block');

  public static function getInfo() {
    return array(
      'name' => 'Block template suggestions',
      'description' => 'Test the template_preprocess_block() function.',
      'group' => 'Block',
    );
  }

  /**
   * Test if template_preprocess_block() handles the suggestions right.
   */
  function testBlockThemeHookSuggestions() {
    // Define a block with a derivative to be preprocessed, which includes both
    // an underscore (not transformed) and a hyphen (transformed to underscore),
    // and generates possibilities for each level of derivative.
    // @todo Clarify this comment.
    $block = entity_create('block', array(
      'plugin' => 'system_menu_block:menu-admin',
      'region' => 'footer',
      'id' => config('system.theme')->get('default') . '.machinename',
    ));

    $variables = array();
    $variables['elements']['#block'] = $block;
    $variables['elements']['#block_config'] = $block->getPlugin()->getConfig() + array(
      'id' => $block->get('plugin'),
      'region' => $block->get('region'),
      'module' => $block->get('module'),
    );
    $variables['elements']['#children'] = '';
    template_preprocess_block($variables);
    $this->assertEqual($variables['theme_hook_suggestions'], array('block__system', 'block__system_menu_block', 'block__system_menu_block__menu_admin', 'block__machinename'));
  }

}
