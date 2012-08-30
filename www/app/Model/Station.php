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
    
    public function api_loadpasses($satellite_name, $ground_stations = false, $min_elevation = false, $pass_count = false){
        /*
        This API calculates the first $passcount passes over $ground_stations using the most recent TLE for $satellite_name.
        
        @param $satellite_name: The name of the satellite to calculate pass times for.
        @param $ground_stations: An array containing the names of the ground stations to consider when calculating passes.
        @param $min_elevation: The minimum elevation to accept when calculating passes.
        @param $pass_count: The number of valid (i.e. meets parameters) passes to calculate.
        
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
            // No errors, perform science
            
        } else {
            // Some sort of error occured when setting up the API
            $result['status']['status'] = 'error';
            $result['status']['message'] = $error_message;
            $result['status']['timestamp'] = time();
            $result['status']['satellites_included'] = 0;
        }
        
        return $result;
    }
}
?>
