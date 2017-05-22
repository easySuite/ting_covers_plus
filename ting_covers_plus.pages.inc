<?php
/**
 * @file
 * Retrieves covers for ting objects and returns them in the correct format
 * based on configuration.
 */

// Load ting_covers.pages.inc for its helper functions
module_load_include('inc', 'ting_covers', 'ting_covers.pages');

/**
 * Return the URL for the cover of an object.
 */
function ting_covers_plus_objects() {
  $ids = array();
  $styles = array();
  foreach ($_POST['coverData'] as $cover_info) {
    $ids[] = $cover_info['local_id'];
    $styles[$cover_info['local_id']] = $cover_info['image_style'];
  }

  $covers = ting_covers_get($ids);

  $return = array();
  foreach ($ids as $id) {
    $source_url = NULL;
    $default_image = _ting_covers_plus_get_default_image($id, '_default');
    $default_image_url = image_style_url($styles[$id], $default_image);
    if (!empty($covers[$id])) {
      $source_url = $covers[$id];
    }
    else {
      $source_url = $default_image;
    }

    $image_url = '';
    if (file_exists($source_url)) {
      $image_url = image_style_url($styles[$id], $source_url);
    }

    // Generate a path corresponding to the default image, styled.
    $return[] = array(
      'local_id' => $id,
      'image_style' => $styles[$id],
      'url' => $image_url,
      'class' => ($default_image_url == image_style_url($styles[$id], $source_url)) ? 'ting-covers-plus-default' : '',
    );
  }
  // Return all image information.
  drupal_json_output($return);
}

/**
 * Helper function returning default image URI for a given material type
 *
 * @param string $mtype
 *    The material type label to get standard image for.
 * @return string
 *    An URI to an image file.
 */
function _ting_covers_plus_get_default_image($mtype, $source) {
  $source_url = NULL;
  $mtype_uris = variable_get('ting_default_covers_mtype_uris');
  $source_uris = variable_get('ting_default_covers_source_uris');

  // Check first if we have default cover defined for SOURCE NAME
  if (isset($source_uris[$source])) {
    $source_url = $source_uris[$source];
  }
  // Then check if we have default cover defined for MATERIAL TYPE
  elseif (isset($mtype_uris[$mtype])) {
    $source_url = $mtype_uris[$mtype];
  }
  // Last, use the default fallback cover if it has been defined
  elseif (isset($mtype_uris['_default'])) {
    $source_url = $mtype_uris['_default'];
  }

  return $source_url;
}
