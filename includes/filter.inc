<?php

function islandora_memberofsite_islandora_object_access($op, $object, $user)
{
    $sites = variable_get('islandora_memberofsite_sites');
    $sites_arr = explode("\r\n", $sites);
    $obj_sites = $object->relationships->get(NULL, 'isMemberOfSite');
    if ($obj_sites->count() == 0) {
        return null;
    }
    foreach ($sites_arr as $site_pid_and_url)
    {
        $site_pid = explode("|", $site_pid_and_url)[0];
        foreach ($obj_sites as $obj_site) {
            if ($obj_site['object']['value'] == $site_pid) {
                return true;
            }
        }
    }
    return false;
}