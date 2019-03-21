<?php


/**
 * To determine if the object is a collection.
 *
 */
function islandora_memberofsite_is_collection_object($object) {
  $is_it = FALSE;
  foreach ($object->models as $model) {
    $is_it |= (strstr($model, 'islandora:collectionCModel') <> '');
  }
  return $is_it;
}

/**
 * This will only add the relationship if it does not already exist.
 */
function islandora_memberofsite_add_relationship_if_not_exists($object, $relationship, $value, $namespace) {
  // get the current relationships
  $rels = $object->relationships->get($namespace, $relationship);
  $existed = FALSE;
  foreach ($rels as $rel) {
    $existed |= (isset($rel['object']['value']) && $rel['object']['value'] == $value);
  }
  if (!$existed) {
    $object->relationships->add($namespace, $relationship, $value);
  }
}

/**
 * This will use the value in $site_rdf to find the matching site url from the
 */
function islandora_memberofsite_get_site_url($site_rdf) {
  $site_url = '';
  $rdf = (isset($site_rdf['object']) && isset($site_rdf['object']['value'])) ? $site_rdf['object']['value'] : NULL;
  if ($rdf) {
    $sites = variable_get('islandora_memberofsite_sites');
    $sites_arr = explode("\r\n", $sites);
    foreach ($sites_arr as $site_rdf_and_url) {
      if (!$site_url) {
        if (strstr($site_rdf_and_url, $rdf)) {
          @list($site, $site_url) = explode("|", $site_rdf_and_url);
        }
      }
    }
  }
  return $site_url;
}

/**
 * This will check for duplicate isMemberOfSite relationships that were added to many objects -- 
   merely a harmless cleanup on object/manage view.
 */
function islandora_memberofsite_fix_relationships($islandora_object, $relationships) {
  $duplicates = array();
  $sites = array();

  foreach ($relationships as $relationship_arr) {
    if ($relationship_arr['predicate']['value'] == 'isMemberOfSite' && $relationship_arr['object']['value']) {
      if (array_search($relationship_arr['object']['value'], $sites) === FALSE) {
        $sites[] = $relationship_arr['object']['value'];
      }
      else {
        if (array_search($relationship_arr['object']['value'], $duplicates) === FALSE) {
          $duplicates[] = $relationship_arr['object']['value'];
        }
      }
    }
  }

  if (count($duplicates) > 0) {
    $ns = 'http://digital.library.pitt.edu/ontology/relations#';
    // Remove and then add the duplicate site relationship so that there will be only one
    // of these relationships
    foreach ($duplicates as $duplicate) {
      // Remove the relationship that matches the string value for $ns / $site.
      if ($islandora_object->relationships->remove($ns, 'isMemberOfSite', $duplicate)) {
        drupal_set_message(t('Duplicate "isMemberOfSite" relationship(s) have been removed for ' .
          '"@site" in the "@ns" resource.', array('@site' => $site, '@ns' => $ns)));
      }
      islandora_memberofsite_add_relationship_if_not_exists($islandora_object, 'isMemberOfSite', $duplicate, $ns);
    }
  }
}
