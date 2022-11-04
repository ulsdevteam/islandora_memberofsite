<?php

function islandora_memberofsite_islandora_object_access($op, $object, $user)
{
    $sites = explode("\r\n", variable_get('islandora_memberofsite_sites'));
    $obj_sites = $object->relationships->get(NULL, 'isMemberOfSite');
    foreach ($sites as $site_pid) {
        foreach ($obj_sites as $obj_site) {
            if ($obj_site['object']['value'] == $site_pid) {
                return true;
            }
        }
    }
    return false;
}