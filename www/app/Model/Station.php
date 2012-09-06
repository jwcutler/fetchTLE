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
    
    public function api_loadpasses($satellite_name, $ground_stations = false, $min_elevation = false, $pass_count = false, $start_date = false){
        /*
        This API calculates the first $passcount passes over $ground_stations using the most recent TLE for $satellite_name.
        
        @param $satellite_name: The name of the satellite to calculate pass times for.
        @param $ground_stations: An array containing the names of the ground stations to consider when calculating passes.
        @param $min_elevation: The minimum elevation to accept when calculating passes.
        @param $pass_count: The number of valid (i.e. meets parameters) passes to calculate.
        @param $start_date: A unix timestamp of when to start calculating passes. 
        
        Returns:
            An array containing all of the passes (invalid or valid) for the specified satellite over the specified ground stations.
        */
        
        // Setup
        $result = array();
        $Configuration = ClassRegistry::init('Configuration');
        $Station = ClassRegistry::init('Station');
        
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
        if (!$min_elevation){
            $min_elevation = $passes_default_min_el;
        }
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
        } else if ($min_elevation < 0 || $min_elevation > 90){
            // Minimum elevation out of range
            $error_message = 'The minimum elevation must be >= 0 and <= 90.';
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
            foreach($ground_stations as $ground_station){
                $station_found = false;
                foreach ($stations as $station){
                    if ($station['Station']['name'] == $ground_station){
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
            }
        }
        
        // Grab the most recent TLE for the specified satellite
        $satellite = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND Tle.created_on < TleAlt.created_on) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND (Tle.name=\''.Sanitize::clean($satellite_name).'\')');
        if (empty($satellite)){
            $error_message = 'The satellite \''.$satellite_name.'\' could not be found.';
        }
        
        // Check if any errors have occured
        if (!$error_message){
            // Iterate through ground stations
            $satellite_passes = array();
            foreach ($stations as $station){
                // Invoke the propagator and load the results
                $gs_satellite_passes = array();
                exec(APP.'Vendor/SatTrack/sattrack -b UTC "'.$station['Station']['name'].'" '.$station['Station']['latitude'].' '.$station['Station']['longitude'].' "'.$satellite[0]['Tle']['name'].'" "'.$satellite[0]['Tle']['raw_l1'].'" "'.$satellite[0]['Tle']['raw_l2'].'" shortpr '.date("dMy", $start_date).' 0:0:0 auto 30 0 nohardcopy', $gs_satellite_passes);
                
                if (strpos($gs_satellite_passes[0], 'SUCCESS')===FALSE){
                    // Some sort of error occured, set the error response
                    $result['status']['status'] = 'error';
                    $result['status']['message'] = 'There was an error calculating the pass times for \''.$satellite[0]['Tle']['name'].'\'.';
                    $result['status']['timestamp'] = time();
                    $result['status']['total_passes_loaded'] = 0;
                    $result['status']['valid_passes_loaded'] = 0;
                } else {
                    // Everything okay, first remove the "SUCCESS" message
                    array_shift($gs_satellite_passes);
                    
                    // Build the array item for each pass
                    foreach ($gs_satellite_passes as $gs_satellite_pass){
                        // Parse the line
                        list($pass_date, $pass_aos, $pass_mel, $pass_los, $pass_duration, $pass_aos_az, $pass_mel_az, $pass_los_az, $pass_peak_elev, $pass_vis, $pass_orbit) = explode('$', $gs_satellite_pass);
                        
                        // Convert the date into a timestamp
                        $pass_date_timestamp = strtotime($pass_date.' '.$pass_aos);
                        
                        echo $pass_date.'-'.$pass_date_timestamp."<br />";
                    }
                }
            }
            
            // Sort the passes by timestamp
            
            // Check if $pass_count was too big
        } else {
            // Some sort of error occured when setting up the API
            $result['status']['status'] = 'error';
            $result['status']['message'] = $error_message;
            $result['status']['timestamp'] = time();
            $result['status']['total_passes_loaded'] = 0;
            $result['status']['valid_passes_loaded'] = 0;
        }
        
        return $result;
    }
}
?>
