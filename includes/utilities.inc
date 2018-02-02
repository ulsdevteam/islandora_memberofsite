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

