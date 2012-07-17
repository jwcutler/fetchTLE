<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<h3>TLE Source Management</h3>
<?php if(empty($sources)): ?>
	No TLE sources are currently defined. Try adding some.
<?php else: ?>

<?php endif; ?>
<div style="padding-top: 5px;">
	<a href="/admin/source/add" class="btn btn-primary">Add New TLE Source</a>
</div>
<?php
$this->end();
?>

