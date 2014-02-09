<?php
/**
 * @file
 * Administration interface for Ting covers plus, which allows configuration of
 * XXX
 */

/**
 * Admin settings form for Ting covers plus.
 */
function ting_covers_plus_admin_default_covers_form($form, &$form_state) {
  $form = array();

  $types = variable_get('ting_well_types', array());
  $settings = variable_get('ting_default_covers', array());

  $form['ting_default_covers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default covers'),
    '#description' => t('Settings for the default cover and default cover per well type'),
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

  $form['ting_default_covers']['_default'] = array(
    '#type' => 'select',
    '#title' => t('Default cover'),
    '#default_value' => isset($settings['_default']) ? $settings['_default'] : '',
    '#options' => (isset($settings['allfiles'])) ? _ting_cover_options($settings['allfiles']) : array(),
    '#description' => t('Default cover used for types that is not specifically set below.'),
  );

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
        '#options' => (isset($settings['allfiles'])) ? _ting_cover_options($settings['allfiles']) : array(),
        '#description' => t('Count: @count', array('@count' => $count)),
        // Fudge the parent so we'll get all the labels in one big array in
        // ting_default_covers.
        '#parents' => array('ting_default_covers', $term),
      );
    }
  }

  $form['#submit'] = array('ting_default_covers_submit');

  return system_settings_form($form);
}

/**
 * Ting covers plus admin form submission handler
 */
function ting_default_covers_submit($form, &$form_state) {

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
  }

  if($newlist_count > 0) {
    $settings['allfiles'] = $newlist;
  }
  else {
    $settings['allfiles'] = FALSE;
  }

  variable_set('ting_default_covers', $settings);

  $form_state['redirect'] = 'admin/config/ting/covers/defaults';
}

/**
 * Helper function to return cover image options
 */
function _ting_cover_options($allfiles) {
  $options = array('' => t('Choose'));

  if (isset($allfiles)) {
    $result = db_select('file_managed', 'f')
      ->fields('f',array('fid', 'filename'))
      ->condition('fid', $allfiles, 'IN')
      ->execute();

    while($record = $result->fetchAssoc()) {
      $options[$record['fid']] = $record['filename'];
    }
  }

  return $options;
}