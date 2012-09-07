<?php echo $this->Html->script('chosen.jquery.min.js'); ?>
<?php echo $this->Html->script('jquery-ui-1.8.23.custom.min.js'); ?>
<?php echo $this->Html->css('chosen.css'); ?>
<?php echo $this->Html->css('flick/jquery-ui-1.8.23.custom.css'); ?>
<script type="text/javascript">
$(document).ready(function(){
    // Hide the results table
    $("#passes_results").hide();
    
    // Enable the ground station selection box
    $("#ground_stations").chosen();
    
    // Enable date selection box
    $("#start_date").datepicker();
    
    // Setup the modals
    $("#error_modal").modal({
        keyboard: true,
        show: true, // Broken
        backdrop: true
    });
    $("#error_modal").modal('hide');
    
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
            pass_count: {
                required: true,
                digits: true,
                min: 1,
                //max: <?php echo $passes_max_pass_count['Configuration']['value']; ?>
                max: 100
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
            pass_count: {
                required: "Please enter the number of passes requested.",
                digits: "The number of passes must be an integer.",
                min: "You must request at least 1 pass.",
                max: "You can't request more than <?php echo $passes_max_pass_count['Configuration']['value']; ?> passes."
            },
            ground_stations: {
                required: "You must select at least one ground station."
            }
        },
        submitHandler: function(form){
            var api_endpoint = "<?php echo $this->webroot; ?>api/passes/<?php echo rawurlencode($satellite[0]['Tle']['name']); ?>.json"; // Pass API endpoint
            
            // Hide the submit button and show the spinner
            $("#calculate_pass_times").hide();
            $("#loading_passes").show();
            
            // Load the form parameters
            pass_count = $('#pass_count').val();
            minimum_elevation = $('#minimum_elevation').val();
            show_all_passes = ($('#show_all_passes').is(':checked'))?'true':'false';
            ground_stations = $('#ground_stations').val().join('_');
            start_date = null;
            if ($('#start_date').val()){
                start_date = $('#start_date').val().split("/");
                start_date = Date.UTC(start_date[2],start_date[0]-1,start_date[1])/1000;
            } else {
                start_date = '';
            }

            $.ajax({
                type: "GET",
                url: api_endpoint,
                data: {minelevation: minimum_elevation, passcount: pass_count, ground_stations: ground_stations, timestamp: start_date, show_all_passes: show_all_passes},
                success: function(response){
                    // Check if the API generated any errors
                    if (response['status']['status'] == 'okay'){
                        // Load the passes into a table
                        for (pass_index in response['passes']){
                            // Convert Times
                            AOS = new Date(response['passes'][pass_index]['pass']['aos']*1000);
                            MEL = new Date(response['passes'][pass_index]['pass']['mel']*1000);
                            MEL_25 = new Date((response['passes'][pass_index]['pass']['mel']*1000)-(150*1000));
                            MEL_3 = new Date((response['passes'][pass_index]['pass']['mel']*1000)-(180*1000));
                            MEL_35 = new Date((response['passes'][pass_index]['pass']['mel']*1000)-(210*1000));
                            LOS = new Date(response['passes'][pass_index]['pass']['los']*1000);
                            pass_result_row = '<tr>';
                            if (response['passes'][pass_index]['pass']['acceptable']){
                                pass_result_row += '<td style="border-top: none;"><img src=\'<?php echo $this->webroot; ?>img/icons/small_success.png\' alt=\'Acceptable Pass\' /></td>';
                            } else {
                                pass_result_row += '<td style="border-top: none;"></td>';
                            }
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['orbit_number']+'</td>';
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['ground_station']+'</td>';
                            pass_result_row += '<td>'+AOS.toLocaleDateString()+' '+pad(AOS.getHours())+':'+pad(AOS.getMinutes())+':'+pad(AOS.getSeconds())+'</td>';
                            pass_result_row += '<td>'+pad(AOS.getUTCMonth()+1)+'/'+pad(AOS.getUTCDate())+'/'+AOS.getUTCFullYear()+'</td>';
                            pass_result_row += '<td>'+pad(AOS.getUTCHours())+':'+pad(AOS.getUTCMinutes())+':'+pad(AOS.getUTCSeconds())+'</td>';
                            pass_result_row += '<td>'+pad(MEL.getUTCHours())+':'+pad(MEL.getUTCMinutes())+':'+pad(MEL.getUTCSeconds())+'</td>';
                            pass_result_row += '<td>'+pad(LOS.getUTCHours())+':'+pad(LOS.getUTCMinutes())+':'+pad(LOS.getUTCSeconds())+'</td>';
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['duration']+'</td>';
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['peak_elevation']+'&deg;</td>';
                            pass_result_row += '<td>'+pad(MEL_25.getUTCHours())+':'+pad(MEL_25.getUTCMinutes())+':'+pad(MEL_25.getUTCSeconds())+'</td>';
                            pass_result_row += '<td>'+pad(MEL_3.getUTCHours())+':'+pad(MEL_3.getUTCMinutes())+':'+pad(MEL_3.getUTCSeconds())+'</td>';
                            pass_result_row += '<td>'+pad(MEL_35.getUTCHours())+':'+pad(MEL_35.getUTCMinutes())+':'+pad(MEL_35.getUTCSeconds())+'</td>';
                            pass_result_row += '</tr>';
                            $("#pass_results tbody").append(pass_result_row);
                        }
                        
                        // Hide the form
                        $("#passes_form").hide();
                        
                        // Show the table
                        $("#passes_results").show(400);
                        
                        // Show the button again
                        $("#calculate_pass_times").show();
                        $("#loading_passes").hide();
                    } else {
                        // Some sort of API error
                        $("#error_message").html(response['status']['message']);
                        $("#error_modal").modal('show');
                        
                        // Show the button again
                        $("#calculate_pass_times").show();
                        $("#loading_passes").hide();
                    }
                },
                error: function(xhr, ajaxOptions, thrownError){
                    // General request error
                    $("#error_message").html(thrownError);
                    $("#error_modal").modal('show');
                    
                    // Show the button again
                    $("#calculate_pass_times").show();
                    $("#loading_passes").hide();
                }
            });
        }
    });
    
    // Detect "Return To Calculator" button
    $("#return_to_calculator").click(function(){
        // Clear the old results
        $("#pass_results tbody").html("");
        
        // Hide the results and show the form
        $("#passes_results").hide(400);
        $("#passes_form").show(400);
    });
});

// Used to pad time values from the API
function pad(n){
    return ("0" + n).slice(-2);
}
</script>
<h1 class="titles">'<?php echo $satellite[0]['Tle']['name']; ?>' Pass Time Calculator</h1>
<?php if ($ground_stations): ?>
    <div id="passes_form">
        <p>Use this utility to calculate the upcoming pass times for the '<?php echo $satellite[0]['Tle']['name']; ?>' satellite.</p>
        <div class="form_errors">
            <ul>
            </ul>
        </div>
        <div style="margin-top: 20px;width: 70%;">
            <form method="GET" class="form-horizontal" id="calculate_form">
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
                                <option value="<?php echo $ground_station['Station']['name']; ?>"><?php echo $ground_station['Station']['name']; ?></option>
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
                        <p class="help-block">The minimum elevation that a satellite needs to have over a ground station to be considered "acceptable".</p>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label" for="pass_count">Number of Passes*</label>
                    <div class="controls">
                        <input type="text" name="pass_count" id="pass_count" value="<?php echo $passes_default_pass_count['Configuration']['value']; ?>" style="width: 50px;" />
                        <p class="help-block">How many acceptable passes to return in the result. Maximum value: <?php echo $passes_max_pass_count['Configuration']['value']; ?>.</p>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label" for="start_date">Start Date</label>
                    <div class="controls">
                        <input type="text" name="start_date" id="start_date" />
                        <p class="help-block">The start date to use when calculating passes. If left blank the current date will be used.</p>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label" for="show_all_passes">Show All Passes</label>
                    <div class="controls">
                        <input type="checkbox" name="show_all_passes" id="show_all_passes" value="true" />
                        <p class="help-block">If selected, all passes calculated during the selected time frame (including unacceptable) will be included in the response.</p>
                    </div>
                </div>
                <button type="submit" class="btn btn-success" id="calculate_pass_times">Calculate Pass Times</button>
                <div id="loading_passes" style="display: none; font-size: 18px; color: #1c6618;">
                    <?php echo $this->Html->image('ajax_spinner_green.png', array('style' => 'vertical-align: middle;')); ?> Loading Passes...
                </div>
            </form>
        </div>
    </div>
    <div id="passes_results">
        <button class="btn btn-primary btn-small" type="button" id="return_to_calculator">Return To Calculator</button>
        <div style="margin: 10px 0px 10px 0px;">All times are in UTC except for the "AOS Local Time" column.</div>
        <table class="table table-condensed table-hover" id="pass_results">
            <thead>
                <th width="20px"></th>
                <th>Orbit #</th>
                <th>Station</th>
                <th>AOS Local Time</th>
                <th>UTC Date</th>
                <th>AOS</th>
                <th>MEL</th>
                <th>LOS</th>
                <th>Duration</th>
                <th>Peak El.</th>
                <th>MEL-2.5 min</th>
                <th>MEL-3 min</th>
                <th>MEL-3.5 min</th>
            </thead>
            <tbody>
            </tbody>
        </table>
    </div>
    
    <!-- ERROR Modal -->
    <div class="modal hide" id="error_modal" style="width: 400px;margin-left: -200px;">
        <div class="modal-header">
            <h3>An Error Occured</h3>
        </div>
        <div class="modal-body">
            An error occured when calculating your requested pass times.
            <div style="padding: 10px 0px 10px 10px;">
                Error: <span id="error_message"></span>
            </div>
            Please try again later.
        </div>
        <div class="modal-footer">
            <a href="#" class="btn" data-dismiss="modal">Okay</a>
        </div>
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
