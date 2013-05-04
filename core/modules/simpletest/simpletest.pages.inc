<?php

/**
 * @file
 * Page callbacks for simpletest module.
 */

use Drupal\Core\Template\Attribute;

/**
 * List tests arranged in groups that can be selected and run.
 */
function simpletest_test_form($form, &$form_state) {
  $form['tests'] = array(
    '#type' => 'details',
    '#title' => t('Tests'),
    '#description' => t('Select the test(s) or test group(s) you would like to run, and click <em>Run tests</em>.'),
  );

  $form['tests']['table'] = array(
    '#theme' => 'simpletest_test_table',
  );

  // Generate the list of tests arranged by group.
  $groups = simpletest_test_get_all();
  $groups['PHPUnit'] = simpletest_phpunit_get_available_tests();
  $form_state['storage']['PHPUnit'] = $groups['PHPUnit'];

  foreach ($groups as $group => $tests) {
    $form['tests']['table'][$group] = array(
      '#collapsed' => TRUE,
    );

    foreach ($tests as $class => $info) {
      $form['tests']['table'][$group][$class] = array(
        '#type' => 'checkbox',
        '#title' => $info['name'],
        '#description' => $info['description'],
      );
    }
  }

  // Operation buttons.
  $form['tests']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Run tests'),
  );
  $form['clean'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clean test environment'),
    '#description' => t('Remove tables with the prefix "simpletest" and temporary directories that are left over from tests that crashed. This is intended for developers when creating tests.'),
  );
  $form['clean']['op'] = array(
    '#type' => 'submit',
    '#value' => t('Clean environment'),
    '#submit' => array('simpletest_clean_environment'),
  );

  return $form;
}

/**
 * Prepares variables for test list table templates.
 *
 * Default template: simpletest-test-table.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - table: A render element representing the table.
 */
function template_preprocess_simpletest_test_table(&$variables) {
  $table = $variables['table'];

  drupal_add_library('simpletest', 'drupal.simpletest');

  // Define the images used to expand/collapse the test groups.
  $js = array(
    'images' => array(
      theme('image', array('uri' => 'core/misc/menu-collapsed.png', 'width' => 7, 'height' => 7, 'alt' => t('Expand'), 'title' => t('Expand'))) . ' <a href="#" class="simpletest-collapse">(' . t('Expand') . ')</a>',
      theme('image', array('uri' => 'core/misc/menu-expanded.png', 'width' => 7, 'height' => 7, 'alt' => t('Collapse'), 'title' => t('Collapse'))) . ' <a href="#" class="simpletest-collapse">(' . t('Collapse') . ')</a>',
    ),
  );

  // Cycle through each test group and create a row.

  $variables['test_groups'] = array();
  foreach (element_children($table) as $key) {
    $element = &$table[$key];

    $variables['test_groups'][$key] = new stdClass();

    // Make the class name safe for output on the page by replacing all
    // non-word/decimal characters with a dash (-).
    $test_class = strtolower(trim(preg_replace("/[^\w\d]/", "-", $key)));
    $variables['test_groups'][$key]->test_class = $test_class;

    // Select the right "expand"/"collapse" image, depending on whether the
    // category is expanded (at least one test selected) or not.
    $collapsed = !empty($element['#collapsed']);
    $image_index = $collapsed ? 0 : 1;

    // Expand/collapse image and group title.
    $variables['test_groups'][$key]->label = $key;

    // Add individual tests to group.
    $current_js = array(
      'testClass' => $test_class . '-test',
      'testNames' => array(),
      'imageDirection' => $image_index,
      'clickActive' => FALSE,
    );

    // Sorting $element by children's #title attribute instead of by class name.
    uasort($element, 'element_sort_by_title');

    $variables['test_groups'][$key]->tests = array();

    // Cycle through each test within the current group.
    foreach (element_children($element) as $test_name) {
      $test = $element[$test_name];
      $test_data = new stdClass();
      $test_data->attributes = new Attribute(array('class' => array()));
      $test_data->id = $test['#id'];

      $current_js['testNames'][] = $test['#id'];

      // Store test title and description so that checkbox won't render them.
      $test_data->title = $test['#title'];
      $test_data->description = $test['#description'];
      unset($test['#description']);

      $test['#title_display'] = 'invisible';

      // Test name is used to determine what tests to run.
      $test['#name'] = $test_name;

      // Add the checkbox field.
      $test_data->field = $test;

      $test_data->attributes['class'][] = $test_class . '-test';
      $test_data->attributes['class'][] = ($collapsed ? 'js-hide' : '');

      $variables['test_groups'][$key]->tests[$test_name] = $test_data;
    }
    $js['simpletest-test-group-' . $test_class] = $current_js;
  }

  // Add js array of settings.
  drupal_add_js(array('simpleTest' => $js), 'setting');
}

/**
 * Run selected tests.
 */
function simpletest_test_form_submit($form, &$form_state) {
  // Get list of tests.
  $tests_list = array();
  simpletest_classloader_register();

  $phpunit_all = array_keys($form_state['storage']['PHPUnit']);

  foreach ($form_state['values'] as $class_name => $value) {
    // Since class_exists() will likely trigger an autoload lookup,
    // we do the fast check first.
    if ($value === 1 && class_exists($class_name)) {
      $test_type = in_array($class_name, $phpunit_all) ? 'UnitTest' : 'WebTest';
      $tests_list[$test_type][] = $class_name;
    }
  }
  if (count($tests_list) > 0 ) {
    $test_id = simpletest_run_tests($tests_list, 'drupal');
    $form_state['redirect'] = 'admin/config/development/testing/results/' . $test_id;
  }
  else {
    drupal_set_message(t('No test(s) selected.'), 'error');
  }
}
