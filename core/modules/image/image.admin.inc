<?php

/**
 * @file
 * Administration pages for image settings.
 */

use Drupal\Core\Template\Attribute;

/**
 * Menu callback; Listing of all current image styles.
 */
function image_style_list() {
  $page = array();

  $styles = entity_load_multiple('image_style');
  $page['image_style_list'] = array(
    '#markup' => theme('image_style_list', array('styles' => $styles)),
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'image') . '/image.admin.css' => array()),
    ),
  );

  return $page;

}

/**
 * Form builder; Edit an image style name and effects order.
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $style
 *   An image style array.
 * @ingroup forms
 * @see image_style_form_submit()
 */
function image_style_form($form, &$form_state, $style) {
  $title = t('Edit style %name', array('%name' => $style->label()));
  drupal_set_title($title, PASS_THROUGH);

  $form_state['image_style'] = $style;
  $form['#tree'] = TRUE;
  $form['#attached']['css'][drupal_get_path('module', 'image') . '/image.admin.css'] = array();

  // Show the thumbnail preview.
  $form['preview'] = array(
    '#type' => 'item',
    '#title' => t('Preview'),
    '#markup' => theme('image_style_preview', array('style' => $style)),
  );

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrative label'),
    '#default_value' => $style->label(),
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $style->id(),
    '#machine_name' => array(
      'exists' => 'image_style_load',
    ),
    '#required' => TRUE,
  );

  // Build the list of existing image effects for this image style.
  $form['effects'] = array(
    '#theme' => 'image_style_effects',
  );
  if (!empty($style->effects)) {
    foreach ($style->effects as $key => $effect) {
      $form['effects'][$key]['#weight'] = isset($form_state['input']['effects']) ? $form_state['input']['effects'][$key]['weight'] : NULL;
      $form['effects'][$key]['label'] = array(
        '#markup' => check_plain($effect['label']),
      );
      $form['effects'][$key]['summary'] = array(
        '#markup' => isset($effect['summary theme']) ? theme($effect['summary theme'], array('data' => $effect['data'])) : '',
      );
      $form['effects'][$key]['weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight for @title', array('@title' => $effect['label'])),
        '#title_display' => 'invisible',
        '#default_value' => $effect['weight'],
      );

      $links = array();
      if (isset($effect['form callback'])) {
        $links['edit'] = array(
          'title' => t('edit'),
          'href' => 'admin/config/media/image-styles/manage/' . $style->id() . '/effects/' . $key,
        );
      }
      $links['delete'] = array(
        'title' => t('delete'),
        'href' => 'admin/config/media/image-styles/manage/' . $style->id() . '/effects/' . $key . '/delete',
      );
      $form['effects'][$key]['operations'] = array(
        '#type' => 'operations',
        '#links' => $links,
      );
      $form['effects'][$key]['configure'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'admin/config/media/image-styles/manage/' . $style->id() . '/effects/' . $key,
        '#access' => isset($effect['form callback']),
      );
      $form['effects'][$key]['remove'] = array(
        '#type' => 'link',
        '#title' => t('delete'),
        '#href' => 'admin/config/media/image-styles/manage/' . $style->id() . '/effects/' . $key . '/delete',
      );
    }
  }

  // Build the new image effect addition form and add it to the effect list.
  $new_effect_options = array();
  foreach (image_effect_definitions() as $effect => $definition) {
    $new_effect_options[$effect] = $definition['label'];
  }
  $form['effects']['new'] = array(
    '#tree' => FALSE,
    '#weight' => isset($form_state['input']['weight']) ? $form_state['input']['weight'] : NULL,
  );
  $form['effects']['new']['new'] = array(
    '#type' => 'select',
    '#title' => t('Effect'),
    '#title_display' => 'invisible',
    '#options' => $new_effect_options,
    '#empty_option' => t('Select a new effect'),
  );
  $form['effects']['new']['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight for new effect'),
    '#title_display' => 'invisible',
    '#default_value' => count($form['effects']) - 1,
  );
  $form['effects']['new']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#validate' => array('image_style_form_add_validate'),
    '#submit' => array('image_style_form_submit', 'image_style_form_add_submit'),
  );

  // Show the Override or Submit button for this style.
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update style'),
  );

  return $form;
}

/**
 * Validate handler for adding a new image effect to an image style.
 */
function image_style_form_add_validate($form, &$form_state) {
  if (!$form_state['values']['new']) {
    form_error($form['effects']['new']['new'], t('Select an effect to add.'));
  }
}

/**
 * Submit handler for adding a new image effect to an image style.
 */
function image_style_form_add_submit($form, &$form_state) {
  $style = $form_state['image_style'];
  // Check if this field has any configuration options.
  $effect = image_effect_definition_load($form_state['values']['new']);

  // Load the configuration form for this option.
  if (isset($effect['form callback'])) {
    $path = 'admin/config/media/image-styles/manage/' . $style->id() . '/add/' . $form_state['values']['new'];
    $form_state['redirect'] = array($path, array('query' => array('weight' => $form_state['values']['weight'])));
  }
  // If there's no form, immediately add the image effect.
  else {
    $effect = array(
      'name' => $effect['name'],
      'data' => array(),
      'weight' => $form_state['values']['weight'],
    );
    image_effect_save($style, $effect);
    drupal_set_message(t('The image effect was successfully applied.'));
  }
}

/**
 * Submit handler for saving an image style.
 */
function image_style_form_submit($form, &$form_state) {
  $style = $form_state['image_style'];

  // Update image effect weights.
  if (!empty($form_state['values']['effects'])) {
    foreach ($form_state['values']['effects'] as $ieid => $effect_data) {
      if (isset($style->effects[$ieid])) {
        $effect = array(
          'name' => $style->effects[$ieid]['name'],
          'data' => $style->effects[$ieid]['data'],
          'weight' => $effect_data['weight'],
          'ieid' => $ieid,
        );
        $style->effects[$ieid] = $effect;
      }
    }
  }

  $style->set('name', $form_state['values']['name']);
  $style->set('label', $form_state['values']['label']);
  $status = $style->save();

  if ($status == SAVED_UPDATED) {
    drupal_set_message(t('Changes to the style have been saved.'));
  }
  $form_state['redirect'] = 'admin/config/media/image-styles/manage/' . $style->id();
}

/**
 * Form builder; Form for adding a new image style.
 *
 * @ingroup forms
 * @see image_style_add_form_submit()
 */
function image_style_add_form($form, &$form_state) {
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrative label'),
    '#default_value' => '',
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => 'image_style_load',
    ),
    '#default_value' => '',
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create new style'),
  );

  return $form;
}

/**
 * Submit handler for adding a new image style.
 */
function image_style_add_form_submit($form, &$form_state) {
  $style = entity_create('image_style', array(
    'name' => $form_state['values']['name'],
    'label' => $form_state['values']['label'],
  ));
  $style->save();
  drupal_set_message(t('Style %name was created.', array('%name' => $style->label())));
  $form_state['redirect'] = 'admin/config/media/image-styles/manage/' . $style->id();
}

/**
 * Form builder; Form for adding and editing image effects.
 *
 * This form is used universally for editing all image effects. Each effect adds
 * its own custom section to the form by calling the 'form callback' specified
 * in hook_image_effect_info().
 *
 * @param $form_state
 *   An associative array containing the current state of the form.
 * @param $style
 *   An image style array.
 * @param $effect
 *   An image effect array.
 *
 * @ingroup forms
 * @see image_resize_form()
 * @see image_scale_form()
 * @see image_rotate_form()
 * @see image_crop_form()
 * @see image_effect_form_submit()
 */
function image_effect_form($form, &$form_state, $style, $effect) {
  // If there's no configuration for this image effect, return to
  // the image style page.
  if (!isset($effect['form callback'])) {
    drupal_goto('admin/config/media/image-styles/manage/' . $style->id());
  }
  $form_state['image_style'] = $style;
  $form_state['image_effect'] = $effect;

  if (!empty($effect['ieid'])) {
    $title = t('Edit %label effect', array('%label' => $effect['label']));
  }
  else{
    $title = t('Add %label effect', array('%label' => $effect['label']));
  }
  drupal_set_title($title, PASS_THROUGH);

  $form['#attached']['css'][drupal_get_path('module', 'image') . '/image.admin.css'] = array();

  $form['ieid'] = array(
    '#type' => 'value',
    '#value' => !empty($effect['ieid']) ? $effect['ieid'] : NULL,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $effect['name'],
  );

  $form['data'] = call_user_func($effect['form callback'], $effect['data']);
  $form['data']['#tree'] = TRUE;

  // Check the URL for a weight, then the image effect, otherwise use default.
  $form['weight'] = array(
    '#type' => 'hidden',
    '#value' => isset($_GET['weight']) ? intval($_GET['weight']) : (isset($effect['weight']) ? $effect['weight'] : count($style->effects)),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => !empty($effect['ieid']) ? t('Update effect') : t('Add effect'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/media/image-styles/manage/' . $style->id(),
  );

  return $form;
}

/**
 * Submit handler for updating an image effect.
 */
function image_effect_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);

  $effect = $form_state['values'];
  $style = $form_state['image_style'];
  image_effect_save($style, $effect);

  drupal_set_message(t('The image effect was successfully applied.'));
  $form_state['redirect'] = 'admin/config/media/image-styles/manage/' . $style->id();
}

/**
 * Element validate handler to ensure a hexadecimal color value.
 */
function image_effect_color_validate($element, &$form_state) {
  if ($element['#value'] != '') {
    $hex_value = preg_replace('/^#/', '', $element['#value']);
    if (!preg_match('/^#[0-9A-F]{3}([0-9A-F]{3})?$/', $element['#value'])) {
      form_error($element, t('!name must be a hexadecimal color value.', array('!name' => $element['#title'])));
    }
  }
}

/**
 * Element validate handler to ensure that either a height or a width is
 * specified.
 */
function image_effect_scale_validate($element, &$form_state) {
  if (empty($element['width']['#value']) && empty($element['height']['#value'])) {
    form_error($element, t('Width and height can not both be blank.'));
  }
}

/**
 * Form structure for the image resize form.
 *
 * Note that this is not a complete form, it only contains the portion of the
 * form for configuring the resize options. Therefore it does not not need to
 * include metadata about the effect, nor a submit button.
 *
 * @param $data
 *   The current configuration for this resize effect.
 */
function image_resize_form($data) {
  $form['width'] = array(
    '#type' => 'number',
    '#title' => t('Width'),
    '#default_value' => isset($data['width']) ? $data['width'] : '',
    '#field_suffix' => ' ' . t('pixels'),
    '#required' => TRUE,
    '#min' => 1,
  );
  $form['height'] = array(
    '#type' => 'number',
    '#title' => t('Height'),
    '#default_value' => isset($data['height']) ? $data['height'] : '',
    '#field_suffix' => ' ' . t('pixels'),
    '#required' => TRUE,
    '#min' => 1,
  );
  return $form;
}

/**
 * Form structure for the image scale form.
 *
 * Note that this is not a complete form, it only contains the portion of the
 * form for configuring the scale options. Therefore it does not not need to
 * include metadata about the effect, nor a submit button.
 *
 * @param $data
 *   The current configuration for this scale effect.
 */
function image_scale_form($data) {
  $form = image_resize_form($data);
  $form['#element_validate'] = array('image_effect_scale_validate');
  $form['width']['#required'] = FALSE;
  $form['height']['#required'] = FALSE;
  $form['upscale'] = array(
    '#type' => 'checkbox',
    '#default_value' => (isset($data['upscale'])) ? $data['upscale'] : 0,
    '#title' => t('Allow Upscaling'),
    '#description' => t('Let scale make images larger than their original size'),
  );
  return $form;
}

/**
 * Form structure for the image crop form.
 *
 * Note that this is not a complete form, it only contains the portion of the
 * form for configuring the crop options. Therefore it does not not need to
 * include metadata about the effect, nor a submit button.
 *
 * @param $data
 *   The current configuration for this crop effect.
 */
function image_crop_form($data) {
  $data += array(
    'width' => '',
    'height' => '',
    'anchor' => 'center-center',
  );

  $form = image_resize_form($data);
  $form['anchor'] = array(
    '#type' => 'radios',
    '#title' => t('Anchor'),
    '#options' => array(
      'left-top'      => t('Top') . ' ' . t('Left'),
      'center-top'    => t('Top') . ' ' . t('Center'),
      'right-top'     => t('Top') . ' ' . t('Right'),
      'left-center'   => t('Center') . ' ' . t('Left'),
      'center-center' => t('Center'),
      'right-center'  => t('Center') . ' ' . t('Right'),
      'left-bottom'   => t('Bottom') . ' ' . t('Left'),
      'center-bottom' => t('Bottom') . ' ' . t('Center'),
      'right-bottom'  => t('Bottom') . ' ' . t('Right'),
    ),
    '#theme' => 'image_anchor',
    '#default_value' => $data['anchor'],
    '#description' => t('The part of the image that will be retained during the crop.'),
  );

  return $form;
}

/**
 * Form structure for the image rotate form.
 *
 * Note that this is not a complete form, it only contains the portion of the
 * form for configuring the rotate options. Therefore it does not not need to
 * include metadata about the effect, nor a submit button.
 *
 * @param $data
 *   The current configuration for this rotate effect.
 */
function image_rotate_form($data) {
  $form['degrees'] = array(
    '#type' => 'number',
    '#default_value' => (isset($data['degrees'])) ? $data['degrees'] : 0,
    '#title' => t('Rotation angle'),
    '#description' => t('The number of degrees the image should be rotated. Positive numbers are clockwise, negative are counter-clockwise.'),
    '#field_suffix' => '&deg;',
    '#required' => TRUE,
  );
  $form['bgcolor'] = array(
    '#type' => 'textfield',
    '#default_value' => (isset($data['bgcolor'])) ? $data['bgcolor'] : '#FFFFFF',
    '#title' => t('Background color'),
    '#description' => t('The background color to use for exposed areas of the image. Use web-style hex colors (#FFFFFF for white, #000000 for black). Leave blank for transparency on image types that support it.'),
    '#size' => 7,
    '#maxlength' => 7,
    '#element_validate' => array('image_effect_color_validate'),
  );
  $form['random'] = array(
    '#type' => 'checkbox',
    '#default_value' => (isset($data['random'])) ? $data['random'] : 0,
    '#title' => t('Randomize'),
    '#description' => t('Randomize the rotation angle for each image. The angle specified above is used as a maximum.'),
  );
  return $form;
}

/**
 * Returns HTML for the page containing the list of image styles.
 *
 * @param $variables
 *   An associative array containing:
 *   - styles: An array of all the image styles returned by image_get_styles().
 *
 * @see image_get_styles()
 * @ingroup themeable
 */
function theme_image_style_list($variables) {
  $styles = $variables['styles'];

  $header = array(t('Style name'), t('Operations'));
  $rows = array();

  foreach ($styles as $style) {
    $row = array();
    $row[] = l($style->label(), 'admin/config/media/image-styles/manage/' . $style->id());
    $links = array();
    $links['edit'] = array(
      'title' => t('edit'),
      'href' => 'admin/config/media/image-styles/manage/' . $style->id(),
      'class' => array('image-style-link'),
    );
    $links['delete'] = array(
      'title' => t('delete'),
      'href' => 'admin/config/media/image-styles/manage/' . $style->id() . '/delete',
      'class' => array('image-style-link'),
    );
    $row[] = array(
      'data' => array(
        '#type' => 'operations',
        '#links' => $links,
      ),
    );
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array(
      'colspan' => 4,
      'data' => t('There are currently no styles. <a href="!url">Add a new one</a>.', array('!url' => url('admin/config/media/image-styles/add'))),
    ));
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Returns HTML for a listing of the effects within a specific image style.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_image_style_effects($variables) {
  $form = $variables['form'];
  $rows = array();

  foreach (element_children($form) as $key) {
    $row = array();
    $form[$key]['weight']['#attributes']['class'] = array('image-effect-order-weight');
    if ($key != 'new') {
      $summary = drupal_render($form[$key]['summary']);
      $row[] = drupal_render($form[$key]['label']) . (empty($summary) ? '' : ' ' . $summary);
      $row[] = drupal_render($form[$key]['weight']);
      $row[] = array('data' => $form[$key]['operations']);
    }
    else {
      // Add the row for adding a new image effect.
      $row[] = '<div class="image-style-new">' . drupal_render($form['new']['new']) . drupal_render($form['new']['add']) . '</div>';
      $row[] = drupal_render($form['new']['weight']);
      $row[] = '';
    }

    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  $header = array(
    t('Effect'),
    t('Weight'),
    t('Operations'),
  );

  if (count($rows) == 1 && (!isset($form['new']['#access']) || $form['new']['#access'])) {
    array_unshift($rows, array(array(
      'data' => t('There are currently no effects in this style. Add one by selecting an option below.'),
      'colspan' => 4,
    )));
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'image-style-effects')));
  drupal_add_tabledrag('image-style-effects', 'order', 'sibling', 'image-effect-order-weight');
  return $output;
}

/**
 * Prepare variables for image style preview templates.
 *
 * Default template: image-style-preview.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - style: The image style being previewed.
 */
function template_preprocess_image_style_preview(&$variables) {
  $style = $variables['style'];

  $sample_image = config('image.settings')->get('preview_image');
  $sample_width = 160;
  $sample_height = 160;

  // Set up original file information.
  $original_path = $sample_image;
  $original_image = image_get_info($original_path);
  if ($original_image['width'] > $original_image['height']) {
    $original_width = min($original_image['width'], $sample_width);
    $original_height = round($original_width / $original_image['width'] * $original_image['height']);
  }
  else {
    $original_height = min($original_image['height'], $sample_height);
    $original_width = round($original_height / $original_image['height'] * $original_image['width']);
  }
  $original_attributes = array_intersect_key($original_image, array('width' => '', 'height' => ''));
  $original_attributes['style'] = 'width: ' . $original_width . 'px; height: ' . $original_height . 'px;';

  // Set up preview file information.
  $preview_file = image_style_path($style->id(), $original_path);
  if (!file_exists($preview_file)) {
    image_style_create_derivative($style, $original_path, $preview_file);
  }
  $preview_image = image_get_info($preview_file);
  if ($preview_image['width'] > $preview_image['height']) {
    $preview_width = min($preview_image['width'], $sample_width);
    $preview_height = round($preview_width / $preview_image['width'] * $preview_image['height']);
  }
  else {
    $preview_height = min($preview_image['height'], $sample_height);
    $preview_width = round($preview_height / $preview_image['height'] * $preview_image['width']);
  }
  $preview_attributes = array_intersect_key($preview_image, array('width' => '', 'height' => ''));
  $preview_attributes['style'] = 'width: ' . $preview_width . 'px; height: ' . $preview_height . 'px;';

  $variables['style_name'] = check_plain($style->label());

  // Original image variables.
  $variables['original_url'] = file_create_url($original_path);
  $variables['original_attributes'] = new Attribute($original_attributes);
  $original_attributes['src'] = file_create_url($original_path);
  $original_attributes['alt'] = t('Sample original image');
  $variables['original_image'] = array(
    '#theme' => 'image',
    '#attributes' => new Attribute($original_attributes),
  );
  $variables['original_image_height'] = $original_image['height'];
  $variables['original_image_width'] = $original_image['width'];
  $variables['original_height'] = $original_height;
  $variables['original_width'] = $original_width;

  // Preview image variables.
  $variables['preview_url'] = file_create_url($preview_file) . '?' . time();
  $variables['preview_attributes'] = new Attribute($preview_attributes);
  $preview_attributes['src'] = file_create_url($preview_file) . '?cache_bypass=' . REQUEST_TIME;
  $preview_attributes['alt'] = t('Sample modified image');
  $variables['preview_image'] = array(
    '#theme' => 'image',
    '#attributes' => new Attribute($preview_attributes),
  );
  $variables['preview_image_height'] = $preview_image['height'];
  $variables['preview_image_width'] = $preview_image['width'];
  $variables['preview_height'] = $preview_height;
  $variables['preview_width'] = $preview_width;
}

/**
 * Prepares variables for image anchor templates.
 *
 * Default template: image-anchor.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - element: An associative array containing the image.
 */
function template_preprocess_image_anchor(&$variables) {
  $element = $variables['element'];

  $rows = array();
  $row = array();
  foreach (element_children($element) as $n => $key) {
    $element[$key]['#attributes']['title'] = $element[$key]['#title'];
    unset($element[$key]['#title']);
    $row[] = array(
      'data' => $element[$key],
    );
    if ($n % 3 == 3 - 1) {
      $rows[] = $row;
      $row = array();
    }
  }
  $variables['table'] = array(
    '#theme' => 'table',
    '#header' => array(),
    '#rows' => $rows,
    '#attributes' => array(
      'class' => array('image-anchor'),
    ),
  );
}

/**
 * Prepares variables for image resize summary templates.
 *
 * Default template: image-resize-summary.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: The current configuration for this resize effect.
 */
function template_preprocess_image_resize_summary(&$variables) {
  $output = '';
  $data = $variables['data'];

  if ($data['width'] && $data['height']) {
    $output = check_plain($data['width']) . 'x' . check_plain($data['height']);
  }
  else {
    $output = ($data['width']) ? t('width @width', array('@width' => $data['width'])) : t('height @height', array('@height' => $data['height']));
  }

  if(isset($data['upscale'])) {
    $output .= ' (' . t('upscaling allowed') . ')';
  }

  $variables['summary'] = $output;
}

/**
 * Prepares variables for image scale summary templates.
 *
 * Default template: image-scale-summary.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: The current configuration for this scale effect.
 */
function template_preprocess_image_scale_summary(&$variables) {
  $variables['summary'] = array(
    '#theme' => 'image_resize_summary',
    '#data' => $variables['data'],
  );
}

/**
 * Prepares variables for image crop summary templates.
 *
 * Default template: image-crop-summary.html.twig
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: The current configuration for this crop effect.
 */
function template_preprocess_image_crop_summary(&$variables) {
  $variables['summary'] = array(
    '#theme' => 'image_resize_summary',
    '#data' => $variables['data'],
  );
}

/**
 * Prepares varaibles for image rotate summary templates.
 *
 * Default template: image-rotate-summary-html-twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: The current configuration for this rotate effect.
 */
function template_preprocess_image_rotate_summary(&$variables) {
  $data = $variables['data'];
  $variables['summary'] = ($data['random']) ? t('random between -@degrees&deg and @degrees&deg', array('@degrees' => str_replace('-', '', $data['degrees']))) : t('@degrees&deg', array('@degrees' => $data['degrees']));
}
