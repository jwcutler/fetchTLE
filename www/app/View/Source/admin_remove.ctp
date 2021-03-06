<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<h3>Delete Source</h3>
Are you absolutely sure you want to delete the '<?php echo $source['Source']['name']; ?>' source? This will remove <strong>all</strong> updates and TLE information that has been collected for this source. This can not be undone.
<br /><br />
<form action="<?php echo $this->webroot; ?>admin/source/<?php echo $source['Source']['id']; ?>/delete" method="POST">
    <button type="submit" name="delete_confirmation" class="btn btn-danger">Yes, Delete Source</button>
    <a href="<?php echo $this->Html->url(array('controller' => 'source', 'action' => 'index', 'admin' => true)); ?>" class="btn btn-primary">Nevermind</a>
</form>
<?php
$this->end();
?>

