<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<script type="text/javascript">
$(document).ready(function(){
    $("#update_tles").click(function(){
	// Disable the button
	$("#update_tles").attr('disabled', 'disabled');
	$("#update_tles").addClass('disabled');

	// Show the spinner
	$("#update_status_container").hide();
	$("#spinner_container").css('visibility', 'visible');

	// Attempt to update the TLE's
	$.get('<?php echo $this->Html->url(array('controller' => 'source', 'action' => 'admin_sourceupdate')); ?>', function(data) {
	    // Regardless of what happenes, just refresh the page. Any errors will get reported below.
	    $("#update_tles").removeAttr('disabled');
	    $("#update_tles").removeClass('disabled');
	    $("#spinner_container").css('visibility', 'hidden');

	    window.location.reload()
	});
    });
});
</script>
<h3>TLE Source Status</h3>
<p>To refresh individual sources, visit the <a href="<?php echo $this->Html->url(array('controller' => 'source', 'action' => 'admin_index')); ?>" class="link">source management page</a>.</p>
<button name="update_tles" id="update_tles" class="btn btn-primary">Manually Refresh TLE's</button>
<div id="spinner_container" style="display: inline; visibility: hidden;position: relative; top: 5px;"><?php echo $this->Html->image('ajax_spinner_small.gif'); ?></div>
<br /><br />
<h4>Source Update Alerts</h4>
<?php if (empty($error_updates)): ?>
    <p>No recent errors have occured while updating TLE sources.</p>
    <br /><br />
<?php else: ?>
    <p>A few errors have occured recently while trying to update TLE sources. They are listed below.</p>
    <table class="table" width="100%">
	<thead>
	    <tr>
		<th width="30%">Error Date</th>
		<th width="31%">Source</th>
		<th width="38%">Error Message</th>
	    </tr>
	</thead>
	<tbody>
	    <?php foreach($error_updates as $error_update): ?>
		<tr>
		    <td width="30%">
                        <?php
                        $source_polled = strtotime($error_update['Update']['created_on']);
                        echo date("m/j/Y H:i:s", $source_polled);
                        ?>
                    </td>
		    <td width="31%"><a href="<?php echo $this->webroot; ?>admin/source/<?php echo $error_update['Source']['id']; ?>/edit" target="_blank" class="link"><?php echo $error_update['Source']['name']; ?></a></td>
		    <td width="38%"><?php echo $error_update['Update']['update_message']; ?></td>
		</tr>
	    <?php endforeach; ?>
	</tbody>
    </table>
<?php endif; ?>
<?php
$this->end();
?>
