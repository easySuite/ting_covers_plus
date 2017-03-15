<?php
/**
 * @file
 * Administration interface for Ting covers plus, which allows configuration of
 * default covers for ting objects
 */

/**
 * Admin settings form for Ting covers plus.
 */
function ting_covers_plus_admin_default_covers_form($form, &$form_state) {
  $form = array();

  $types = variable_get('ting_well_types', array());
  $sources = variable_get('ting_well_sources', array());
  $settings = variable_get('ting_default_covers', array());

  // Get additional info on allfiles part of the array, for labels and URLs
  $allfiles_data = array();
  if (isset($settings['allfiles']) && !empty($settings['allfiles'])) {
    $allfiles_result = db_select('file_managed', 'f')
      ->fields('f',array('fid', 'filename', 'uri'))
      ->condition('fid', $settings['allfiles'], 'IN')
      ->execute();
    while($record = $allfiles_result->fetchAssoc()) {
      $allfiles_data[] = array(
        'fid' => $record['fid'],
        'filename' => $record['filename'],
        'uri' => $record['uri'],
      );
    }
  }

  $form['ting_default_covers_remove'] = array(
    '#type' => 'fieldset',
    '#title' => t('Hide empty covers'),
    '#description' => t('Remove empty covers or insert noimage.'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['ting_default_covers_remove']['ting_covers_plus_hide_covers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide empty covers'),
    '#default_value' => variable_get('ting_covers_plus_hide_covers', FALSE),
  );

  // Add buttons to update well types and sources
  $form['update'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update from datawell'),
    '#description' => t('If needed, well types and well sources can be updated/refreshed here. This helps when evident that the settings below lacks settings for some types and/or sources.'),
  );

  $form['update']['types'] = array(
    '#type' => 'submit',
    '#value' => t('Update well types'),
    '#submit' => array('_ting_covers_plus_update_types'),
  );

  $form['update']['sources'] = array(
    '#type' => 'submit',
    '#value' => t('Update well sources'),
    '#submit' => array('_ting_covers_plus_update_sources'),
  );

  $form['ting_default_covers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default covers'),
    '#description' => t('Settings for the default covers. A default cover will be set, first according to well SOURCE of the record, thereafter according to well MATERIAL TYPE for the record, and if none applies, the default cover (fallback) will be used.'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Use the #managed_file FAPI element to upload an image file.
  // See http://www.cleancode.co.nz/blog/1096/multiple-file-upload-managedfile-drupal
  $form['ting_default_covers']['allfiles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Uploaded covers'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $filecount = 0;

  // load existing list
  if (isset($settings['allfiles'])) {
    foreach($settings['allfiles'] as $key => $value) {
      // Use the #managed_file FAPI element to upload an image file.
      $form['ting_default_covers']['allfiles']['file_'.$filecount] = array(
        '#title' => t('Cover ' . $filecount . ' (' . $value . ')'),
        '#type' => 'managed_file',
        '#upload_location' => 'public://ting_covers_defaults/',
        '#default_value' => $value,
        '#upload_validators' => array(
          'file_validate_extensions' => array('gif png jpg jpeg'),
          // Pass the maximum file size in bytes
          'file_validate_size' => array(1000000),
        ),
      );

      $filecount++;
    }
  }

  // always attach a new upload control
  // Use the #managed_file FAPI element to upload an image file.
  $form['ting_default_covers']['allfiles']['file_'.$filecount] = array(
    '#title' => t('Cover image ' . $filecount),
    '#type' => 'managed_file',
    '#upload_location' => 'public://ting_covers_defaults/',
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
      // Pass the maximum file size in bytes
      'file_validate_size' => array(1000000),
    ),
  );

  // Add a preview of the uploaded covers that are now available for settings
  $covers_markup = '';
  if (isset($allfiles_data)) {
    foreach ($allfiles_data as $record) {
      $covers_markup .= '<li><img alt="File: ' . $record['filename'] . '" title="' . $record['filename'] . '" src="' . file_create_url($record['uri']) .'"/><span>' . $record['filename'] . '</span></li>';
    }
  }

  $form['ting_default_covers']['covers_display'] = array(
    '#type' => 'item',
    '#title' => t('Covers available for setting defaults'),
    '#markup' => '<div>' . $covers_markup . '</div>',
  );

  // Create an options array
  $cover_options = array('' => t('Choose'));
  if (isset($allfiles_data)) {
    foreach($allfiles_data as $record) {
      $cover_options[$record['fid']] = $record['filename'];
    }
  }

  // Settings for sources
  if (count($sources)) {
    $form['ting_default_covers']['sources'] = array(
      '#type' => 'fieldset',
      '#title' => t('Source specific default covers'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t("Here you may set default covers for sources. If a SOURCE default cover is not set, a default cover will then be chosen according to MATERIAL TYPE specific covers below."),
    );

    foreach ($sources as $source => $scount) {
      $form['ting_default_covers']['sources'][$source] = array(
        '#type' => 'select',
        '#title' => $source,
        '#default_value' => isset($settings['sources'][$source]) ? $settings['sources'][$source] : '',
        '#options' => (isset($settings['allfiles'])) ? $cover_options : array(),
        '#description' => t('Count: @count', array('@count' => $scount)),
      );
    }
  }

  // Setting for types
  if (count($types)) {
    $form['ting_default_covers']['types'] = array(
      '#type' => 'fieldset',
      '#title' => t('Type specific default covers'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t("Here you may override the default cover for individual material types."),
    );

    foreach ($types as $term => $count) {
      $form['ting_default_covers']['types'][$term] = array(
        '#type' => 'select',
        '#title' => $term,
        '#default_value' => isset($settings['types'][$term]) ? $settings['types'][$term] : '',
        '#options' => (isset($settings['allfiles'])) ? $cover_options : array(),
        '#description' => t('Count: @count', array('@count' => $count)),
      );
    }
  }

  // Setting for default fallback cover
  $form['ting_default_covers']['_default'] = array(
    '#type' => 'select',
    '#title' => t('Default cover - fallback'),
    '#default_value' => isset($settings['_default']) ? $settings['_default'] : '',
    '#options' => (isset($settings['allfiles'])) ? $cover_options : array(),
    '#description' => t('Default cover used for any of the above - if setting has not been set. This is the final fallback cover to ensure everything has covers.'),
  );

  $path = drupal_get_path('module', 'ting_covers_plus');
  $form['#attached'] = array(
    'css' => array(
      'type' => 'file',
      'data' => $path . '/css/admin.css',
    ),
  );

  $form['#submit'] = array('ting_default_covers_submit');

  return system_settings_form($form);
}

/**
 * Ting covers plus admin form submission handler
 */
function ting_default_covers_submit($form, &$form_state) {
  $hide_covers = $form_state['values']['ting_default_covers_remove'];
  variable_set('ting_covers_plus_hide_covers', $hide_covers['ting_covers_plus_hide_covers']);

  $settings = variable_get('ting_default_covers', array());

  // to get the current list
  if ($fileslist = isset($settings['allfiles']) ? $settings['allfiles'] : FALSE) {

    //have to find out deleted ones
    $i = 0;
    foreach($settings['allfiles'] as $fileindex => $fileidvalue) {
      if ($form_state['values']['ting_default_covers']['allfiles']['file_'.$i] == 0) {
        // deleted
        $file =  file_load($fileidvalue) ;
        if ($file) {
          // When a module is managing a file, it must manage the usage count.
          // Here we decrement the usage count with file_usage_delete().
          file_usage_delete($file, 'ting_covers_plus', 'sample_image', 1);

          // The file_delete() function takes a file object and checks to see if
          // the file is being used by any other modules. If it is the delete
          // operation is cancelled, otherwise the file is deleted.
          file_delete($file);
        }
      }
      $i++;
    }
  }

  // now save files
  $newlist = array();
  $newlist_count = 0;

  foreach ($form_state['values']['ting_default_covers']['allfiles'] as $key => $value) {
    if ($value != 0) {
      // file exists
      $file = file_load($value);
      if ($file->status == 0) {
        $file->status = FILE_STATUS_PERMANENT;
        file_save($file);

        // When a module is managing a file, it must manage the usage count.
        // Here we increment the usage count with file_usage_add().
        file_usage_add($file, 'ting_covers_plus', 'sample_image', 1);
      }

      $newlist['file_'.$newlist_count] = $file->fid;
      $newlist_count++;
    }
    // Remove empty values from form_state
    else {
      unset($form_state['values']['ting_default_covers']['allfiles'][$key]);
    }
  }

  if($newlist_count > 0) {
    $settings['allfiles'] = $newlist;
  }
  else {
    $settings['allfiles'] = FALSE;
  }

  variable_set('ting_default_covers', $settings);

  // Get additional info on allfiles part of the array, for labels and URLs
  if ($settings['allfiles']) {
    $fid_uri_map = array();
    $map_result = db_select('file_managed', 'f')
      ->fields('f',array('fid', 'uri'))
      ->condition('fid', $settings['allfiles'], 'IN')
      ->execute();
    while($row = $map_result->fetchAssoc()) {
      $fid_uri_map[$row['fid']] = $row['uri'];
    }
  }

  if (isset($fid_uri_map) && !empty($fid_uri_map)) {
    // create a copy of types array with the keys transformed to a css-friendly string
    $mtype_uris = array('_default' => $fid_uri_map[$form_state['values']['ting_default_covers']['_default']] );
    if ($form_state['values']['ting_default_covers']['types']) {
      foreach($form_state['values']['ting_default_covers']['types'] as $key => $value) {
        $mtype_uris[drupal_clean_css_identifier($key)] = (isset($fid_uri_map[$value]) ? $fid_uri_map[$value] : NULL);
      }
    }
    variable_set('ting_default_covers_mtype_uris', $mtype_uris);

    // create a copy of sources array with the keys transformed to a css-friendly string
    reset($fid_uri_map);
    $source_uris = array('_default' => $fid_uri_map[$form_state['values']['ting_default_covers']['_default']] );
    if ($form_state['values']['ting_default_covers']['sources']) {
      foreach($form_state['values']['ting_default_covers']['sources'] as $key => $value) {
        $source_uris[drupal_clean_css_identifier($key)] = (isset($fid_uri_map[$value]) ? $fid_uri_map[$value] : NULL);
      }
    }
    variable_set('ting_default_covers_source_uris', $source_uris);
  }

  $form_state['redirect'] = 'admin/config/ting/covers/defaults';
}

/**
 * Submit handler. Updates the list of known well types.
 */
function _ting_covers_plus_update_types($form, &$form_state) {
  _ting_fetch_well_types();
}

/**
 * Submit handler. Updates the list of known well sources.
 */
function _ting_covers_plus_update_sources($form, &$form_state) {
  _ting_fetch_well_sources();
}
