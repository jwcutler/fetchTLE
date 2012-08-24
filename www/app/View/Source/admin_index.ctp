<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<script type="text/javascript">
$(document).ready(function(){
	$("[id^=refresh_]").click(function(){
		// Get the ID
		source_id = $(this).attr('rel');
		source_name = $(this).attr('title');
		
		// Disable the button
		$(this).attr('disabled', 'disabled');
		$(this).addClass('disabled');

		// Show the spinner, hide the buttons
		$("#spinner_container_"+source_id).show();
		$("#buttons_container_"+source_id).hide();

		// Attempt to update the TLE's
		$.get('<?php echo $this->webroot; ?>admin/source/sourceupdate/'+source_name, function(data) {
			// Regardless of what happenes, just refresh the page. Any errors will get reported by the flash.
			$(this).removeAttr('disabled');
			$(this).removeClass('disabled');
			$("#spinner_container_"+source_id).hide();
			$("#buttons_container_"+source_id).show();

			window.location.reload()
		});
    });
});
</script>
<h3>TLE Source Management</h3>
<?php if(empty($sources)): ?>
	No TLE sources are currently defined. Try adding some.
<?php else: ?>
	All currently configured sources are displayed below.
	<table class="table" width="100%">
		<thead>
			<tr>
				<th width="20%">Name</th>
				<th width="27%">Description</th>
				<th width="30%">Last Polled</th>
				<th width="23%">Actions</th>
			</tr>
		</thead>
		<tbody>
			<?php foreach($sources as $source): ?>
				<tr>
					<td width="20%"><a href="<?php echo $source['Source']['url']; ?>" target="_blank" class="link"><?php echo $source['Source']['name']; ?></a></td>
					<td width="27%"><?php echo $source['Source']['description']; ?></td>
					<td width="30%">
						<?php if (!empty($source['Update'])): ?>
							<?php
							$source_polled = $source['Update'][count($source['Update'])-1]['created_on'];
							echo date("m/j/Y H:i:s T", $source_polled);
							?>
						<?php else: ?>
							Not Polled Yet
						<?php endif; ?>
					</td>
					<td width="23%">
						<div id="spinner_container_<?php echo $source['Source']['id']; ?>" style="display: none; position: relative; top: 5px;"><center><?php echo $this->Html->image('ajax_spinner_small.gif'); ?></center></div>
						<div id="buttons_container_<?php echo $source['Source']['id']; ?>">
							<a id="refresh_<?php echo $source['Source']['id']; ?>" rel="<?php echo $source['Source']['id']; ?>" title="<?php echo $source['Source']['name']; ?>" class="btn btn-mini btn-primary" style="cursor: pointer;">Refresh</a> <a href="<?php echo $this->webroot; ?>admin/source/<?php echo $source['Source']['id']; ?>/edit" class="btn btn-mini btn-primary">Manage</a>
						</div>
					</td>
				</tr>
			<?php endforeach; ?>
		</tbody>
	</table>
<?php endif; ?>
<div style="padding-top: 5px;">
	<a href="<?php echo $this->Html->url(array('controller' => 'source', 'action' => 'add', 'admin' => true)); ?>" class="btn btn-primary">Add New TLE Source</a>
</div>
<?php
$this->end();
?>

