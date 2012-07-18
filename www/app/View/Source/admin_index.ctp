<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<h3>TLE Source Management</h3>
<?php if(empty($sources)): ?>
	No TLE sources are currently defined. Try adding some.
<?php else: ?>
	All currently configured sources are displayed below.
	<table class="table" width="100%">
		<thead>
			<tr>
				<th width="20%">Name</th>
				<th width="30%">Description</th>
				<th width="30%">Last Polled</th>
				<th width="20%">Actions</th>
			</tr>
		</thead>
		<tbody>
			<?php foreach($sources as $source): ?>
				<tr>
					<td width="20%"><a href="<?php echo $source['Source']['url']; ?>" target="_blank" class="link"><?php echo $source['Source']['name']; ?></a></td>
					<td width="30%"><?php echo $source['Source']['description']; ?></td>
					<td width="30%">
						<?php if (!empty($source['Update'])): ?>
							<?php
							$source_polled = strtotime($source['Update'][count($source['Update'])-1]['created_on']);
							echo date("m/j/Y H:i:s", $source_polled);
							?>
						<?php else: ?>
							Not Polled Yet
						<?php endif; ?>
					</td>
					<td width="20%"><a href="/admin/source/<?php echo $source['Source']['id']; ?>/delete" class="btn btn-mini btn-primary">Delete</a> <a href="/admin/source/<?php echo $source['Source']['id']; ?>/edit" class="btn btn-mini btn-primary">Edit</a></td>
				</tr>
			<?php endforeach; ?>
		</tbody>
	</table>
<?php endif; ?>
<div style="padding-top: 5px;">
	<a href="/admin/source/add" class="btn btn-primary">Add New TLE Source</a>
</div>
<?php
$this->end();
?>

