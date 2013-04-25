<?php

/**
 * @file
 * Preprocessors and theme functions for the Views UI.
 */

use Drupal\Core\Template\Attribute;

/**
 * Prepares variables for Views UI container templates.
 *
 * Default template: views-ui-container.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #attributes, #children.
 */
function template_preprocess_views_ui_container(&$variables) {
  $variables['attributes'] = new Attribute($variables['element']['#attributes']);
  $variables['children'] = $variables['element']['#children'];
}

function template_preprocess_views_ui_display_tab_setting(&$variables) {
  static $zebra = 0;
  $variables['zebra'] = ($zebra % 2 === 0 ? 'odd' : 'even');
  $zebra++;

  // Put the main link to the left side
  array_unshift($variables['settings_links'], $variables['link']);
  $variables['settings_links'] = implode('<span class="label">&nbsp;|&nbsp;</span>', $variables['settings_links']);

  if (!empty($variables['defaulted'])) {
    $variables['attributes']['class'][] = 'defaulted';
  }
  if (!empty($variables['overridden'])) {
    $variables['attributes']['class'][] = 'overridden';
    $variables['attributes_array']['title'][] = t('Overridden');
  }

  // Append a colon to the description, if requested.
  if ($variables['description'] && $variables['description_separator']) {
    $variables['description'] .= t(':');
  }
}

/**
 * Prepares variables for Views UI display tab bucket templates.
 *
 * Default template: views-ui-display-tab-bucket.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #name, #overridden, #children, #title, #actions.
 */
function template_preprocess_views_ui_display_tab_bucket(&$variables) {
  $element = $variables['element'];

  if (!empty($element['#name'])) {
    $variables['attributes']['class'][] = drupal_html_class($element['#name']);
  }
  if (!empty($element['#overridden'])) {
    $variables['attributes']['class'][] = 'overridden';
    $variables['attributes']['title'][] = t('Overridden');
  }

  $variables['content'] = $element['#children'];
  $variables['title'] = $element['#title'];
  $variables['actions'] = !empty($element['#actions']) ? $element['#actions'] : array();
}

/**
 * Implements hook_preprocess_HOOK() for theme_views_ui_view_info().
 */
function template_preprocess_views_ui_view_info(&$variables) {
  $variables['title'] = $variables['view']->label();

  $displays = $variables['view']->getDisplaysList();
  $variables['displays'] = empty($displays) ? t('None') : format_plural(count($displays), 'Display', 'Displays') . ': ' . '<em>' . implode(', ', $displays) . '</em>';
}

/**
 * Returns basic administrative information about a view.
 */
function theme_views_ui_view_info($variables) {
  $output = '';
  $output .= '<h3 class="views-ui-view-title">' . $variables['title'] . "</h3>\n";
  $output .= '<div class="views-ui-view-displays">' . $variables['displays'] . "</div>\n";
  return $output;
}

/**
 * Prepares variables for Views UI expose filter form.
 *
 * Default template: views-ui-expose-filter-form.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 */
function template_preprocess_views_ui_expose_filter_form(&$variables) {
  $form = $variables['form'];
  $variables['form_description'] = $form['form_description'];
  $variables['expose_button'] = $form['expose_button'];
  $variables['group_button'] = $form['group_button'];
  unset($form['form_description']);
  unset($form['expose_button']);
  unset($form['group_button']);

  if (isset($form['required'])) {
    $variables['required'] = $form['required'];
    unset($form['required']);
  }

  $variables['label'] = $form['label'];
  $variables['description'] = $form['description'];
  unset($form['label']);
  unset($form['description']);

  $variables['operator'] = $form['operator'];

  // Only output the right column markup if there's a left column to begin with.
  if (!empty($form['operator']['#type'])) {
    $variables['has_left_column'] = TRUE;
  }
  else {
    $variables['has_left_column'] = FALSE;
  }
  unset($form['operator']);

  $variables['value'] = $form['value'];
  unset($form['value']);

  if (isset($form['use_operator'])) {
    $variables['use_operator'] = $form['use_operator'];
    unset($form['use_operator']);
  }

  $variables['more'] = $form['more'];
  unset($form['more']);

  // Remove elements from the 'children' render array.
  // @todo Refactor when http://drupal.org/node/1920886 is resolved.
  $variables['children'] = drupal_render_children($form);
}

/**
 * Prepares variables for a Views UI build group filter form templates.
 *
 * Default template: views-ui-build-group-filter-form.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 */
function template_preprocess_views_ui_build_group_filter_form(&$variables) {
  $form = $variables['form'];

  $variables['form_description'] = $form['form_description'];
  $variables['expose_button'] = $form['expose_button'];
  $variables['group_button'] = $form['group_button'];
  unset($form['form_description']);
  unset($form['expose_button']);
  unset($form['group_button']);

  if (isset($form['required'])) {
    $variables['required'] = $form['required'];
    unset($form['required']);
  }

  if (isset($form['operator'])) {
    $variables['operator'] = $form['operator'];
    unset($form['operator']);
  }

  if (isset($form['value'])) {
    $variables['value'] = $form['value'];
    unset($form['value']);
  }

  $variables['optional'] = $form['optional'];
  $variables['remember'] = $form['remember'];
  unset($form['optional']);
  unset($form['remember']);

  $variables['widget'] = $form['widget'];
  $variables['label'] = $form['label'];
  $variables['description'] = $form['description'];
  unset($form['widget']);
  unset($form['label']);
  unset($form['description']);

  $variables['table'] = array(
    '#type' => 'table',
    '#header' => array(
      t('Default'),
      t('Weight'),
      t('Label'),
      t('Operator'),
      t('Value'),
      t('Operations'),
    ),
    '#tabledrag' => array(
      array('order', 'sibling', 'weight'),
    ),
    '#attributes' => array(
      'class' => array('views-filter-groups'),
      'id' => 'views-filter-groups',
    ),
  );

  $form['default_group'] = form_process_radios($form['default_group']);
  $form['default_group_multiple'] = form_process_checkboxes($form['default_group_multiple']);
  $form['default_group']['All']['#title'] = '';

  hide($form['default_group_multiple']['All']);
  $variables['table']['#rows'][] = array(
    array(
      'data' => $form['default_group']['All'],
    ),
    '',
    array(
      'data' => config('views.settings')->get('ui.exposed_filter_any_label') == 'old_any' ? t('&lt;Any&gt;') : t('- Any -'),
      'colspan' => 4,
      'class' => array('class' => 'any-default-radios-row'),
    ),
  );

  foreach (element_children($form['group_items']) as $group_id) {
    $form['group_items'][$group_id]['value']['#title'] = '';
    $remove_link = array(
      '#theme' => 'link',
      '#path' => 'javascript:void()',
      '#text' => '<span>' . t('Remove') . '</span>',
      '#options' => array(
        'attributes' => array(
          'id' => 'views-remove-link-' . $group_id,
          'class' => array(
            'views-hidden',
            'views-button-remove',
            'views-groups-remove-link',
            'views-remove-link',
          ),
          'alt' => t('Remove this item'),
          'title' => t('Remove this item'),
        ),
        'html' => TRUE,
      ),
    );
    $data = array(
      'default' => array(
        'data' => array(
          $form['default_group'][$group_id],
          $form['default_group_multiple'][$group_id],
        ),
      ),
      'weight' => array(
        'data' => $form['group_items'][$group_id]['weight'],
      ),
      'title' => array(
        'data' => $form['group_items'][$group_id]['title'],
      ),
      'operator' => array(
        'data' => $form['group_items'][$group_id]['operator'],
      ),
      'value' => array(
        'data' => $form['group_items'][$group_id]['value'],
      ),
      'remove' => array(
        'data' => array(
          $form['group_items'][$group_id]['remove'],
          $remove_link,
        ),
      ),
    );
    $variables['table']['#rows'][] = array(
      'data' => $data,
      'id' => 'views-row-' . $group_id,
      'class' => array('draggable'),
    );
  }
  unset($form['group_items']);
  unset($form['default_group']);
  unset($form['default_group_multiple']);

  $variables['add_group'] = $form['add_group'];
  $variables['more'] = $form['more'];
  unset($form['add_group']);
  unset($form['more']);

  // Remove elements from the 'children' render array.
  // @todo Refactor when http://drupal.org/node/1920886 is resolved.
  $variables['children'] = drupal_render_children($form);
}

/**
 * Turn the reorder form into a proper table
 */
function theme_views_ui_reorder_displays_form($vars) {
  $form = $vars['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#display'])) {
      $display = &$form[$key];

      $row = array();
      $row[] = drupal_render($display['title']);
      $form[$key]['weight']['#attributes']['class'] = array('weight');
      $row[] = drupal_render($form[$key]['weight']);
      if (isset($display['removed'])) {
        $row[] = drupal_render($form[$key]['removed']) .
          l('<span>' . t('Remove') . '</span>',
            'javascript:void()',
            array(
              'attributes' => array(
                'id' => 'display-remove-link-' . $key,
                'class' => array('views-button-remove display-remove-link'),
                'alt' => t('Remove this display'),
                'title' => t('Remove this display')),
              'html' => TRUE));
      }
      else {
        $row[] = '';
      }
      $class = array();
      $styles = array();
      if (isset($form[$key]['weight']['#type'])) {
        $class[] = 'draggable';
      }
      if (isset($form[$key]['deleted']['#value']) && $form[$key]['deleted']['#value']) {
        $styles[] = 'display: none;';
      }
      $rows[] = array('data' => $row, 'class' => $class, 'id' => 'display-row-' . $key, 'style' => $styles);
    }
  }

  $header = array(t('Display'), t('Weight'), t('Remove'));
  $output = '';
  drupal_add_tabledrag('reorder-displays', 'order', 'sibling', 'weight');

  $output = drupal_render($form['override']);
  $output .= '<div class="scroll">';
  $output .= theme('table',
    array('header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'reorder-displays'),
  ));
  $output .= '</div>';
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Prepares variables for Views UI rearrange filter form templates.
 *
 * Default template: views-ui-rearrange-filter-form.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 */
function template_preprocess_views_ui_rearrange_filter_form(&$vars) {
  $form = $vars['form'];
  $rows = $ungroupable_rows = array();
  // Enable grouping only if > 1 group.
  $grouping = count(array_keys($form['#group_options'])) > 1;

  foreach ($form['#group_renders'] as $group_id => $contents) {
    // Header row for the group.
    if ($group_id !== 'ungroupable') {
      // Set up tabledrag so that it changes the group dropdown when rows are
      // dragged between groups.
      drupal_add_tabledrag('views-rearrange-filters', 'match', 'sibling', 'views-group-select', 'views-group-select-' . $group_id);

      // Title row, spanning all columns.
      $row = array();
      // Add a cell to the first row, containing the group operator.
      $row[] = array(
        'class' => array('group', 'group-operator', 'container-inline'),
        'data' => $form['filter_groups']['groups'][$group_id],
        'rowspan' => max(array(2, count($contents) + 1)),
      );
      // Title.
      $row[] = array(
        'class' => array('group', 'group-title'),
        'data' => '<span>' . $form['#group_options'][$group_id] . '</span>',
        'colspan' => 4,
      );
      $rows[] = array(
        'class' => array('views-group-title'),
        'data' => $row,
        'id' => 'views-group-title-' . $group_id,
      );

      // Row which will only appear if the group has nothing in it.
      $row = array();
      $class = 'group-' . (count($contents) ? 'populated' : 'empty');
      $instructions = '<span>' . t('No filters have been added.') . '</span> <span class="js-only">' . t('Drag to add filters.') . '</span>';
      // When JavaScript is enabled, the button for removing the group (if it's
      // present) should be hidden, since it will be replaced by a link on the
      // client side.
      if (!empty($form['remove_groups'][$group_id]['#type']) && $form['remove_groups'][$group_id]['#type'] == 'submit') {
        $form['remove_groups'][$group_id]['#attributes']['class'][] = 'js-hide';
      }
      $row[] = array(
        'colspan' => 5,
        'data' => array(
          $instructions,
          $form['remove_groups'][$group_id],
        )
      );
      $rows[] = array(
        'class' => array(
          'group-message',
          'group-' . $group_id . '-message',
          $class,
        ),
        'data' => $row,
        'id' => 'views-group-' . $group_id,
      );
    }

    foreach ($contents as $id) {
      if (isset($form['filters'][$id]['name'])) {
        $row = array();
        $row[]['data'] = $form['filters'][$id]['name'];
        $form['filters'][$id]['weight']['#attributes']['class'] = array('weight');
        $row[]['data'] = $form['filters'][$id]['weight'];
        $form['filters'][$id]['group']['#attributes']['class'] = array('views-group-select views-group-select-' . $group_id);
        $row[]['data'] = $form['filters'][$id]['group'];
        $form['filters'][$id]['removed']['#attributes']['class'][] = 'js-hide';
        $remove_link = array(
          '#theme' => 'link',
          '#path' => 'javascript:void()',
          '#text' => '<span>' . t('Remove') . '</span>',
          '#options' => array(
            'attributes' => array(
              'id' => 'views-remove-link-' . $id,
              'class' => array(
                'views-hidden',
                'views-button-remove',
                'views-groups-remove-link',
                'views-remove-link',
              ),
              'alt' => t('Remove this item'),
              'title' => t('Remove this item'),
            ),
            'html' => TRUE,
          ),
        );
        $row[]['data'] = array(
          $form['filters'][$id]['removed'],
          $remove_link,
        );

        $row = array(
          'data' => $row,
          'class' => array('draggable'),
          'id' => 'views-row-' . $id,
        );
        if ($group_id !== 'ungroupable') {
          $rows[] = $row;
        }
        else {
          $ungroupable_rows[] = $row;
        }
      }
    }
  }

  unset($form['filters']);
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No fields available.'), 'colspan' => '2'));
  }

  if (isset($form['override'])) {
    $variables['override'] = $form['override'];
    unset($form['override']);
  }

  $variables['grouping'] = $grouping;
  if ($grouping) {
    $variables['filter_groups_operator'] = $form['filter_groups']['operator'];
  }
  else {
    // @todo find out what type these are to move the #title to the template.
    $form['filter_groups']['groups'][0]['#title'] = t('Operator');
    $variables['filter_groups_group'] = $form['filter_groups']['groups'][0];
  }

  if (!empty($ungroupable_rows)) {
    drupal_add_tabledrag('views-rearrange-filters-ungroupable', 'order', 'sibling', 'weight');
    $header = array(
      t('Ungroupable filters'),
      t('Weight'),
      array('class' => array('views-hide-label'), 'data' => t('Group')),
      array('class' => array('views-hide-label'), 'data' => t('Remove')),
    );
    $variables['ungroupable_table'] = array(
      '#type' => 'table',
      '#header' => $header,
      '#rows' => $ungroupable_rows,
      '#attributes' => array('id' => 'views-rearrange-filters-ungroupable', 'class' => array('arrange')),
    );
  }

  // Set up tabledrag so that the weights are changed when rows are dragged.
  drupal_add_tabledrag('views-rearrange-filters', 'order', 'sibling', 'weight');
  $variables['table'] = array(
    '#type' => 'table',
    '#rows' => $rows,
    '#attributes' => array('id' => 'views-rearrange-filters', 'class' => array('arrange')),
  );

  unset($form['filter_groups']);
  unset($form['remove_groups']);

  // When JavaScript is enabled, the button for adding a new group should be
  // hidden, since it will be replaced by a link on the client side.
  $form['buttons']['add_group']['#attributes']['class'][] = 'js-hide';

  // Render the rest of the form and return.
  // Remove elements from the 'children' render array.
  // @todo Refactor when http://drupal.org/node/1920886 is resolved.
  $variables['children'] = drupal_render_children($form);
}

/**
 * Prepares variables for style plugin table templates.
 *
 * Default template: views-ui-style-plugin-table.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
*/
function template_preprocess_views_ui_style_plugin_table(&$variables) {
  $form = $variables['form'];

  $variables['description'] = $form['description_markup'];

  $header = array(
    t('Field'),
    t('Column'),
    t('Align'),
    t('Separator'),
    array(
      'data' => t('Sortable'),
      'align' => 'center',
    ),
    array(
      'data' => t('Default order'),
      'align' => 'center',
    ),
    array(
      'data' => t('Default sort'),
      'align' => 'center',
    ),
    array(
      'data' => t('Hide empty column'),
      'align' => 'center',
    ),
    array(
      'data' => t('Responsive'),
      'align' => 'center',
    ),
  );
  $rows = array();
  foreach (element_children($form['columns']) as $id) {
    $row = array();
    $row[]['data'] = $form['info'][$id]['name'];
    $row[]['data'] = $form['columns'][$id];
    $row[]['data'] = $form['info'][$id]['align'];
    $row[]['data'] = $form['info'][$id]['separator'];

    // Unset so they don't get rendered in drupal_render_children.
    unset($form['info'][$id]['name']);
    unset($form['columns'][$id]);
    unset($form['info'][$id]['align']);
    unset($form['info'][$id]['separator']);
    if (!empty($form['info'][$id]['sortable'])) {
      $row[] = array(
        'data' => $form['info'][$id]['sortable'],
        'align' => 'center',
      );
      $row[] = array(
        'data' => $form['info'][$id]['default_sort_order'],
        'align' => 'center',
      );
      $row[] = array(
        'data' => $form['default'][$id],
        'align' => 'center',
      );
      // Unset so they don't get rendered in drupal_render_children.
      unset($form['info'][$id]['sortable']);
      unset($form['info'][$id]['default_sort_order']);
    }
    else {
      $row[] = '';
      $row[] = '';
      $row[] = '';
    }
    $row[] = array(
      'data' => $form['info'][$id]['empty_column'],
      'align' => 'center',
    );
    $row[] = array(
      'data' => $form['info'][$id]['responsive'],
      'align' => 'center',
    );
    $rows[] = $row;
    // Unset so they don't get rendered in drupal_render_children.
    unset($form['info'][$id]['empty_column']);
    unset($form['info'][$id]['responsive']);
  }

  // Add the special 'None' row.
  $rows[] = array(t('None'), '', '', '', '', '', array('align' => 'center', 'data' => $form['default'][-1]), '', '');
  unset($form['default']);
  $variables['table'] = array(
    '#type' => 'table',
    '#theme' => 'table__views_ui_style_plugin_table',
    '#header' => $header,
    '#rows' => $rows
  );
  $variables['children'] = drupal_render_children($form);
}

/**
 * Prepares variables for Views UI view preview section templates.
 *
 * Default template: views-ui-view-preview-section.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - view: The View object.
 *   - section: The section name of a View (e.g. title, rows or pager).
 */
function template_preprocess_views_ui_view_preview_section(&$vars) {
  switch ($vars['section']) {
    case 'title':
      $vars['title'] = t('Title');
      $links = views_ui_view_preview_section_display_category_links($vars['view'], 'title', $vars['title']);
      break;
    case 'header':
      $vars['title'] = t('Header');
      $links = views_ui_view_preview_section_handler_links($vars['view'], $vars['section']);
      break;
    case 'empty':
      $vars['title'] = t('No results behavior');
      $links = views_ui_view_preview_section_handler_links($vars['view'], $vars['section']);
      break;
    case 'exposed':
      // @todo Sorts can be exposed too, so we may need a better title.
      $vars['title'] = t('Exposed Filters');
      $links = views_ui_view_preview_section_display_category_links($vars['view'], 'exposed_form_options', $vars['title']);
      break;
    case 'rows':
      // @todo The title needs to depend on what is being viewed.
      $vars['title'] = t('Content');
      $links = views_ui_view_preview_section_rows_links($vars['view']);
      break;
    case 'pager':
      $vars['title'] = t('Pager');
      $links = views_ui_view_preview_section_display_category_links($vars['view'], 'pager_options', $vars['title']);
      break;
    case 'more':
      $vars['title'] = t('More');
      $links = views_ui_view_preview_section_display_category_links($vars['view'], 'use_more', $vars['title']);
      break;
    case 'footer':
      $vars['title'] = t('Footer');
      $links = views_ui_view_preview_section_handler_links($vars['view'], $vars['section']);
      break;
    case 'attachment_before':
      // @todo: Add links to the attachment configuration page.
      $vars['title'] = t('Attachment before');
      break;
    case 'attachment_after':
      // @todo: Add links to the attachment configuration page.
      $vars['title'] = t('Attachment after');
      break;
  }

  if (isset($links)) {
    $build = array(
      '#theme' => 'links__contextual',
      '#links' => $links,
      '#attributes' => array('class' => array('contextual-links')),
      '#attached' => array(
        'library' => array(array('contextual', 'drupal.contextual-links')),
      ),
    );
    $vars['links'] = $build;
  }
  $vars['theme_hook_suggestions'][] = 'views_ui_view_preview_section__' . $vars['section'];
}
