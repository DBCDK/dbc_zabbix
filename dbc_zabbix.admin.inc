<?php
/**
 * @file
 * Admin related backend.
 */

/**
 * Settings form for individual item settings
 */
function dbc_zabbix_settings_form() {

  $form['#submit'] = array('dbc_zabbix_settings_form_submit');

  $form['actions'] = array(
    '#type'         => 'actions',
    'submit'        => array(
      '#type' => 'submit',
      '#value' => t('Save Zabbix URLs'),
    ),
  );

  $form['items'] = array(
    '#type'         => 'fieldset',
    '#tree'         => TRUE,
    '#theme'        => 'dbc_zabbix_settings_theme',
  );

  $index = 1;
  $client = new ting_client_class();

  foreach ( $client->getUrls() as $service ) {
    if ( $service['class'] && $service['url'] ) {
      $form['items'][$index] = array(
        '#weight' => $index,
      );
      $form['items'][$index]['origin'] = array(
        '#type' => 'hidden',
        '#value' => 'ting_client',
      );
      $form['items'][$index]['label'] = array(
        '#type' => 'markup',
        '#markup' => $service['class'],
      );
      $form['items'][$index]['url'] = array(
        '#type' => 'markup',
        '#markup' => $service['url'],
      );
      $xsd_url = NULL;
      if ( isset($service['xsd_url']) ) {
        $xsd_url = variable_get($service['xsd_url'], NULL);
      }
      $form['items'][$index]['xsd_url'] = array(
        '#type' => 'markup',
        '#markup' => $xsd_url,
      );
      $index++;
    }
    else {
      // Write to an error box
      $missing = '';
      if (!$service['class']) {
        $missing .= $missing ? ', class' : 'class';
      }
      if (!$service['url']) {
        $missing .= $missing ? ', url' : 'url';
      }
      $item = isset($service['url']) && !empty($service['url'])   ? $service['url']   : 'foo ' . $index;
      $item = isset($service['class']) && !empty($service['class']) ? $service['class'] : $item;
      drupal_set_message(t('Error: Webservice item "' . $item . '" has not been properly declared.  Missing parts: ' . $missing), 'notice', $repeat = FALSE);
    }
  }

  // Saved items
  $dbc_zabbix_settings = variable_get('dbc_zabbix_settings', array());
  foreach ( $dbc_zabbix_settings as $service ) {
    $form['items'][$index] = array(
      '#weight' => $index,
    );
    $form['items'][$index]['origin'] = array(
      '#type' => 'hidden',
      '#value' => 'variables',
    );
    $form['items'][$index]['label'] = array(
      '#type' => 'textfield', 
      '#default_value' => $service['label'], 
      '#size' => 30, 
    );
    $form['items'][$index]['url'] = array(
      '#type' => 'textfield', 
      '#default_value' => $service['url'], 
      '#size' => 50, 
    );
    $form['items'][$index]['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('-'),
      '#submit' => array('dbc_zabbix_settings_form_delete'),
    );
    $index++;
  }

  
  // Add service manually

  $form['items'][$index] = array(
    '#weight' => $index,
  );
  $form['items'][$index]['origin'] = array(
    '#type' => 'hidden',
    '#value' => 'variables',
  );
  $form['items'][$index]['label'] = array(
    '#type' => 'textfield', 
    '#default_value' => '', 
    '#size' => 30, 
  );
  $form['items'][$index]['url'] = array(
    '#type' => 'textfield', 
    '#default_value' => '', 
    '#size' => 50, 
  );
  $form['items'][$index]['xsd_url'] = array();
  $form['items'][$index]['actions']['add'] = array(
    '#type' => 'submit',
    '#value' => t('+'),
    '#submit' => array('dbc_zabbix_settings_form_add'),
  );
  $index++;

  return $form;
}

/**
 * Submit handler for webservice settings
 */
function dbc_zabbix_settings_form_validate($form, &$form_state) {

  foreach ( $form_state['values']['items'] as $key => $item ) {
    if ($item['origin'] == 'variables' && !empty($item['url'])) {
      if (empty($item['label'])) {
        form_set_error('items][' . $key . '][url',
                       t("Label is not set for \"@name\".", array('@name' => $item['url'])));
      }
      if (!variable_get($item['url'], NULL)) {
        form_set_error('items][' . $key . '][url',
                       t("No variable named \"@name\" exist.", array('@name' => $item['url'])));
      }
    }
  }
  
}


/**
 * Submit handler for webservice settings
 */
function dbc_zabbix_settings_form_add($form, &$form_state) {

  $items = variable_get('dbc_zabbix_settings', array());
  $key   = $form_state['triggering_element']['#parents'][1];
  $added = FALSE;

  $label = $form_state['values']['items'][$key]['label'];
  $url   = $form_state['values']['items'][$key]['url'];
  
  if ( !empty($label) && !empty($label) ) {
    $items[] = array(
      'label' => $label,
      'url'   => $url
    );
    $added = TRUE;
  }
  else {
  
  }

  $dbc_zabbix_settings = variable_set('dbc_zabbix_settings', $items);
  if ( $added ) {
    drupal_set_message(t('The variable has been added.'));
  }
  else {
    form_set_error('items][' . $key,
                   t("Failed to add the variable.", array()));
  }

}

/**
 * Submit handler for webservice settings
 */
function dbc_zabbix_settings_form_delete($form, &$form_state) {

  $items = variable_get('dbc_zabbix_settings', array());
  $key   = $form_state['triggering_element']['#parents'][1];
  $deleted = FALSE;

  foreach ( $items as $n => $item ) {
    if ( $item['url'] == $form_state['values']['items'][$key]['url'] &&
         $item['label'] == $form_state['values']['items'][$key]['label'] ) {
      unset($items[$n]);
      $deleted = TRUE;
      break;
    }
  }

  $dbc_zabbix_settings = variable_set('dbc_zabbix_settings', $items);
  if ( $deleted ) {
    drupal_set_message(t('The variable has been deleted.'));
  }
  else {
    form_set_error('items][' . $key,
                   t("Failed to delete the variable.", array()));
  }

}

/**
 * Submit handler for webservice settings
 */
function dbc_zabbix_settings_form_submit($form, &$form_state) {

  $items = array();

  foreach ( $form_state['values']['items'] as $key => $item ) {
    if ( !empty($item['url']) && !empty($item['label']) ) {
      $items[] = array(
        'label' => $form_state['values']['items'][$key]['label'],
        'url'   => $form_state['values']['items'][$key]['url']
      );
    }
  }
  $dbc_zabbix_settings = variable_set('dbc_zabbix_settings', $items);
  drupal_set_message(t('The Zabbix URLs has been updated.'));

}