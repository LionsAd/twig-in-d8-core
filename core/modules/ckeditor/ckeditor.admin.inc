<?php

/**
 * @file
 * Callbacks and theming for the CKEditor toolbar configuration UI.
 */

use Drupal\Core\Template\Attribute;

/**
 * Prepares variables for CKEditor settings toolbar templates.
 *
 * Default template: ckeditor-settings-toolbar.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - editor: An editor object.
 *   - plugins: A list of plugins.
 */
function template_preprocess_ckeditor_settings_toolbar(&$variables) {
  // Simplify the language direction information for toolbar buttons.
  $language_interface = language(LANGUAGE_TYPE_INTERFACE);
  $variables['language_direction'] = $language_interface->direction ? 'rtl' : 'ltr';

  // Create lists of active and disabled buttons.
  $editor = $variables['editor'];
  $plugins = $variables['plugins'];

  $buttons = array();
  $multiple_buttons = array();
  $active_buttons = array();

  foreach ($plugins as $plugin_buttons) {
    foreach ($plugin_buttons as $button_name => $button) {
      $button['name'] = $button_name;
      if (!empty($button['multiple'])) {
        $multiple_buttons[$button_name] = $button;
      }
      $buttons[$button_name] = $button;
    }
  }

  foreach ($editor->settings['toolbar']['buttons'] as $row_number => $row) {
    foreach ($row as $button_name) {
      if (isset($buttons[$button_name])) {
        $active_buttons[$row_number][] = $buttons[$button_name];
        if (empty($buttons[$button_name]['multiple'])) {
          unset($buttons[$button_name]);
        }
      }
    }
  }

  $disabled_buttons = array_diff_key($buttons, $multiple_buttons);

  $rtl = $variables['language_direction'] === 'rtl' ? '_rtl' : '';

  $build_button_item = function($button, $rtl) {
    // Value of the button item.
    if (isset($button['image_alternative'])) {
      $value = $button['image_alternative' . $rtl];
    }
    elseif (isset($button['image'])) {
      $value = array(
        '#theme' => 'image',
        '#uri' => $button['image' . $rtl],
        '#title' => $button['label'],
      );
    }
    else {
      $value = '?';
    }

    // Set additional attribute on the button if it can occur multiple times.
    if (!empty($button['multiple'])) {
      $button['attributes']['class'][] = 'ckeditor-multiple-button';
    }

    // Build the button item.
    $attributes = array(
      'data-button-name' => $button['name'],
    );
    if (!empty($button['attributes'])) {
      $attributes = array_merge($attributes, $button['attributes']);
    }

    $button_item = array(
      'value' => $value,
      'attributes' => new Attribute($attributes),
    );
    return $button_item;
  };

  // Assemble items to be added to active button rows.
  $variables['active_buttons'] = array();
  foreach ($active_buttons as $row_number => $row_buttons) {
    foreach ($row_buttons as $button) {
      $variables['active_buttons'][$row_number][] = $build_button_item($button, $rtl);
    }
  }
  // Assemble list of disabled buttons (which are always a single row).
  $variables['disabled_buttons'] = array();
  foreach ($disabled_buttons as $button) {
    $variables['disabled_buttons'][] = $build_button_item($button, $rtl);
  }
  // Assemble list of multiple buttons that may be added multiple times.
  $variables['multiple_buttons'] = array();
  foreach ($multiple_buttons as $button) {
    $variables['multiple_buttons'][] = $build_button_item($button, $rtl);
  }
}
