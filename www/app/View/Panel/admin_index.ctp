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
		$.get('/admin/source/sourceupdate', function(data) {
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
<button name="update_tles" id="update_tles" class="btn btn-primary">Manually Update TLE's</button>
<div id="spinner_container" style="display: inline; visibility: hidden;position: relative; top: 5px;"><img src='/img/ajax_spinner_small.gif' alt='Currently Loading TLEs' /></div>
<br /><br />
<h4>Source Update Alerts</h4>
<?php if (empty($error_sources)): ?>
    No errors have occured recently while trying to update TLE sources.
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
			<?php foreach($error_sources as $error_source): ?>
				<tr>
					<td width="30%">
                        <?php
                        $source_polled = strtotime($error_source['Update'][count($error_source['Update'])-1]['created_on']);
                        echo date("m/j/Y H:i:s", $source_polled);
                        ?>
                    </td>
					<td width="31%"><a href="/admin/source/<?php echo $error_source['Source']['id']; ?>/edit" target="_blank" class="link"><?php echo $error_source['Source']['name']; ?></a></td>
					<td width="38%"><?php echo $error_source['Update'][count($error_source['Update'])-1]['update_message']; ?></td>
				</tr>
			<?php endforeach; ?>
		</tbody>
	</table>
<?php endif; ?>
<h3>API Status</h3>
<?php
$this->end();
?>
