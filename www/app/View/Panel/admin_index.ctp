<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<h3><?php echo Configure::read('Website.name'); ?> Configuration</h3>

<?php
$this->end();
?>
