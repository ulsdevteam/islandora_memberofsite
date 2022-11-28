<?php

/**
 * @file
 * Administration forms and submit handling for isMemberOfSite module.
 */

/**
 * Administration and configuration form for memberofsite.
 *
 * @ingroup forms
 *
 * @todo break out non-settings elements into their own form.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_memberofsite_admin_form($form, &$form_state) {
  module_load_include('inc', 'islandora_memberofsite', 'includes/utilities');
  $all_sites = islandora_memberofsite_get_sites($form_state['values']['site_model']);
  $selected_sites = explode("\r\n", variable_get('islandora_memberofsite_sites'));
  $default_values = array();
  foreach ($all_sites as $pid => $site) {
    $default_values[$pid] = in_array($pid, $selected_sites);
  }
  $form = array(
    'namespace' => array(
      '#type' => 'textfield',
      '#title' => t('Namespace'),
      '#description' => t('Value to use for xmlns attributes on IsMemberOfSite associations.'),
      '#default_value' => variable_get('islandora_memberofsite_namespace', 'http://digital.library.pitt.edu/ontology/relations#'),
      '#size' => 100,
    ),
    'site_model' => array(
      '#type' => 'textfield',
      '#title' => t('Site Content Model'),
      '#description' => t('PID of the CModel representing a site.'),
      '#default_value' => variable_get('islandora_memberofsite_site_model'),
      '#size' => 100,
      '#ajax' => array(
        'wrapper' => 'memberofsite-sites',
        'callback' => 'islandora_memberofsite_sites_div',
      )
    ),
    'sites_desc' => array(
      '#type' => 'item',
      '#title' => t('Site PIDs'),
      '#description' => t('The <b>Site PID value</b> is used for the rdf:resource attribute on IsMemberOfSite associations. ' .
        'When sites are selected here, objects will only be accessible if they are a member of one of the selected sites.'),
    ),
    'sites' => array(
      '#prefix' => '<div id="memberofsite-sites">',
      '#suffix' => '</div>',
      '#type' => 'tableselect',
      '#header' => array('pid' => t('PID'), 'label' => t('Name')),
      '#options' => $all_sites,
      '#default_value' => $default_values,
      '#empty' => t('Enter a site CModel above.')
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    ),
    'reset' => array(
      '#type' => 'submit',
      '#value' => t('Reset to defaults'),
    ),
  );
  return $form;
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_memberofsite_admin_form_submit(array $form, array &$form_state) {
  drupal_set_message(t('The settings have been updated!'));
  $id = $form_state['triggering_element']['#id'];
  switch ($id) {
    case 'edit-submit':
      variable_set('islandora_memberofsite_namespace', $form_state['values']['namespace']);
      variable_set('islandora_memberofsite_site_model', $form_state['values']['site_model']);
      variable_set('islandora_memberofsite_sites', implode("\r\n", array_filter($form_state['values']['sites'])));
      break;

    case 'edit-reset':
      variable_del('islandora_memberofsite_namespace');
      variable_del('islandora_memberofsite_site_model');
      variable_del('islandora_memberofsite_sites');
      break;
  }
}

/**
 * Get the element to render on an ajax event
 */
function islandora_memberofsite_sites_div($form, $form_state) {
  return $form['sites'];
}
