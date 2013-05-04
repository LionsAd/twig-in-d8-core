<?php

/**
 * @file
 * User page callbacks for the Filter module.
 */

/**
 * Page callback: Displays a page with long filter tips.
 *
 * @param $format
 *   (optional) A filter format. Defaults to NULL.
 *
 * @return string
 *   An HTML-formatted string.
 *
 * @see filter_menu()
 * @see theme_filter_tips()
 */
function filter_tips_long($format = NULL) {
  if (!empty($format)) {
    $filter_tips = array(
      '#theme' => 'filter_tips',
      '#tips' => _filter_tips($format->format, TRUE),
      '#long' => TRUE,
    );
  }
  else {
    $filter_tips = array(
      '#theme' => 'filter_tips',
      '#tips' => _filter_tips(-1, TRUE),
      '#long' => TRUE,
    );
  }
  return $filter_tips;
}
