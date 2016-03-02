<?php

/**
 * @file
 * Theme implementation for table.
 */

$first_add = TRUE;

?>
<table id="values" class="sticky-enabled">
  <thead>
    <tr>
      <th><?php print t('Label'); ?></th>
      <th><?php print t('URL'); ?></th>
      <th><?php print t('XSD URL'); ?></th>
    </tr>
  </thead>
  <tbody>

<?php foreach (element_children($form) as $key => $element):
      $data = $form[$element];
      if (isset($data['actions']) && $first_add):
        $first_add = FALSE;
?>
  <tr>
    <th><?php print t('Label'); ?></th>
    <th><?php print t('Variable name'); ?></th>
    <th></th>
  </tr>
<?php endif; ?>
  <tr class="draggable <?php print $key % 2 == 0 ? 'odd' : 'even'; ?>">
    <td>
<?php
      print drupal_render($data['origin']);
      print drupal_render($data['label']);
?></td>
    <td>
<?php
      print drupal_render($data['url']);
?></td>
    <td>
<?php
      print drupal_render($data['xsd_url']);
      if (isset($data['actions'])) {
        print drupal_render($data['actions']['add']);
        print drupal_render($data['actions']['delete']);
      }
?></td>
      </tr>
<?php endforeach; ?>
  </tbody>
</table>
