<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<h3>Delete Source</h3>
Are you absolutely sure you want to delete the '<?php echo $source['Source']['name']; ?>' source?
<br /><br />
<form action="/admin/source/<?php echo $source['Source']['id']; ?>/delete" method="POST">
    <button type="submit" name="delete_confirmation" class="btn btn-danger">Yes, Delete Source</button>
    <a href="/admin/source" class="btn btn-primary">Nevermind</a>
</form>
<?php
$this->end();
?>

