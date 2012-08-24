<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<script type="text/javascript">
$(document).ready(function(){    
    // Form validation
    var container = $("div.form_errors");
    $("#edit_source_form").validate({
        errorContainer: container,
        errorLabelContainer: $("ul", container),
        errorClass: "form_error",
        wrapper: 'li',
        rules: {
            source_name: {
                required: true
            },
            source_url: {
                required: true
            }
        },
        messages: {
            source_name: {
                required: "Please enter a source name."
            },
            source_url: {
                required: "Please enter a source URL."
            }
        }
    });
	
	// Handle TLE detail expansions
	$('[id^="expand_tle_"]').click(function() {
		id = $(this).attr('title');
		
		// Toggle the box
		$("#expanded_tle_"+id).toggle();
		
		// Toggle the indicator
		curr_indicator = $(this).html();
		if (curr_indicator=="[+]"){
			$(this).html("[-]");
		} else {
			$(this).html("[+]");
		}
	});
});
</script>
<style type="text/css">
pre {
	background-color: #EEEEEE;
	padding: 0px;
	margin: 0px;
	border: none;
	font-size: 11px;
}
</style>
<h3>Edit the '<?php echo $source['Source']['name']; ?>' TLE Source</h3>
<p>Use the form below to edit the source.</p>
<div class="form_errors">
    <ul>
    </ul>
</div>
<div style="width: 70%; margin-top: 10px;">
    <form action="<?php echo $this->webroot; ?>admin/source/<?php echo $source['Source']['id']; ?>/edit" method="POST" class="form-horizontal" id="edit_source_form">
        <div class="control-group">
            <label class="control-label" for="source_name">Source Name</label>
            <div class="controls">
                <input type="text" name="source_name" id="source_name" value="<?php echo $source['Source']['name']; ?>" />
                <p class="help-block">Note that if you change this any scripts that referenced this source with the API will need to be updated.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_url">Source URL</label>
            <div class="controls">
                <input type="text" name="source_url" id="source_url" value="<?php echo $source['Source']['url']; ?>" />
                <p class="help-block">This must be a standard TLE file.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="source_description">Source Description</label>
            <div class="controls">
                <textarea name="source_description"><?php echo $source['Source']['description']; ?></textarea>
            </div>
        </div>
        <button type="submit" class="btn btn-success">Edit Source</button>
        <a href="<?php echo $this->webroot; ?>admin/source/<?php echo $source['Source']['id']; ?>/delete" class="btn btn-danger">Delete Source</a>
    </form>
</div>
<h3>Recent Updates</h3>
<?php if (empty($source['Update'])): ?>
    <p>There are no recent updates to report.</p>
    <br /><br />
<?php else: ?>
    <p>The <?php echo $update_limit; ?> most recent updates for the <?php echo $source['Source']['name']; ?> are displayed below.</p>
    <table class="table" width="60%">
	<thead>
	    <tr>
		<th width="30%">Date</th>
		<th width="70%">Update Status</th>
	    </tr>
	</thead>
	<tbody>
	    <?php foreach($source['Update'] as $source_update): ?>
		<tr>
		    <td width="30%">
			<?php
			$source_polled = $source_update['created_on'];
			echo date("m/j/Y H:i:s T", $source_polled);
			?>
			 - <?php echo $source_update['id']; ?>
		    </td>
		    <td width="70%">
			<?php
			$update_status = (empty($source_update['update_message']))?"<span style='color: green;'>Okay</span>":"<span style='color: red;'>".$source_update['update_message']."</span>";
			echo $update_status;
			?>
		    </td>
		</tr>
	    <?php endforeach; ?>
	</tbody>
    </table>
<?php endif; ?>
<h3>Most Recent TLEs</h3>
<?php if (empty($latest_tles)): ?>
    <p>No TLE's have recently been fetched for this source.</p>
<?php else: ?>
    All <?php echo count($latest_tles['Tle']); ?> TLE's from the last <i>successful</i> update (which occured on <?php $successful_update_time = $latest_tles['Update']['created_on']; echo date("m/j/Y H:i:s T", $successful_update_time); ?>) are displayed below.
    <div style='font-size: 10px;'>
	<?php foreach($latest_tles['Tle'] as $latest_tle): ?>
	    <a id="expand_tle_<?php echo $latest_tle['id']; ?>" title="<?php echo $latest_tle['id']; ?>" style="cursor: pointer; font-size: 8px;">[+]</a> <?php echo $latest_tle['name']; ?><br />
	    <div id="expanded_tle_<?php echo $latest_tle['id']; ?>" class="tle_expansion">
		<pre><?php echo $latest_tle['raw_l1']; ?></pre>
		<pre><?php echo $latest_tle['raw_l2']; ?></pre>
	    </div>
	<?php endforeach; ?>
    </div>
<?php endif; ?>
<?php
$this->end();
?>

