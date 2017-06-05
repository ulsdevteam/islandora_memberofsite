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
function islandora_memberofsite_admin_form() {
  $form = array(
    'namespace' => array(
      '#type' => 'textfield',
      '#title' => t('Namespace'),
      '#description' => t('Value to use for xmlns attributes on IsMemberOfSite associations.'),
      '#default_value' => variable_get('islandora_memberofsite_namespace', 'http://digital.library.pitt.edu/ontology/relations#'),
      '#size' => 100,
    ),
    'sites' => array(
      '#type' => 'textarea',
      '#title' => t('Sites'),
      '#description' => t('Value to use for the rdf:resource attribute on IsMemberOfSite associations.  List possible site values (must match the "!link" value from those sites\' configurations.)',
        array('!link' => l(t('Solr base filter'), '/admin/islandora/search/islandora_solr/settings'))),
      '#default_value' => variable_get('islandora_memberofsite_sites'),
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
      variable_set('islandora_memberofsite_sites', $form_state['values']['sites']);
      break;

    case 'edit-reset':
      variable_del('islandora_memberofsite_namespace');
      variable_del('islandora_memberofsite_sites');
      break;
  }
}
