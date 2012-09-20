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
            pass_count: {
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
            minimum_elevations = new Array();
            pass_count = $('#pass_count').val();
            minimum_elevation = $('#minimum_elevation').val();
            show_all_passes = ($('#show_all_passes').is(':checked'))?'true':'false';
            show_utc = ($('#show_utc').is(':checked'))?true:false;
            ground_stations_temp = $('#ground_stations').val();
            ground_stations = ground_stations_temp.join('_');
            start_date = null;
            if ($('#start_date').val()){
                start_date = $('#start_date').val().split("/");
                start_date = Date.UTC(start_date[2],start_date[0]-1,start_date[1])/1000;
            } else {
                start_date = '';
            }
            for (ground_station_index in ground_stations_temp){
                // Load the min elevation for the specified ground station
                if ($("#minimum_elevation_"+ground_stations_temp[ground_station_index]).length){
                    minimum_elevations.push($("#minimum_elevation_"+ground_stations_temp[ground_station_index]).val());
                } else {
                    minimum_elevations.push('<?php echo $passes_default_min_el['Configuration']['value']; ?>');
                }
            }
            minelevations = minimum_elevations.join('_');

            $.ajax({
                type: "GET",
                url: api_endpoint,
                data: {min_elevations: minelevations, pass_count: pass_count, ground_stations: ground_stations, timestamp: start_date, show_all_passes: show_all_passes},
                success: function(response){
                    // Check if the API generated any errors
                    if (response['status']['status'] == 'okay'){
                        // Load the passes into a table
                        for (pass_index in response['passes']){
                            // Convert Times
                            AOS = new Date(response['passes'][pass_index]['pass']['aos']*1000);
                            MEL = new Date(response['passes'][pass_index]['pass']['mel']*1000);
                            LOS = new Date(response['passes'][pass_index]['pass']['los']*1000);
                            acceptable_el_start = (response['passes'][pass_index]['pass']['acceptable_el_start']==false)?false:new Date(response['passes'][pass_index]['pass']['acceptable_el_start']*1000);
                            acceptable_el_end = (response['passes'][pass_index]['pass']['acceptable_el_end']==false)?false:new Date(response['passes'][pass_index]['pass']['acceptable_el_end']*1000);
                            pass_result_row = '<tr>';
                            if (response['passes'][pass_index]['pass']['acceptable']){
                                pass_result_row += '<td style="border-top: none;"><img src=\'<?php echo $this->webroot; ?>img/icons/small_success.png\' alt=\'Acceptable Pass\' /></td>';
                            } else {
                                pass_result_row += '<td style="border-top: none;"></td>';
                            }
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['orbit_number']+'</td>';
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['ground_station']+'</td>';
                            pass_result_row += '<td>'+response['passes'][pass_index]['pass']['peak_elevation']+'&deg;</td>';
                            if (show_utc){
                                // Display all times in UTC
                                pass_result_row += '<td>'+pad(AOS.getUTCMonth()+1)+'/'+pad(AOS.getUTCDate())+'/'+AOS.getUTCFullYear()+'</td>';
                                pass_result_row += '<td>'+pad(AOS.getUTCHours())+':'+pad(AOS.getUTCMinutes())+':'+pad(AOS.getUTCSeconds())+'</td>';
                                pass_result_row += '<td>'+pad(MEL.getUTCHours())+':'+pad(MEL.getUTCMinutes())+':'+pad(MEL.getUTCSeconds())+'</td>';
                                pass_result_row += '<td>'+pad(LOS.getUTCHours())+':'+pad(LOS.getUTCMinutes())+':'+pad(LOS.getUTCSeconds())+'</td>';
                                pass_result_row += '<td>'+response['passes'][pass_index]['pass']['duration']+'</td>';
                                if (!acceptable_el_start){
                                    pass_result_row += '<td>NA</td>';
                                } else {
                                    pass_result_row += '<td>'+pad(acceptable_el_start.getUTCHours())+':'+pad(acceptable_el_start.getUTCMinutes())+':'+pad(acceptable_el_start.getUTCSeconds())+'</td>';
                                }
                                if (!acceptable_el_end){
                                    pass_result_row += '<td>NA</td>';
                                } else {
                                    pass_result_row += '<td>'+pad(acceptable_el_end.getUTCHours())+':'+pad(acceptable_el_end.getUTCMinutes())+':'+pad(acceptable_el_end.getUTCSeconds())+'</td>';
                                }
                                
                                // Display the timezone
                                $("#timezone_notification").html("Note: All times and dates are in UTC.");
                            } else {
                                // Display times in local timezone
                                pass_result_row += '<td>'+pad(AOS.getMonth()+1)+'/'+pad(AOS.getDate())+'/'+AOS.getFullYear()+'</td>';
                                pass_result_row += '<td>'+pad(AOS.getHours())+':'+pad(AOS.getMinutes())+':'+pad(AOS.getSeconds())+'</td>';
                                pass_result_row += '<td>'+pad(MEL.getHours())+':'+pad(MEL.getMinutes())+':'+pad(MEL.getSeconds())+'</td>';
                                pass_result_row += '<td>'+pad(LOS.getHours())+':'+pad(LOS.getMinutes())+':'+pad(LOS.getSeconds())+'</td>';
                                pass_result_row += '<td>'+response['passes'][pass_index]['pass']['duration']+'</td>';
                                if (!acceptable_el_start){
                                    pass_result_row += '<td>NA</td>';
                                } else {
                                    pass_result_row += '<td>'+pad(acceptable_el_start.getHours())+':'+pad(acceptable_el_start.getMinutes())+':'+pad(acceptable_el_start.getSeconds())+'</td>';
                                }
                                if (!acceptable_el_end){
                                    pass_result_row += '<td>NA</td>';
                                } else {
                                    pass_result_row += '<td>'+pad(acceptable_el_end.getHours())+':'+pad(acceptable_el_end.getMinutes())+':'+pad(acceptable_el_end.getSeconds())+'</td>';
                                }
                                
                                // Display the timezone
                                time_zone_string_temp = AOS.toTimeString();
                                time_zone_string = time_zone_string_temp.substr(-4, 3);
                                time_zone_offset_string = time_zone_string_temp.substr(-14, 8);
                                $("#timezone_notification").html("Note: All times and dates are in "+time_zone_string+" ("+time_zone_offset_string+").");
                            }
                            // Calculate the duration of the acceptable elevation window
                            if (!acceptable_el_start && !acceptable_el_end){
                                pass_result_row += '<td>NA</td>';
                            } else {
                                // Calculate the difference
                                acceptable_duration = Math.abs(acceptable_el_end - acceptable_el_start)/1000; // In Seconds
                                hours = Math.floor(acceptable_duration/3600);
                                minutes = Math.floor((acceptable_duration/60) % 60);
                                seconds = acceptable_duration % 60;
                                pass_result_row += '<td>'+pad(hours)+':'+pad(minutes)+':'+pad(seconds)+'</td>';
                            }
                            
                            pass_result_row += '</tr>';
                            $("#pass_results tbody").append(pass_result_row);
                            
                            // Populate the request parameter panel
                            $("#passcount_notification").html("Number of Acceptable Passes To Calculate: <i>"+response['status']['params']['pass_count']);
                            $("#showallpasses_notification").html("Show All Passes: <i>"+response['status']['params']['show_all_passes']+"</i>");
                            $("#satellite_tle_notification").html("TLE Used In Calculations: <div style='margin: 0px 0px 0px 20px;font-style: italic;'><pre style='margin: 0px; padding: 0px; background-color: #eeeeee; border: none; line-height: 1; font-size: 11px; color: black;'>"+response['status']['params']['satellite']+"<br />"+response['status']['params']['raw_tle_line_1']+"<br />"+response['status']['params']['raw_tle_line_2']+"</pre></div>");
                            $("#groundstations_notification").html("");
                            for (ground_station_index in response['status']['params']['ground_stations']){
                                $("#groundstations_notification").append(response['status']['params']['ground_stations'][ground_station_index]['name']+" minimum elevation: <i>"+response['status']['params']['ground_stations'][ground_station_index]['min_elevation']+"&deg;</i><br />");
                            }
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
    
    // Exand calculation parameters
    $("#show_parameters").click(function (){
        link_content = $(this).html();
            
        if (link_content == '[- Show Calculation Parameters]'){
            $(this).html('[+ Show Calculation Parameters]');
        } else {
            $(this).html('[- Show Calculation Parameters]');
        }
        
        $("#calculation_parameters").toggle(400);
    });
    
    // Detect ground station changes
    $("#ground_stations").change(function(){
        // Hide the default text
        $("#minimum_elevation_no_stations").hide();
        
        // Add a minimum elevation field for each ground station (if it doesn't all ready exist)
        selected_stations = $(this).val();
        for (station_index in selected_stations){
            station_name = selected_stations[station_index]
            
            // Add the elevation field if it doesn't all ready exist
            if (!$("#elevation_field_"+station_name).length){
                $("#minimum_elevation_container").append("<div id='elevation_field_"+station_name+"' title ='"+station_name+"' style='margin-bottom: 3px;'><div class=\"input-prepend input-append\"><span class=\"add-on\">"+station_name+"</span><input type=\"text\" name=\"minimum_elevation_"+station_name+"\" id=\"minimum_elevation_"+station_name+"\" value=\"<?php echo $passes_default_min_el['Configuration']['value']; ?>\" style=\"width: 50px;\" /><span class=\"add-on\">&deg;</span></div></div>");
                
                // Add form validation for the new elevation field
                $("#minimum_elevation_"+station_name).rules("add", {
                    required: true,
                    digits: true,
                    min: 0,
                    max: 90,
                    messages: {
                        required: "Please enter a minimum elevation for '"+station_name+"'.",
                        digits: "The minimum elevation for '"+station_name+"' must be an integer.",
                        min: "The minimum elevation for '"+station_name+"' must be at least 0 degrees.",
                        max: "The maximum elevation for '"+station_name+"' can't be larger than 90 degrees."
                    }
                });
            }
        }
        
        // Remove any elevation fields for deleted ground stations
        $("[id^=elevation_field_]").each(function(){
            // Check if the station is selected
            if (jQuery.inArray($(this).attr('title'), selected_stations)==-1){
                // Remove the form validation
                $("#minimum_elevation_"+$(this).attr('title')).rules("remove");
                
                // Not selected, so remove it
                $(this).remove();
            }
        });
        
        // Check if any ground stations are left
        if (!$('[id^=elevation_field_]').length){
            $("#minimum_elevation_no_stations").show();
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
                                <option value="<?php echo rawurlencode($ground_station['Station']['name']); ?>"><?php echo $ground_station['Station']['name']; ?></option>
                            <?php endforeach; ?>
                        </select>
                        <p class="help-block">The ground stations that you would like to use when calculating satellite pass times.</p>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label" for="minimum_elevation">Minimum Elevations*</label>
                    <div class="controls">
                        <div id="minimum_elevation_container">
                            <div id="minimum_elevation_no_stations" style="font-style: italic;">
                                Select some ground stations above to specify the elevations.
                            </div>
                        </div>
                        <p class="help-block" style="margin-top: 10px;">The minimum elevation that a satellite needs to have over a specific ground station to be considered "acceptable".</p>
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
                        <p class="help-block">If selected, all passes calculated during the selected time frame (including unacceptable passes) will be included in the response.</p>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label" for="show_utc">Display UTC Times</label>
                    <div class="controls">
                        <input type="checkbox" name="show_utc" id="show_utc" value="true" checked='checked' />
                        <p class="help-block">If selected, all dates and times displayed will be in UTC. Otherwise, all dates and times will be displayed in your local timezone.</p>
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
        <br />
        <div style="background-color: #eeeeee; display: inline-block; padding: 5px; margin: 10px 0px 10px 0px; border: 1px solid #cccccc; border-radius: 4px; -moz-border-radius: 4px;">
            <div>
                <a class="link" id="show_parameters" style="cursor: pointer;">[+ Show Calculation Parameters]</a>
            </div>
            <div style="margin: 10px 0px 0px 0px; display:none;" id="calculation_parameters">
                <div id="passcount_notification"></div>
                <div id="showallpasses_notification"></div>
                <div id="satellite_tle_notification"></div>
                <div id="groundstations_notification"></div>
            </div>
        </div>
        <div style="margin-bottom: 10px;" id="timezone_notification"></div>
        <table class="table table-condensed table-hover" id="pass_results">
            <thead>
                <th width="20px"></th>
                <th>Orbit #</th>
                <th>Station</th>
                <th>Peak El.</th>
                <th>Date</th>
                <th>AOS</th>
                <th>MEL</th>
                <th>LOS</th>
                <th>Duration</th>
                <th>Acceptable El. Start</th>
                <th>Acceptable El. End</th>
                <th>Acceptable Duration</th>
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
