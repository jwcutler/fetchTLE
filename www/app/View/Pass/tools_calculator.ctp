<?php echo $this->Html->script('chosen.jquery.min.js'); ?>
<?php echo $this->Html->css('chosen.css'); ?>
<script type="text/javascript">
$(document).ready(function(){
    // Enable the ground station selection box
    $("#ground_stations").chosen();
    
    // Form validation
    var container = $("div.form_errors");
    jQuery.validator.setDefaults({ 
        ignore: ".hidden"
    });
    $("#calculate_form").validate({
        errorContainer: container,
        errorLabelContainer: $("ul", container),
        errorClass: "form_error",
        wrapper: 'li',
        rules: {
            minimum_elevation: {
                required: true,
                digits: true,
                min: 0,
                max: 90
            },
            number_of_passes: {
                required: true,
                digits: true,
                min: 1,
                max: <?php echo $passes_max_pass_count['Configuration']['value']; ?>
            },
            ground_stations: {
                required: true
            }
        },
        messages: {
            minimum_elevation: {
                required: "Please enter a minimum elevation.",
                digits: "The minimum elevation must be an integer.",
                min: "The minimum elevation must be at least 0 degrees.",
                max: "The minimum elevation can't be larger than 90 degrees."
            },
            number_of_passes: {
                required: "Please enter the number of passes requested.",
                digits: "The number of passes must be an integer.",
                min: "You must request at least 1 pass.",
                max: "You can't request more than <?php echo $passes_max_pass_count['Configuration']['value']; ?> passes."
            },
            ground_stations: {
                required: "You must select at least one ground station."
            }
        }
    });
});
</script>
<h1 class="titles">'<?php echo $satellite[0]['Tle']['name']; ?>' Pass Time Calculator</h1>
<?php if ($ground_stations): ?>
    <p>Use this utility to calculate the upcoming pass times for the '<?php echo $satellite[0]['Tle']['name']; ?>' satellite.</p>
    <div class="form_errors">
        <ul>
        </ul>
	</div>
    <div style="margin-top: 20px;width: 50%;">
        <form action="<?php echo Router::url('/', true); ?>tools/passes/<?php echo rawurlencode($satellite[0]['Tle']['name']); ?>" method="POST" class="form-horizontal" id="calculate_form">
            <div class="control-group">
                <label class="control-label">Satellite</label>
                <div class="controls">
                    <span class="input-xlarge uneditable-input"><?php echo $satellite[0]['Tle']['name']; ?></span>
                </div>
            </div>
            <div class="control-group">
                <label class="control-label" for="station_description">Ground Stations*</label>
                <div class="controls">
                    <select name="ground_stations" multiple="multiple" id="ground_stations" style="width: 250px;">
                        <?php foreach($ground_stations as $ground_station): ?>
                            <option value="<?php echo $ground_station['Station']['id']; ?>"><?php echo $ground_station['Station']['name']; ?></option>
                        <?php endforeach; ?>
                    </select>
                    <p class="help-block">The ground stations that you would like to use when calculating satellite pass times.</p>
                </div>
            </div>
            <div class="control-group">
                <label class="control-label" for="minimum_elevation">Minimum Elevation*</label>
                <div class="controls">
                    <div class="input-append">
                        <input type="text" name="minimum_elevation" id="minimum_elevation" value="<?php echo $passes_default_min_el['Configuration']['value']; ?>" style="width: 50px;" /><span class="add-on">&deg;</span>
                    </div>
                    <p class="help-block">The minimum elevation that a satellite needs to have over a ground station to result in a valid pass.</p>
                </div>
            </div>
            <div class="control-group">
                <label class="control-label" for="number_of_passes">Number of Passes*</label>
                <div class="controls">
                    <input type="text" name="number_of_passes" id="number_of_passes" value="<?php echo $passes_default_pass_count['Configuration']['value']; ?>" style="width: 50px;" />
                    <p class="help-block">How many valid passes to return in the result. Maximum value: <?php echo $passes_max_pass_count['Configuration']['value']; ?>.</p>
                </div>
            </div>
            <button type="submit" class="btn btn-success">Calculate Pass Times</button>
        </form>
    </div>
<?php else: ?>
    <div class="large_error">
        <?php echo $this->Html->image('icons/alert.png'); ?>
        <br />
        Pass times for this satellite can't be calculated because no ground stations are currently available. Please try again once some get added.
        <br /><br />
        <a href="<?php echo Router::url('/', true); ?>tools/passes" class="link">&laquo; Return To Index</a>
    </div>
<?php endif; ?>
