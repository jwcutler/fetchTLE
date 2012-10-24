<?php
/*
Station Model
*/

// Load required classess
App::uses('Sanitize', 'Utility');

class Station extends AppModel {
    var $name = 'Station';
    
    // Validation rules
    public $validate = array(
        'name' => array(
            'name_unique' => array(
                'rule' => 'isUnique',
                'required' => true
            )
        )
    );
    
    public function api_loadpasses($satellite_name, $ground_stations = false, $min_elevations = false, $pass_count = false, $start_date = false, $show_all_passes = true){
        /*
        This API calculates the first $passcount passes over $ground_stations using the most recent TLE for $satellite_name.
        
        @param $satellite_name: The name of the satellite to calculate pass times for.
        @param $ground_stations: An array containing the names of the ground stations to consider when calculating passes.
        @param $min_elevations: An array containing the minimum elevations to use for each ground station in $ground_stations.
        @param $pass_count: The number of valid (i.e. meets parameters) passes to calculate.
        @param $start_date: A unix timestamp of when to start calculating passes. 
        @param $show_all_passes: Whether or not all passes (including ones that don't meet the requirements) should be shown.
        
        Returns:
            An array containing all of the passes (invalid or valid) for the specified satellite over the specified ground stations.
        */
        
        // Setup
        $result = array();
        $Configuration = ClassRegistry::init('Configuration');
        $Station = ClassRegistry::init('Station');
        $gs_success = true;
        $stations_elevations = array();
        
        // Load the required configuration
        $configurations = $Configuration->find('all', array(
            'conditions' => array(
                'Configuration.name' => array('passes_default_min_el', 'passes_available_ground_stations', 'passes_default_ground_stations', 'passes_default_pass_count', 'passes_max_pass_count')
            )
        ));
        foreach($configurations as $configuration){
            $$configuration['Configuration']['name'] = $configuration['Configuration']['value'];
        }
        
        // Set the default parameters
        if (!$pass_count){
            $pass_count = $passes_default_pass_count;
        }
        if (!$start_date){
            $start_date = time();
        }
        
        // Verify the extra parameters
        $error_message = null;
        if (empty($satellite_name)){
            // Empty satellite names
            $error_message = 'Invalid satellite name specified.';
        } else if ($pass_count <= 1){
            // Too small of a pass count
            $error_message = 'At least one pass must be requested.';
        } else if ($pass_count > $passes_max_pass_count){
            // Pass count above limit
            $error_message = 'Pass count above limit. Maximum pass count: '.$passes_max_pass_count;
        } else if (!is_numeric($start_date)) {
            // Invalid timestamp
            $error_message = 'Your starting timestamp is invalid.';
        }
        
        // Loop through and verify the elevations
        if (is_array($min_elevations)){
            foreach ($min_elevations as $min_elevation){
                if (is_numeric($min_elevation)){
                    if ($min_elevation < 0 || $min_elevation > 90){
                        $error_message = 'Your minimum elevations must be >= 0 and <= 90.';
                        break;
                    }
                } else {
                    $error_message = 'Your minimum elevations must be numeric.';
                    break;
                }
            }
        }
        
        // Attempt to load the specified ground stations
        if ($ground_stations){
            // Ground stations specified, try to load them
            $stations = $Station->find('all', array(
                'conditions' => array(
                    'Station.name' => $ground_stations
                )
            ));
            
            // Loop through and figure out what ground stations couldn't be loaded.
            $missing_stations = array();
            foreach($ground_stations as $ground_station_index => $ground_station){
                $station_found = false;
                foreach ($stations as $station){
                    if ($station['Station']['name'] == $ground_station){
                        // Store the elevation to use for the ground station
                        $station_elevation = $passes_default_min_el;
                        if (is_array($min_elevations)){
                            if (array_key_exists($ground_station_index, $min_elevations)){
                                $station_elevation = $min_elevations[$ground_station_index];
                            }
                        }
                        $stations_elevations[$station['Station']['name']] = $station_elevation;
                        
                        $station_found = true;
                        break;
                    }
                }
                
                if (!$station_found){
                    array_push($missing_stations, $ground_station);
                }
            }
            if (!empty($missing_stations)){
                $missing_stations = implode(', ', $missing_stations);
                $error_message = 'The following ground stations could not be found: '.$missing_stations;
            }
        } else {
            // No ground stations specified, load the defaults
            $default_ground_stations = explode(',', $passes_default_ground_stations);
            $stations = $Station->find('all', array(
                'conditions' => array(
                    'Station.id' => $default_ground_stations
                )
            ));
            
            if (empty($stations)){
                $error_message = 'None of the default ground stations could be found.';
            } else {
                // Fill up the elevations array
                foreach($stations as $station){
                    $stations_elevations[$station['Station']['name']] = $passes_default_min_el;
                }
            }
        }
        
        // Grab the most recent TLE for the specified satellite
        $satellite = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($start_date).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($start_date).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND (Tle.name=\''.Sanitize::clean($satellite_name).'\')');
        if (empty($satellite)){
            $error_message = 'The satellite \''.$satellite_name.'\' could not be found.';
        }
        
        // Check if any errors have occured
        if (!$error_message){
            // Iterate through ground stations
            $satellite_passes = array();
            $result['passes'] = array();
            foreach ($stations as $station_index => $station){
                // Invoke the propagator and load the results
                $gs_satellite_passes = array();
                $station_latitude = $station['Station']['latitude'];
                $station_longitude = $station['Station']['longitude']*-1; // Negate because SatTrack is weird
                exec(APP.'Vendor/SatTrack/sattrack -b UTC "'.$station['Station']['name'].'" '.$station_latitude.' '.$station_longitude.' "'.$satellite[0]['Tle']['name'].'" "'.$satellite[0]['Tle']['raw_l1'].'" "'.$satellite[0]['Tle']['raw_l2'].'" shortpr '.gmdate("dMy", $start_date).' 0:0:0 auto 30 0 '.$stations_elevations[$station['Station']['name']].' nohardcopy', $gs_satellite_passes);
                
                if (strpos($gs_satellite_passes[0], 'SUCCESS')===FALSE){
                    // Some sort of error occured, set the error response and exit the loop
                    $result['status']['status'] = 'error';
                    $result['status']['message'] = 'There was an error calculating the pass times for \''.$satellite[0]['Tle']['name'].'\'.';
                    $result['status']['timestamp'] = time();
                    $result['status']['total_passes_loaded'] = 0;
                    $result['status']['valid_passes_loaded'] = 0;
                    $gs_success = false;
                    break;
                } else {
                    // Everything okay, first remove the "SUCCESS" message
                    array_shift($gs_satellite_passes);
                    
                    // Build the array item for each pass
                    foreach ($gs_satellite_passes as $gs_satellite_pass){
                        // Parse the line
                        list($pass_date, $pass_aos, $pass_mel, $pass_los, $pass_el_start, $pass_el_end, $pass_duration, $pass_aos_az, $pass_mel_az, $pass_los_az, $pass_peak_elev, $pass_vis, $pass_orbit) = explode('$', $gs_satellite_pass);
                        
                        if ($show_all_passes || $pass_peak_elev>=$stations_elevations[$station['Station']['name']]){
                            // Convert the times into a timestamp
                            $pass_date_array = explode('-', $pass_date);
                            $pass_aos_array = explode(':', $pass_aos);
                            $pass_aos_timestamp = gmmktime($pass_aos_array[0], $pass_aos_array[1], $pass_aos_array[2], $pass_date_array[0], $pass_date_array[1], $pass_date_array[2]);
                            $pass_mel_array = explode(':', $pass_mel);
                            $pass_mel_timestamp = gmmktime($pass_mel_array[0], $pass_mel_array[1], $pass_mel_array[2], $pass_date_array[0], $pass_date_array[1], $pass_date_array[2]);
                            $pass_los_array = explode(':', $pass_los);
                            $pass_los_timestamp = gmmktime($pass_los_array[0], $pass_los_array[1], $pass_los_array[2], $pass_date_array[0], $pass_date_array[1], $pass_date_array[2]);
                            $pass_el_start_array = explode(':', $pass_el_start);
                            $pass_el_start_timestamp = ($pass_el_start_array[0]=='0'&&$pass_el_start_array[1]=='0'&&$pass_el_start_array[2]=='0')?false:gmmktime($pass_el_start_array[0], $pass_el_start_array[1], $pass_el_start_array[2], $pass_date_array[0], $pass_date_array[1], $pass_date_array[2]);
                            $pass_el_end_array = explode(':', $pass_el_end);
                            $pass_el_end_timestamp = ($pass_el_end_array[0]=='0'&&$pass_el_end_array[1]=='0'&&$pass_el_end_array[2]=='0')?false:gmmktime($pass_el_end_array[0], $pass_el_end_array[1], $pass_el_end_array[2], $pass_date_array[0], $pass_date_array[1], $pass_date_array[2]);
                            
                            // Filter out passes that occur before the start time but in the same day
                            if ($pass_aos_timestamp >= $start_date){
                                // Detect if any timestamps rolled over to the next day and correct
                                $pass_mel_timestamp = ($pass_mel_timestamp < $pass_aos_timestamp)?$pass_mel_timestamp+(24*60*60):$pass_mel_timestamp;
                                $pass_los_timestamp = ($pass_los_timestamp < $pass_aos_timestamp)?$pass_los_timestamp+(24*60*60):$pass_los_timestamp;
                                if ($pass_el_start_timestamp != false){
                                  $pass_el_start_timestamp = ($pass_el_start_timestamp < $pass_aos_timestamp)?$pass_el_start_timestamp+(24*60*60):$pass_el_start_timestamp;
                                }
                                if ($pass_el_end_timestamp != false){
                                  $pass_el_end_timestamp = ($pass_el_end_timestamp < $pass_aos_timestamp)?$pass_el_end_timestamp+(24*60*60):$pass_el_end_timestamp;
                                }
                                
                                // Add the pass to the array
                                $temp_pass = array();
                                $temp_pass['pass']['orbit_number'] = intval($pass_orbit);
                                $temp_pass['pass']['aos'] = $pass_aos_timestamp;
                                $temp_pass['pass']['aos_az'] = floatval($pass_aos_az);
                                $temp_pass['pass']['mel'] = $pass_mel_timestamp;
                                $temp_pass['pass']['mel_az'] = floatval($pass_mel_az);
                                $temp_pass['pass']['los'] = $pass_los_timestamp;
                                $temp_pass['pass']['los_az'] = floatval($pass_los_az);
                                $temp_pass['pass']['acceptable_el_start'] = $pass_el_start_timestamp;
                                $temp_pass['pass']['acceptable_el_end'] = $pass_el_end_timestamp;
                                $temp_pass['pass']['duration'] = $pass_duration;
                                $temp_pass['pass']['peak_elevation'] = floatval($pass_peak_elev);
                                $temp_pass['pass']['acceptable'] = ($pass_peak_elev>=$stations_elevations[$station['Station']['name']])?true:false;
                                $temp_pass['pass']['ground_station'] = $station['Station']['name'];
                                
                                array_push($result['passes'], $temp_pass);
                            }
                        }
                    }
                }
            }
            
            if ($gs_success){
                // Sort the passes by timestamp
                usort($result['passes'], array($this,"sort_passes"));
                
                // Loop through and remove extra passes
                $acceptable_pass_count = 0;
                foreach ($result['passes'] as $pass_number => $temp_pass){
                    if ($temp_pass['pass']['acceptable']){
                        $acceptable_pass_count++;
                    }
                    
                    if ($acceptable_pass_count >= $pass_count){
                        // Reached the pass limit, remove the rest if there are any
                        if ((count($result['passes'])-1)>=$pass_number+1){
                            array_splice($result['passes'], $pass_number+1);
                        }
                        break;
                    }
                }
                
                // Construct status element
                $result['status']['status'] = 'okay';
                $result['status']['message'] = 'Passes were computed successfully.';
                $result['status']['timestamp'] = time();
                $result['status']['total_passes_loaded'] = count($result['passes']);
                $result['status']['acceptable_passes_loaded'] = $acceptable_pass_count;
            }
        } else {
            // Some sort of error occured when setting up the API
            $result['status']['status'] = 'error';
            $result['status']['message'] = $error_message;
            $result['status']['timestamp'] = time();
            $result['status']['total_passes_loaded'] = 0;
            $result['status']['acceptable_passes_loaded'] = 0;
        }
        
        // Add the request parameters
        $result['status']['params']['pass_count'] = intval($pass_count);
        $result['status']['params']['timestamp'] = $start_date;
        $result['status']['params']['show_all_passes'] = $show_all_passes;
        $result['status']['params']['satellite'] = $satellite_name;
        if (!empty($satellite)){
            $result['status']['params']['raw_tle_line_1'] = $satellite[0]['Tle']['raw_l1'];
            $result['status']['params']['raw_tle_line_2'] = $satellite[0]['Tle']['raw_l2'];
        }
        if (!empty($stations)){
            $result['status']['params']['ground_stations']  = array();
            foreach($stations as $station_index => $station){
                $result['status']['params']['ground_stations'][$station['Station']['name']]['min_elevation'] = intval($stations_elevations[$station['Station']['name']]);
                $result['status']['params']['ground_stations'][$station['Station']['name']]['name'] = $station['Station']['name'];
                $result['status']['params']['ground_stations'][$station['Station']['name']]['latitude'] = floatval($station['Station']['latitude']);
                $result['status']['params']['ground_stations'][$station['Station']['name']]['longitude'] = floatval($station['Station']['longitude']);
                $result['status']['params']['ground_stations'][$station['Station']['name']]['description'] = $station['Station']['description'];
            }
        }
        
        //var_dump($result);
        
        return $result;
    }
    
    public function arrayToRawPasses($pass_times){
        /*
        Converts the supplied array into a raw satellite pass file. Duplicates all ready handled by the queries in api_loadpasses().
        
        @param $pass_times: Array of computed pass times.
        */
        
        $raw_passes = Array();
        
        // Check if there were any errors
        if ($pass_times['status']['status']=='okay'){
            if (isset($pass_times['passes']) && !empty($pass_times['passes'])){
                foreach($pass_times['passes'] as $temp_pass){
                    $temp_pass = Array(
                        'satellite_name' => $pass_times['status']['params']['satellite'],
                        'orbit_number' => $temp_pass['pass']['orbit_number'],
                        'aos' => $temp_pass['pass']['aos'],
                        'aos_az' => $temp_pass['pass']['aos_az'],
                        'mel' => $temp_pass['pass']['mel'],
                        'mel_az' => $temp_pass['pass']['mel_az'],
                        'los' => $temp_pass['pass']['los'],
                        'los_az' => $temp_pass['pass']['los_az'],
                        'acceptable_el_start' => $temp_pass['pass']['acceptable_el_start'],
                        'acceptable_el_end' => $temp_pass['pass']['acceptable_el_start'],
                        'duration' => $temp_pass['pass']['duration'],
                        'peak_elevation' => $temp_pass['pass']['peak_elevation'],
                        'acceptable' => $temp_pass['pass']['acceptable'],
                        'ground_station' => $temp_pass['pass']['ground_station']
                    );
                    
                    array_push($raw_passes, $temp_pass);
                }
            }
        } else {
            // Some sort of error
            $raw_passes = null;
            $raw_passes = $pass_times['status']['message'];
        }
        
        return $raw_passes;
    }
    
    public function arrayToXMLPasses($pass_times){
        /*
        Converts $pass_times into an XML string.
        
        @param $pass_times: Array of passes to convert into XML.
        
        Returns:
            An XML string representing the array.
        */
        
        // Start XML document.
        $xml_string = '<?xml version="1.0"?>';
        $xml_string .= '<api_passes>';
        
        // Add the status element
        $xml_string .= '<status>';
            $xml_string .= '<status>'.htmlspecialchars($pass_times['status']['status']).'</status>';
            $xml_string .= '<message>'.htmlspecialchars($pass_times['status']['message']).'</message>';
            $xml_string .= '<timestamp>'.htmlspecialchars($pass_times['status']['timestamp']).'</timestamp>';
            $xml_string .= '<total_passes_loaded>'.htmlspecialchars($pass_times['status']['total_passes_loaded']).'</total_passes_loaded>';
            $xml_string .= '<acceptable_passes_loaded>'.htmlspecialchars($pass_times['status']['acceptable_passes_loaded']).'</acceptable_passes_loaded>';
            $xml_string .= '<params>';
                $xml_string .= '<pass_count>'.htmlspecialchars($pass_times['status']['params']['pass_count']).'</pass_count>';
                $xml_string .= '<timestamp>'.htmlspecialchars($pass_times['status']['params']['timestamp']).'</timestamp>';
                $show_all_passes = ($pass_times['status']['params']['show_all_passes'])?'true':'false';
                $xml_string .= '<show_all_passes>'.htmlspecialchars($show_all_passes).'</show_all_passes>';
                $xml_string .= '<satellite>'.htmlspecialchars($pass_times['status']['params']['satellite']).'</satellite>';
                if (isset($pass_times['status']['params']['raw_tle_line_1'])){
                    $xml_string .= '<raw_tle_line_1>'.htmlspecialchars($pass_times['status']['params']['raw_tle_line_1']).'</raw_tle_line_1>';
                }
                if (isset($pass_times['status']['params']['raw_tle_line_2'])){
                    $xml_string .= '<raw_tle_line_2>'.htmlspecialchars($pass_times['status']['params']['raw_tle_line_2']).'</raw_tle_line_2>';
                }
                if (isset($pass_times['status']['params']['ground_stations'])){
                    $xml_string .= '<ground_stations>';
                        foreach ($pass_times['status']['params']['ground_stations'] as $ground_station_name => $ground_station){
                            $xml_string .= '<ground_station name=\''.htmlspecialchars($ground_station_name).'\'>';
                                $xml_string .= '<name>'.htmlspecialchars($ground_station_name).'</name>';
                                $xml_string .= '<min_elevation>'.htmlspecialchars($ground_station['min_elevation']).'</min_elevation>';
                                $xml_string .= '<latitude>'.htmlspecialchars($ground_station['latitude']).'</latitude>';
                                $xml_string .= '<longitude>'.htmlspecialchars($ground_station['longitude']).'</longitude>';
                                $xml_string .= '<description>'.htmlspecialchars($ground_station['description']).'</description>';
                            $xml_string .= '</ground_station>';
                        }
                    $xml_string .= '</ground_stations>';
                }
            $xml_string .= '</params>';
        $xml_string .= '</status>';
        
        // Add the satellites
        if (isset($pass_times['passes']) && !empty($pass_times['passes'])){
            $xml_string .= '<passes>';
            foreach($pass_times['passes'] as $temp_pass){
                $xml_string .= '<pass aos=\''.htmlspecialchars($temp_pass['pass']['aos']).'\'>';
                    $xml_string .= '<orbit_number>'.htmlspecialchars($temp_pass['pass']['orbit_number']).'</orbit_number>';
                    $xml_string .= '<aos>'.htmlspecialchars($temp_pass['pass']['aos']).'</aos>';
                    $xml_string .= '<aos_az>'.htmlspecialchars($temp_pass['pass']['aos_az']).'</aos_az>';
                    $xml_string .= '<mel>'.htmlspecialchars($temp_pass['pass']['mel']).'</mel>';
                    $xml_string .= '<mel_az>'.htmlspecialchars($temp_pass['pass']['mel_az']).'</mel_az>';
                    $xml_string .= '<los>'.htmlspecialchars($temp_pass['pass']['los']).'</los>';
                    $xml_string .= '<los_az>'.htmlspecialchars($temp_pass['pass']['los_az']).'</los_az>';
                    $xml_string .= '<acceptable_el_start>'.htmlspecialchars($temp_pass['pass']['acceptable_el_start']).'</acceptable_el_start>';
                    $xml_string .= '<acceptable_el_end>'.htmlspecialchars($temp_pass['pass']['acceptable_el_end']).'</acceptable_el_end>';
                    $xml_string .= '<duration>'.htmlspecialchars($temp_pass['pass']['duration']).'</duration>';
                    $xml_string .= '<peak_elevation>'.htmlspecialchars($temp_pass['pass']['peak_elevation']).'</peak_elevation>';
                    $acceptable_text = ($temp_pass['pass']['acceptable'])?'true':'false';
                    $xml_string .= '<acceptable>'.htmlspecialchars($acceptable_text).'</acceptable>';
                    $xml_string .= '<ground_station>'.htmlspecialchars($temp_pass['pass']['ground_station']).'</ground_station>';
                $xml_string .= '</pass>';
            }
            $xml_string .= '</passes>';
        }
        
        // Close XML
        $xml_string .= '</api_passes>';
        
        return $xml_string;
    }
    
    private function sort_passes($first_pass, $second_pass){
        /*
        This function is used with USORT to sort the pass API array by timestamp.
        
        @param $first_pass: First pass to compare
        @param $second_pass: Second pass to compare
        */
        
        return $first_pass['pass']['aos'] > $second_pass['pass']['aos'];
    }
}
?>
