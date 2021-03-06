<?php

/**
 * @file
 * Contains \Drupal\node\Plugin\block\block\SyndicateBlock.
 */

namespace Drupal\node\Plugin\block\block;

use Drupal\block\BlockBase;
use Drupal\Component\Annotation\Plugin;
use Drupal\Core\Annotation\Translation;

/**
 * Provides a 'Syndicate' block that links to the site's RSS feed.
 *
 * @Plugin(
 *   id = "node_syndicate_block",
 *   admin_label = @Translation("Syndicate"),
 *   module = "node"
 * )
 */
class SyndicateBlock extends BlockBase {

  /**
   * Overrides \Drupal\block\BlockBase::settings().
   */
  public function settings() {
    return array(
      'block_count' => 10,
    );
  }

  /**
   * Overrides \Drupal\block\BlockBase::blockAccess().
   */
  public function blockAccess() {
    return user_access('access content');
  }

  /**
   * Implements \Drupal\block\BlockBase::blockBuild().
   */
  protected function blockBuild() {
    return array(
      '#theme' => 'feed_icon',
      '#url' => 'rss.xml',
    );
  }

}
