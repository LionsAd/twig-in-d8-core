<?php

/**
 * Prepares variables for theme test templates.
 *
 * Default template: theme-test.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - foo: Dummy text to display.
 */
function template_preprocess_theme_test(&$variables) {
  $variables['foo'] = 'template_preprocess_theme_test';
}
