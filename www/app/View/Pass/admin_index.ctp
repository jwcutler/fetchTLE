<?php
$this->extend('/Common/admin_panel');

$this->start('panel_content');
?>
<?php echo $this->Html->script('chosen.jquery.min.js'); ?>
<?php echo $this->Html->css('chosen.css'); ?>
<script type="text/javascript">
$(document).ready(function(){
	// Setup the select box
    $("#available_ground_stations").chosen();
    
    // Form validation
    var container = $("div.form_errors");
    $("#pass_tool_configuration").validate({
        errorContainer: container,
        errorLabelContainer: $("ul", container),
        errorClass: "form_error",
        wrapper: 'li',
        rules: {
            "available_ground_stations[]": {
                required: true
            }
        },
        messages: {
            "available_ground_stations[]": {
                required: "Please select at least one ground station."
            }
        }
    });
});
</script>
<h3>Upcoming Pass Tool Configuration</h3>
Use this form to configure the <a href="<?php echo $this->Html->url(array('controller' => 'pass', 'action' => 'index', 'tools' => true)); ?>" target="_blank" class="link">Upcoming Pass Tool</a>.
<div class="form_errors">
    <ul>
    </ul>
</div>
<div style="width: 70%; margin-top: 10px;">
    <form method="POST" class="form-horizontal" id="pass_tool_configuration">
        <div class="control-group">
            <label class="control-label" for="config[<?php echo $passes_default_min_el['Configuration']['id']; ?>]">Default Minimum Elevation</label>
            <div class="controls">
                <input type="text" name="config[<?php echo $passes_default_min_el['Configuration']['id']; ?>]" value="<?php echo $passes_default_min_el['Configuration']['value']; ?>" />
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="available_ground_stations[]">Available Ground Stations</label>
            <div class="controls">
                <select name="available_ground_stations[]" multiple="multiple" style="width: 350px;" id="available_ground_stations">
                    <?php $active_ground_stations = explode(',', $passes_available_ground_stations['Configuration']['value']); ?>
                    <?php foreach($ground_stations as $ground_station): ?>
                        <option value="<?php echo $ground_station['Station']['id']; ?>" <?php if(is_array($active_ground_stations)&&in_array($ground_station['Station']['id'], $active_ground_stations)){echo 'selected="selected"';} ?>><?php echo $ground_station['Station']['name']; ?></option>
                    <?php endforeach; ?>
                </select>
                <input type="hidden" name="available_ground_stations_id" value="<?php echo $passes_available_ground_stations['Configuration']['id']; ?>" />
                <p class="help-block">These ground stations will be available to the upcoming pass tool.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="config[<?php echo $passes_default_pass_count['Configuration']['id']; ?>]">Default Pass Count</label>
            <div class="controls">
                <input type="text" name="config[<?php echo $passes_default_pass_count['Configuration']['id']; ?>]" value="<?php echo $passes_default_pass_count['Configuration']['value']; ?>" />
                <p class="help-block">The default number of passes to calculate.</p>
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="config[<?php echo $passes_max_pass_count['Configuration']['id']; ?>]">Max Pass Count</label>
            <div class="controls">
                <input type="text" name="config[<?php echo $passes_max_pass_count['Configuration']['id']; ?>]" value="<?php echo $passes_max_pass_count['Configuration']['value']; ?>" />
            </div>
        </div>
        <button type="submit" class="btn btn-success">Update Configuration</button>
    </form>
</div>
<?php
$this->end();
?>

