<?php

use Drupal\Core\Template\Attribute;

/**
 * @file
 * Code required only when rendering the available updates report.
 */

/**
 * Page callback: Generates a page about the update status of projects.
 *
 * @see update_menu()
 */
function update_status() {
  $build = array();
  $last = state()->get('update.last_check') ?: 0;
  $build['last_check'] = array(
    '#theme' => 'update_last_check',
    '#last' => $last,
  );
  if ($available = update_get_available(TRUE)) {
    module_load_include('inc', 'update', 'update.compare');
    $data = update_calculate_project_data($available);
  }
  else {
    $data = _update_no_data();
  }

  if (!is_array($data)) {
    $build['data'] = array(
      '#markup' => $data,
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
    return $build;
  }

  $build['report'] = array(
    '#theme' => 'update_report',
    '#data' => $data,
  );
  return $build;
}

/**
 * Prepares variables the project status report.
 *
 * Default template: update-report.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - data: An array of data about each project's status.
 */
function template_preprocess_update_report(&$variables) {
  $data = $variables['data'];
  $rows = array();

  $notification_level = config('update.settings')->get('notification.threshold');

  // Create an array of status values keyed by module or theme name, since
  // we'll need this while generating the report if we have to cross reference
  // anything (e.g. subthemes which have base themes missing an update).
  foreach ($data as $project) {
    foreach ($project['includes'] as $key => $name) {
      $status[$key] = $project['status'];
    }
  }

  foreach ($data as $project) {
    switch ($project['status']) {
      case UPDATE_CURRENT:
        $class = 'ok';
        $icon = array(
          '#theme' => 'image',
          '#uri' => 'core/misc/watchdog-ok.png',
          '#width' => 18,
          '#height' => 18,
          '#alt' => t('ok'),
          '#title' => t('ok'),
        );
        break;
      case UPDATE_UNKNOWN:
      case UPDATE_FETCH_PENDING:
      case UPDATE_NOT_FETCHED:
        $class = 'unknown';
        $icon = array(
          '#theme' => 'image',
          '#uri' => 'core/misc/watchdog-warning.png',
          '#width' => 18,
          '#height' => 18,
          '#alt' => t('warning'),
          '#title' => t('warning'),
        );
        break;
      case UPDATE_NOT_SECURE:
      case UPDATE_REVOKED:
      case UPDATE_NOT_SUPPORTED:
        $class = 'error';
        $icon = array(
          '#theme' => 'image',
          '#uri' => 'core/misc/watchdog-error.png',
          '#width' => 18,
          '#height' => 18,
          '#alt' => t('error'),
          '#title' => t('error'),
        );
        break;
      case UPDATE_NOT_CHECKED:
      case UPDATE_NOT_CURRENT:
      default:
        $class = 'warning';
        $icon = array(
          '#theme' => 'image',
          '#uri' => 'core/misc/watchdog-warning.png',
          '#width' => 18,
          '#height' => 18,
          '#alt' => t('warning'),
          '#title' => t('warning'),
        );
        break;
    }

    // Used to collect the project values.
    $row = array();
    $row['attributes'] = new Attribute(array('class' => $class));
    $row['icon'] = $icon;

    // Rendered status label here because we can't check that a TwigReference
    // is empty in the twig file.
    $status_label = array(
      '#theme' => 'update_status_label',
      '#status' => $project['status'],
    );
    $row['status_label'] = drupal_render($status_label);

    if (!empty($project['reason'])) {
      $row['reason'] = check_plain($project['reason']);
    }
    else {
      $row['reason'] = '';
    }

    // Set the project title.
    if (isset($project['title'])) {
      if (isset($project['link'])) {
        $row['title'] = l($project['title'], $project['link']);
      }
      else {
        $row['title'] = check_plain($project['title']);
      }
    }
    else {
      $row['title'] = check_plain($project['name']);
    }

    // Clean up the version for twig.
    $row['existing_version'] = check_plain($project['existing_version']);

    if ($project['install_type'] == 'dev' && !empty($project['datestamp'])) {
      $row['datestamp'] = format_date($project['datestamp'], 'custom', 'Y-M-d');
    }

    $versions_inner = array();
    $security_class = array();
    $version_class = array();
    if (isset($project['recommended'])) {
      if ($project['status'] != UPDATE_CURRENT || $project['existing_version'] !== $project['recommended']) {

        // First, figure out what to recommend.
        // If there's only 1 security update and it has the same version we're
        // recommending, give it the same CSS class as if it was recommended,
        // but don't print out a separate "Recommended" line for this project.
        if (!empty($project['security updates'])
            && count($project['security updates']) == 1
            && $project['security updates'][0]['version'] === $project['recommended']
            ) {
          $security_class[] = 'version-recommended';
          $security_class[] = 'version-recommended-strong';
        }
        else {
          $version_class[] = 'version-recommended';
          // Apply an extra class if we're displaying both a recommended
          // version and anything else for an extra visual hint.
          if ($project['recommended'] !== $project['latest_version']
              || !empty($project['also'])
              || ($project['install_type'] == 'dev'
                && isset($project['dev_version'])
                && $project['latest_version'] !== $project['dev_version']
                && $project['recommended'] !== $project['dev_version'])
              || (isset($project['security updates'][0])
                && $project['recommended'] !== $project['security updates'][0])
              ) {
            $version_class[] = 'version-recommended-strong';
          }
          $versions_inner[] = array(
            '#theme' => 'update_version',
            '#version' => $project['releases'][$project['recommended']],
            '#tag' => t('Recommended version:'),
            '#class' => $version_class,
          );
        }

        // Now, print any security updates.
        if (!empty($project['security updates'])) {
          $security_class[] = 'version-security';
          foreach ($project['security updates'] as $security_update) {
            $versions_inner[] = array(
              '#theme' => 'update_version',
              '#version' => $security_update,
              '#tag' => t('Security update:'),
              '#class' => $security_class,
            );
          }
        }
      }

      if ($project['recommended'] !== $project['latest_version']) {
        $versions_inner[] = array(
          '#theme' => 'update_version',
          '#version' => $project['releases'][$project['latest_version']],
          '#tag' => t('Latest version:'),
          '#class' => array('version-latest'),
        );
      }
      if ($project['install_type'] == 'dev'
          && $project['status'] != UPDATE_CURRENT
          && isset($project['dev_version'])
          && $project['recommended'] !== $project['dev_version']) {
        $versions_inner[] = array(
          '#theme' => 'update_version',
          '#version' => $project['releases'][$project['dev_version']],
          '#tag' => t('Development version:'),
          '#class' => array('version-latest'),
        );
      }
    }

    if (isset($project['also'])) {
      foreach ($project['also'] as $also) {
        $versions_inner[] = array(
          '#theme' => 'update_version',
          '#version' => $project['releases'][$also],
          '#tag' => t('Also available:'),
          '#class' => array('version-also-available'),
        );
      }
    }

    if (!empty($versions_inner)) {
      $row['versions'] = $versions_inner;
    }

    $row['extra'] = array();
    if (!empty($project['extra'])) {
      foreach ($project['extra'] as $key => $value) {
        $extra_item = array();
        $extra_item['attributes'] = new Attribute(array('class' => $value['class']));
        $extra_item['label'] = check_plain($value['label']);
        $extra_item['data'] = drupal_placeholder($value['data']);
        $row['extra'][$key] = $extra_item;
      }
    }

    // Make sure we start with a clean slate for each project in the report.
    $row['disabled'] = array();
    $row['includes'] = array();
    if (!empty($project['disabled'])) {
      sort($project['disabled']);
      $row['disabled'] = $project['disabled'];
    }

    sort($project['includes']);
    $row['includes'] = $project['includes'];

    if (!empty($project['base_themes'])) {
      asort($project['base_themes']);
      $base_themes = array();
      foreach ($project['base_themes'] as $base_key => $base_theme) {
        switch ($status[$base_key]) {
          case UPDATE_NOT_SECURE:
          case UPDATE_REVOKED:
          case UPDATE_NOT_SUPPORTED:
            $status_label = array(
              '#theme' => 'update_status_label',
              '#status' => $status[$base_key],
            );
            $base_themes[] = t('%base_theme (!base_label)', array(
              '%base_theme' => $base_theme,
              '!base_label' => drupal_render($status_label),
            ));
            break;

          default:
            $base_themes[] = drupal_placeholder($base_theme);
        }
      }
      $row['base_themes'] = $base_themes;
    }

    if (!empty($project['sub_themes'])) {
      sort($project['sub_themes']);
      $row['sub_themes'] = $project['sub_themes'];
    }

    // Build project rows.
    if (!isset($rows[$project['project_type']])) {
      $rows[$project['project_type']] = array();
    }
    $row_key = !empty($project['title']) ? drupal_strtolower($project['title']) : drupal_strtolower($project['name']);
    $rows[$project['project_type']][$row_key] = $row;
  }

  $project_types = array(
    'core' => t('Drupal core'),
    'module' => t('Modules'),
    'theme' => t('Themes'),
    'module-disabled' => t('Disabled modules'),
    'theme-disabled' => t('Disabled themes'),
  );
  $variables['project_types'] = array();
  foreach ($project_types as $type_name => $type_label) {
    if (!empty($rows[$type_name])) {
      ksort($rows[$type_name]);
      $variables['project_types'][] = array(
        'label' => $type_label,
        'projects' => $rows[$type_name],
      );
    }
  }
  drupal_add_css(drupal_get_path('module', 'update') . '/update.css');
}

/**
 * Returns HTML for a label to display for a project's update status.
 *
 * @param array $variables
 *   An associative array containing:
 *   - status: The integer code for a project's current update status.
 *
 * @see update_calculate_project_data()
 */
function template_preprocess_update_status_label(&$variables) {
  $variables['attributes'] = new Attribute(array('class' => array()));
  switch ($variables['status']) {
    case UPDATE_NOT_SECURE:
      $variables['attributes']['class'] = 'security-error';
      $variables['text'] = 'Security update required!';
      break;
    case UPDATE_REVOKED:
      $variables['attributes']['class'] = 'revoked';
      $variables['text'] = 'Revoked!';
      break;
    case UPDATE_NOT_SUPPORTED:
      $variables['attributes']['class'] = 'not-supported';
      $variables['text'] = 'Not supported!';
      break;
    case UPDATE_NOT_CURRENT:
      $variables['attributes']['class'] = 'not-current';
      $variables['text'] = 'Update available';
      break;
    case UPDATE_CURRENT:
      $variables['attributes']['class'] = 'current';
      $variables['text'] = 'Up to date';
      break;
  }
}

/**
 * Prepare variables for the version display of a project.
 *
 * Default template: update-version.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - version: An array of data about the latest released version, containing:
 *     - version: The version number.
 *     - release_link: The URL for the release notes.
 *     - date: The date of the release.
 *     - download_link: The URL for the downloadable file.
 *   - tag: The title of the project.
 *   - class: A array containing extra classes for the wrapping table.
 */
function template_preprocess_update_version(&$variables) {
  $version = $variables['version'];

  // Remove 'update_version' from the attributes array. This is added through
  // template_preprocess() but we do not need it.
  // @todo Remove after http://drupal.org/node/1938430 is resolved.
  $attributes = array();
  $attributes['class'] = $variables['class'];
  $attributes['class'][] = 'version';
  $variables['attributes'] = new Attribute($attributes);

  $variables['version_link'] = l($version['version'], $version['release_link']);
  $variables['version_date'] = format_date($version['date'], 'custom', 'Y-M-d');

  $links = array();
  $links['download'] = array(
    'title' => t('Download'),
    'href' => $version['download_link'],
  );
  $links['release-notes'] = array(
    'title' => t('Release notes'),
    'href' => $version['release_link'],
  );
  $variables['version_links'] = array(
    '#theme' => 'links__update_version',
    '#links' => $links,
  );
}
