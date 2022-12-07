<?php

/**
 * @file
 * Handles the isMemberOfSite values for objects.
 */

/**
 * The add memberofsite form.
 */
function islandora_memberofsite_memberofsite_add($form, &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_memberofsite', 'includes/utilities');

  $sites = variable_get('islandora_memberofsite_sites');

  // Remove the sites that have been configured for this object already.
  $obj_sites = $object->relationships->get(NULL, 'isMemberOfSite');
  $existing_sites = $newsites = array();
  if (count($obj_sites) > 0) {
    foreach ($obj_sites as $obj_site) {
      if (isset($obj_site['object']['value'])) {
        $existing_sites[] = $obj_site['object']['value'];
      }
    }
  }
  foreach ($sites as $site) {
    if (array_search($site, $existing_sites) === FALSE) {
      $newsites[$site] = $site;
    }
  }
  if (count($newsites) < 1) {
    drupal_set_message('This object is already associated with all possible site values ' .
            'or no sites have been configured for the isMemberOfSite module. ' .
            l('Change the available Sites for adding IsMemberOfSite tags to object models',
              'admin/islandora/tools/islandora_memberofsite'));
    drupal_goto("islandora/object/{$object->id}/manage/");
    return FALSE;
  }
  $form = array(
    'site' => array(
      '#type' => 'select',
      '#title' => t('Add IsMemberOfSite'),
      '#description' => t('Select the rdf:resource attribute for the new IsMemberOfSite association.'),
      '#options' => $newsites,
    ),
    'object_id' => array(
      '#type' => 'value',
      '#value' => $object->id,
    ),
  );
  $is_collection_object = islandora_memberofsite_is_collection_object($object);
  if ($is_collection_object) {
    $form['also_add_children'] = array(
      '#type' => 'checkbox',
      '#title' => t('Also add this site relationship for all objects in this collection'),
      '#default_value' => TRUE,
    );
  }
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );

  return $form;
}

/**
 * Submit handler for islandora_memberofsite_memberofsite_add.
 */
function islandora_memberofsite_memberofsite_add_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_memberofsite', 'includes/utilities');

  $ns = variable_get('islandora_memberofsite_namespace');
  $object = islandora_object_load($form_state['values']['object_id']);

  if (is_object($object)) {
    islandora_memberofsite_add_relationship_if_not_exists($object, 'isMemberOfSite', $form_state['values']['site'], $ns);
    drupal_set_message(t('The "isMemberOfSite" relationship has been added to "@object_id".', array('@object_id' => $form_state['values']['object_id'])));

    $is_collection_object = islandora_memberofsite_is_collection_object($object);
    if ($is_collection_object && isset($form_state['values']['also_add_children']) &&
      $form_state['values']['also_add_children']) {
      module_load_include('inc', 'islandora_basic_collection', 'include/utilities');
      list($count, $results) = islandora_basic_collection_get_member_objects($object, 0, 100000, 'view');
      foreach ($results as $child_object_result) {
        $child_object = isset($child_object_result['object']['value']) ? islandora_object_load($child_object_result['object']['value']) : NULL;
        if ($child_object) {
          islandora_memberofsite_add_relationship_if_not_exists($child_object, 'isMemberOfSite', $form_state['values']['site'], $ns);
        }
      }
      drupal_set_message(t('The "isMemberOfSite" relationship has also been added for all @count child objects in this "@object_id" collection.',
        array('@count' => $count, '@object_id' => $form_state['values']['object_id'])));
    }
  }

  $form_state['redirect'] = "islandora/object/{$form_state['values']['object_id']}/manage/";
}

/**
 * The remove memberofsite handler.
 */
function islandora_memberofsite_memberofsite_remove($form, &$form_state, AbstractObject $object, $ns, $site) {
  module_load_include('inc', 'islandora_memberofsite', 'includes/utilities');

  $form = array();
  $ns = urldecode($ns);
  $site = urldecode($site);
  $form = array(
    'ns' => array(
      '#type' => 'value',
      '#value' => $ns,
    ),
    'site' => array(
      '#type' => 'value',
      '#value' => $site,
    ),
    'object_id' => array(
      '#type' => 'value',
      '#value' => $object->id,
    ),
  );

  $is_collection_object = islandora_memberofsite_is_collection_object($object);
  if ($is_collection_object) {
    $form['also_remove_children'] = array(
      '#type' => 'checkbox',
      '#title' => t('Also remove this site relationship for all objects related to this collection.'),
      '#default_value' => TRUE,
    );
  }

  return confirm_form(
    $form,
    t('Are you sure you want to delete the "isMemberOfSite" relationship from "@site" in the "@ns" resource?', array('@site' => $site, '@ns' => $ns)),
    "islandora/object/{$object->id}/manage/",
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for islandora_memberofsite_memberofsite_remove.
 */
function islandora_memberofsite_memberofsite_remove_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_memberofsite', 'includes/utilities');

  $ns = $form_state['values']['ns'];
  $site = $form_state['values']['site'];
  if (isset($form_state['values']['ns']) && isset($form_state['values']['object_id'])) {
    $object = islandora_object_load($form_state['values']['object_id']);
    // Remove the relationship that matches the string value for $ns / $site.
    if ($object->relationships->remove($ns, 'isMemberOfSite', $form_state['values']['site'])) {
      drupal_set_message(t('The "isMemberOfSite" relationship has been removed for "@site" in the "@ns" resource.', array('@site' => $site, '@ns' => $ns)));
    }
    else {
      drupal_set_message(t('There was a problem attempting to remove the "isMemberOfSite" relationship for "@site" in the "@ns" resource.', array('@site' => $site, '@ns' => $ns)));
    }
  }

  $is_collection_object = islandora_memberofsite_is_collection_object($object);
  if ($is_collection_object && isset($form_state['values']['also_remove_children']) &&
    $form_state['values']['also_remove_children']) {
    module_load_include('inc', 'islandora_basic_collection', 'include/utilities');
    list($count, $results) = islandora_basic_collection_get_member_objects($object, 0, 100000, 'view');
    $problem_pids = TRUE;
    foreach ($results as $child_object_result) {
      $child_object = isset($child_object_result['object']['value']) ? islandora_object_load($child_object_result['object']['value']) : NULL;
      if ($child_object) {
        if (!$child_object->relationships->remove($ns, 'isMemberOfSite', $form_state['values']['site'])) {
          $problem_pids[] = $child_object->id;
        }
      }
    }
    if (count($problem_pids) > 0) {
      drupal_set_message(t('There was a problem attempting to remove the "isMemberOfSite" ' .
        'relationship for "@site" in the "@ns" resource.', array('@site' => $site, '@ns' => $ns)));
    }
    else {
      drupal_set_message(t('The "isMemberOfSite" relationship "@site" in the "@ns" resource ' .
        'has also been removed for all child objects of "@object_title".',
        array('@site' => $site, '@ns' => $ns, '@object_title' => $object->label)));
    }
  }


  $form_state['redirect'] = "islandora/object/{$object->id}/manage/";
}
