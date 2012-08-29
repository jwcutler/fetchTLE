<?php
/*
TLE Model

This model handles all TLE management and interaction options.
*/

// Load required classes
App::uses('Sanitize', 'Utility');

class Tle extends AppModel {
    var $name = 'Tle';
    
    // Define associations
    public $belongsTo = 'Update';
    
    public function tools_loadsatellites($satellite_names, $timestamp = false){
        /*
        Fetches the raw TLE information for the specified satellites(s) and returns it.
        
        @param $satellite_names: An array containing the names of all satellites to load.
        @param $timestamp: A UNIX timestamp to filter results.
        
        Returns:
            The raw database query result array on success.
            FALSE if none of the satellites can be found.
        */
        
        // Setup
        $mod_update = ClassRegistry::init('Update');
        $satellites = null;
		
        // Construct a query out of the names
        $query_satellite_names = Array();
        foreach($satellite_names as $satellite_name){
			//var_dump(urldecode($satellite_name));
            array_push($query_satellite_names, 'Tle.name=\''.$satellite_name.'\'');
        }
        $query_satellite_names = implode(' OR ',$query_satellite_names);
		
        // Check if a timestamp was set
        if ($timestamp){
            // Find the TLE's that are the closest to the specified timestamp along with their associated updates
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($timestamp).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($timestamp).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
        } else {
            // Grab the most recent TLE for each specified satellite along with its associated update
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND Tle.created_on < TleAlt.created_on) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
        }
        
        if (empty($satellites)){
            // Satellites could not be loaded
            return false;
        } else {
            return $satellites;
        }
    }
    
    public function api_loadsatellites($satellite_names, $timestamp = false){
        /*
        Fetches the TLE information for the specified satellite(s) and packages it into a custom array for use with the API.
        
        @param $satellite_names: An array containing the names of all satellites to load.
        @param $timestamp: A UNIX timestamp to filter results.
        */
        
        // Setup
        $result = Array();
        $mod_update = ClassRegistry::init('Update');
		
        // Construct a query out of the names
        $query_satellite_names = Array();
        foreach($satellite_names as $satellite_name){
			//var_dump(urldecode($satellite_name));
            array_push($query_satellite_names, 'Tle.name=\''.$satellite_name.'\'');
        }
        $query_satellite_names = implode(' OR ',$query_satellite_names);
		
        // Check if a timestamp was set
        if ($timestamp){
            // Find the TLE's that are the closest to the specified timestamp along with their associated updates
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($timestamp).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($timestamp).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
        } else {
            // Grab the most recent TLE for each specified satellite along with its associated update
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND Tle.created_on < TleAlt.created_on) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
        }
		
		//echo $this->getLastQuery();
		//var_dump($satellites);
        
        if (empty($satellites)){
            // No matching satellites found
            $result['status']['status'] = 'error';
            $result['status']['message'] = 'None of the provided satellites could be located or have been updated recently.';
            $result['status']['timestamp'] = time();
            $result['status']['satellites_fetched'] = 0;
        } else {
            // Results found, loop through each satellite and assemble the result
            foreach ($satellites as $satellite){
                $satellite_name = $satellite['Tle']['name'];
                
                // Set the satellite's status
                $result['satellites'][$satellite_name]['status']['updated'] = $satellite['Tle']['created_on'];
                
                // Set the satellite's TLE info
                $result['satellites'][$satellite_name]['tle'] = $satellite['Tle'];
                unset($result['satellites'][$satellite_name]['tle']['id']);
                unset($result['satellites'][$satellite_name]['tle']['created_on']);
                unset($result['satellites'][$satellite_name]['tle']['update_id']);
            }
                
            // At least 1 satellite loaded successfully
            if (count($result['satellites'])>=1){
                $result['status']['status'] = 'okay';
                $result['status']['message'] = 'At least one of the specified satellites was loaded.';
                $result['status']['timestamp'] = time();
                $result['status']['satellites_fetched'] = count($result['satellites']);
            } else {
                $result['status']['status'] = 'error';
                $result['status']['message'] = 'None of the specified satellites could be loaded.';
                $result['status']['timestamp'] = time();
                $result['status']['satellites_fetched'] = 0;
            }
        }
        
        return $result;
    }
    
    public function api_loadpositions($satellite_names, $start = false, $end = false, $resolution = false){
        /*
        Fetches position information for the specified satellites from $start to $end.
        
        @param $satellites: An array of satellite names.
        @param $start: Unix timestamp representing the beginning of the range of positions to calculate.
        @param $end: Unix timestamp representing the end of the range of positions to calculate.
        @param $resolution: How often to calculate the position (how many seconds between calculations).
        */
        
        // Setup
        $result = Array();
        $mod_update = ClassRegistry::init('Update');
        $calculation_limit = 2000;
        
        // Setup default values
        $resolution = ($resolution&&$resolution>=1)?$resolution:60; // Default to a minute between calculations
        $default_range = 24*60*60; // Jump by a day
        if ($start&&!$end){
            // Start timestamp specified but no end timestamp
            $end = $start+$default_range;
        } else if ($end&&!$start){
            // End timestamp specified but no start timestamp
            $start = $end-$default_range;
        } else if (!$start&&!$end){
            // Neither specified, just get the next $default_range
            $start = time();
            $end = $start+$default_range;
        }
        
        // Validate parameters
        if ($start>=$end){
            // Start timestamp later than the end timestamp
            $result['status']['status'] = 'error';
            $result['status']['message'] = 'End timestamp occurs earlier than the start timestamp (or they are equal).';
            $result['status']['timestamp'] = time();
            $result['status']['satellites_calculated'] = 0;
        } else if ((($end-$start)/$resolution)>$calculation_limit) {
            // Too small of a resolution
            $result['status']['status'] = 'error';
            $result['status']['message'] = 'Too many calculations ('.(($end-$start)/$resolution).') would be performed with that resolution (the limit is '.$calculation_limit.')';
            $result['status']['timestamp'] = time();
            $result['status']['satellites_calculated'] = 0;
        } else {
            // Construct a query out of the satellite names
            $query_satellite_names = Array();
            foreach($satellite_names as $satellite_name){
                //var_dump(urldecode($satellite_name));
                array_push($query_satellite_names, 'Tle.name=\''.$satellite_name.'\'');
            }
            $query_satellite_names = implode(' OR ',$query_satellite_names);
            
            // Load the TLEs that are closest to the starting timestamp
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($start).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($start).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
            
            //echo $this->getLastQuery().'<br /.';
            //var_dump($satellites);
            
            if (empty($satellites)){
                // No matching satellites found
                $result['status']['status'] = 'error';
                $result['status']['message'] = 'None of the provided satellites could be located or have been updated recently.';
                $result['status']['timestamp'] = time();
                $result['status']['satellites_fetched'] = 0;
            } else {
                // Needed TLEs loaded, calculate the positions
                $error_count = 0;
                foreach($satellites as $satellite){
                    $satellite_name = $satellite['Tle']['name'];
                    
                    // Perform the calculation
                    $position_results = Array();
                    exec(APP.'Vendor/satpos "'.$satellite_name.'" "'.$satellite['Tle']['raw_l1'].'" "'.$satellite['Tle']['raw_l2'].'" '.escapeshellarg($start).' '.escapeshellarg($end).' '.escapeshellarg($resolution), $position_results);
                    
                    // Check for calculation errors
                    if (strpos($position_results[0], '[ERROR]')!==FALSE){
                        $error_message = substr($position_results[0], 7);
                        
                        // Error calculating satellite positions
                        $error_count++;
                        $result['satellites'][$satellite_name]['status']['status'] = 'error';
                        $result['satellites'][$satellite_name]['status']['message'] = 'There was an error calculating the satellite positions: '.$error_message;
                        $result['satellites'][$satellite_name]['status']['generated_at'] = time();
                        $result['satellites'][$satellite_name]['status']['positions_calculated'] = 0;
                        $result['satellites'][$satellite_name]['status']['name'] = $satellite_name;
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_1'] = $satellite['Tle']['raw_l1'];
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_2'] = $satellite['Tle']['raw_l2'];
                        $result['satellites'][$satellite_name]['status']['timestamp_start'] = $start;
                        $result['satellites'][$satellite_name]['status']['timestamp_end'] = $end;
                        $result['satellites'][$satellite_name]['status']['resolution'] = $resolution;
                    } else {
                        // Parse the calculation results
                        foreach($position_results as $position_result){
                            list($timestamp, $latitude, $longitude, $altitude) = explode(':', $position_result);
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['timestamp'] = $timestamp;
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['latitude'] = $latitude;
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['longitude'] = $longitude;
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['altitude'] = $altitude;
                        }
                        
                        // Add the satellite status
                        $result['satellites'][$satellite_name]['status']['status'] = 'okay';
                        $result['satellites'][$satellite_name]['status']['message'] = 'Satellite positions calculated successfully.';
                        $result['satellites'][$satellite_name]['status']['generated_at'] = time();
                        $result['satellites'][$satellite_name]['status']['positions_calculated'] = sizeof($position_results);
                        $result['satellites'][$satellite_name]['status']['name'] = $satellite_name;
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_1'] = $satellite['Tle']['raw_l1'];
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_2'] = $satellite['Tle']['raw_l2'];
                        $result['satellites'][$satellite_name]['status']['timestamp_start'] = $start;
                        $result['satellites'][$satellite_name]['status']['timestamp_end'] = $end;
                        $result['satellites'][$satellite_name]['status']['resolution'] = $resolution;
                    }
                }
                
                // Add the main status element
                if ($error_count!=0){
                    $result['status']['status'] = 'error';
                    $result['status']['message'] = 'There was an error calculating the positions of '.$error_count.' of the specified satellites.';
                    $result['status']['timestamp'] = time();
                    $result['status']['satellites_calculated'] = count($result['satellites'])-$error_count;
                } else if (count($result['satellites'])>=1){
                    $result['status']['status'] = 'okay';
                    $result['status']['message'] = 'The positions of the specified satellites were calculated successfully.';
                    $result['status']['timestamp'] = time();
                    $result['status']['satellites_calculated'] = count($result['satellites']);
                } else {
                    $result['status']['status'] = 'error';
                    $result['status']['message'] = 'None of the specified satellites could be loaded.';
                    $result['status']['timestamp'] = time();
                    $result['status']['satellites_calculated'] = 0;
                }
            }
        }
        
        //var_dump($result);
        return $result;
    }
    
    public function arrayToXML($satellites){
        /*
        Converts the satellites API array into an XML string.
        
        @param $satellites: Array containing satellite data.
        
        Returns:
            An XML string representing the array.
        */
        
        // Start XML document.
        $xml_string = '<?xml version="1.0"?>';
        $xml_string .= '<api_satellites>';
        
        // Add the status element
        $xml_string .= '<status>';
        $xml_string .= '<status>'.htmlspecialchars($satellites['status']['status']).'</status>';
        $xml_string .= '<message>'.htmlspecialchars($satellites['status']['message']).'</message>';
        $xml_string .= '<timestamp>'.htmlspecialchars($satellites['status']['timestamp']).'</timestamp>';
        $xml_string .= '<satellites_fetched>'.htmlspecialchars($satellites['status']['satellites_fetched']).'</satellites_fetched>';
        $xml_string .= '</status>';
        
        // Add the satellites
        if (isset($satellites['satellites']) && !empty($satellites['satellites'])){
            $xml_string .= '<satellites>';
            foreach($satellites['satellites'] as $satellite_name => $satellite){
                $xml_string .= '<satellite name=\''.htmlspecialchars($satellite_name).'\'>';
                
                // Add the satellite status
                $xml_string .= '<status>';
                $xml_string .= '<updated>'.htmlspecialchars($satellite['status']['updated']).'</updated>';
                $xml_string .= '</status>';
                
                // Add the TLE information
                $xml_string .= '<tle>';
                $xml_string .= '<name>'.htmlspecialchars($satellite['tle']['name']).'</name>';
                $xml_string .= '<satellite_number>'.htmlspecialchars($satellite['tle']['satellite_number']).'</satellite_number>';
                $xml_string .= '<classification>'.htmlspecialchars($satellite['tle']['classification']).'</classification>';
                $xml_string .= '<launch_year>'.htmlspecialchars($satellite['tle']['launch_year']).'</launch_year>';
                $xml_string .= '<launch_number>'.htmlspecialchars($satellite['tle']['launch_number']).'</launch_number>';
                $xml_string .= '<launch_piece>'.htmlspecialchars($satellite['tle']['launch_piece']).'</launch_piece>';
                $xml_string .= '<epoch_year>'.htmlspecialchars($satellite['tle']['epoch_year']).'</epoch_year>';
                $xml_string .= '<epoch>'.htmlspecialchars($satellite['tle']['epoch']).'</epoch>';
                $xml_string .= '<ftd_mm_d2>'.htmlspecialchars($satellite['tle']['ftd_mm_d2']).'</ftd_mm_d2>';
                $xml_string .= '<std_mm_d6>'.htmlspecialchars($satellite['tle']['std_mm_d6']).'</std_mm_d6>';
                $xml_string .= '<bstar_drag>'.htmlspecialchars($satellite['tle']['bstar_drag']).'</bstar_drag>';
                $xml_string .= '<element_number>'.htmlspecialchars($satellite['tle']['element_number']).'</element_number>';
                $xml_string .= '<checksum_l1>'.htmlspecialchars($satellite['tle']['checksum_l1']).'</checksum_l1>';
                $xml_string .= '<inclination>'.htmlspecialchars($satellite['tle']['inclination']).'</inclination>';
                $xml_string .= '<right_ascension>'.htmlspecialchars($satellite['tle']['right_ascension']).'</right_ascension>';
                $xml_string .= '<eccentricity>'.htmlspecialchars($satellite['tle']['eccentricity']).'</eccentricity>';
                $xml_string .= '<perigee>'.htmlspecialchars($satellite['tle']['perigee']).'</perigee>';
                $xml_string .= '<mean_anomaly>'.htmlspecialchars($satellite['tle']['mean_anomaly']).'</mean_anomaly>';
                $xml_string .= '<mean_motion>'.htmlspecialchars($satellite['tle']['mean_motion']).'</mean_motion>';
                $xml_string .= '<revs>'.htmlspecialchars($satellite['tle']['revs']).'</revs>';
                $xml_string .= '<checksum_l2>'.htmlspecialchars($satellite['tle']['checksum_l2']).'</checksum_l2>';
                $xml_string .= '<raw_l1>'.$satellite['tle']['raw_l1'].'</raw_l1>';
                $xml_string .= '<raw_l2>'.$satellite['tle']['raw_l2'].'</raw_l2>';
                $xml_string .= '</tle>';
                
                $xml_string .= '</satellite>';
            }
            $xml_string .= '</satellites>';
        }
        
        // Close XML
        $xml_string .= '</api_satellites>';
        
        return $xml_string;
    }
    
    public function arrayToXMLPositions($satellites){
        /*
        Converts $satellites into an XML string.
        
        @param $satellites: Array containing satellite data.
        
        Returns:
            An XML string representing the array.
        */
        
        // Start XML document.
        $xml_string = '<?xml version="1.0"?>';
        $xml_string .= '<api_positions>';
        
        // Add the status element
        $xml_string .= '<status>';
        $xml_string .= '<status>'.htmlspecialchars($satellites['status']['status']).'</status>';
        $xml_string .= '<message>'.htmlspecialchars($satellites['status']['message']).'</message>';
        $xml_string .= '<timestamp>'.htmlspecialchars($satellites['status']['timestamp']).'</timestamp>';
        $xml_string .= '<satellites_calculated>'.htmlspecialchars($satellites['status']['satellites_calculated']).'</satellites_calculated>';
        $xml_string .= '</status>';
        
        // Add the satellites
        if (isset($satellites['satellites']) && !empty($satellites['satellites'])){
            $xml_string .= '<satellites>';
            foreach($satellites['satellites'] as $satellite_name => $satellite){
                $xml_string .= '<satellite name=\''.htmlspecialchars($satellite_name).'\'>';
                
                // Add the satellite status
                $xml_string .= '<status>';
                $xml_string .= '<status>'.htmlspecialchars($satellite['status']['status']).'</status>';
                $xml_string .= '<message>'.htmlspecialchars($satellite['status']['message']).'</message>';
                $xml_string .= '<generated_at>'.htmlspecialchars($satellite['status']['generated_at']).'</generated_at>';
                $xml_string .= '<positions_calculated>'.htmlspecialchars($satellite['status']['positions_calculated']).'</positions_calculated>';
                $xml_string .= '<name>'.htmlspecialchars($satellite['status']['name']).'</name>';
                $xml_string .= '<raw_tle_line_1>'.htmlspecialchars($satellite['status']['raw_tle_line_1']).'</raw_tle_line_1>';
                $xml_string .= '<raw_tle_line_2>'.htmlspecialchars($satellite['status']['raw_tle_line_2']).'</raw_tle_line_2>';
                $xml_string .= '<timestamp_start>'.htmlspecialchars($satellite['status']['timestamp_start']).'</timestamp_start>';
                $xml_string .= '<timestamp_end>'.htmlspecialchars($satellite['status']['timestamp_end']).'</timestamp_end>';
                $xml_string .= '<resolution>'.htmlspecialchars($satellite['status']['resolution']).'</resolution>';
                $xml_string .= '</status>';
                
                // Add the TLE information
                if (array_key_exists('positions', $satellite)){
                    $xml_string .= '<positions>';
                    foreach ($satellite['positions'] as $satellite_position){
                        $xml_string .= '<position timestamp=\''.$satellite_position['timestamp'].'\'>';
                        $xml_string .= '<latitude>'.$satellite_position['latitude'].'</latitude>';
                        $xml_string .= '<longitude>'.$satellite_position['longitude'].'</longitude>';
                        $xml_string .= '<altitude>'.$satellite_position['altitude'].'</altitude>';
                        $xml_string .= '</position>';
                    }
                    $xml_string .= '</positions>';
                }
                
                $xml_string .= '</satellite>';
            }
            $xml_string .= '</satellites>';
        }
        
        // Close XML
        $xml_string .= '</api_positions>';
        
        return $xml_string;
    }
    
    public function arrayToRaw($satellites){
        /*
        Converts the supplied array into a raw TLE file. Duplicates all ready handled by the queries in api_loadsatellites().
        
        @param $satellites: Array of satellite TLE's.
        */
        
        $raw_tles = Array();
        
        if (isset($satellites['satellites']) && !empty($satellites['satellites'])){
            foreach($satellites['satellites'] as $satellite){
                $temp_satellite = Array(
                    'name' => $satellite['tle']['name'],
                    'raw_l1' => $satellite['tle']['raw_l1'],
                    'raw_l2' => $satellite['tle']['raw_l2']
                );
                
                array_push($raw_tles, $temp_satellite);
            }
        }
        
        return $raw_tles;
    }
    
    public function arrayToRawPositions($satellites){
        /*
        Converts $satellites to a Text/HTML position output.
        
        @param $satellites: Array of calculated satellite positions.
        */
        
        $raw_positions = Array();
        
        if (isset($satellites['satellites']) && !empty($satellites['satellites'])){
            foreach($satellites['satellites'] as $satellite){
                if ($satellite['status']['status']=='okay'){
                    foreach ($satellite['positions'] as $satellite_position){
                        $temp_satellite = Array(
                            'name' => $satellite['status']['name'],
                            'timestamp' => $satellite_position['timestamp'],
                            'latitude' => $satellite_position['latitude'],
                            'longitude' => $satellite_position['longitude'],
                            'altitude' => $satellite_position['altitude']
                        );
                        array_push($raw_positions, $temp_satellite);
                    }
                }
            }
        }
        
        return $raw_positions;
    }
    
    public function parse_tle_source($raw_tle_data, $update_id){
        /*
        Parses the provided raw TLE source.
        
        @param $raw_tle_data: The raw TLE file from the source.
        @param $update_id: The ID of the parent update.
        
        Returns:
            True on success
            Human readable error message on failure
        */
        
        // Make sure some data is present
        if (!empty($raw_tle_data)){
            // Attempt to read in the TLE source
            $tle_line_counter = 0;
            $tle_entry_counter = 0;
            $new_tle_entries = array();
            $valid_tle = true;
            $tle_lines = explode("\n", $raw_tle_data);
            foreach ($tle_lines as $tle_line){
                // Skip empty lines
                if (empty($tle_line)){
                    continue;
                }
                
                // Check if the entry needs to be saved
                if ($tle_line_counter==3){
                    $tle_line_counter = 0;
                    $tle_entry_counter++;
                }
                
                // Add a new TLE entry array if needed
                if ($tle_line_counter == 0){
                    array_push($new_tle_entries, array());
                }
                
                // Parse the first line
                if ($tle_line_counter == 0){
                    $new_tle_entries[$tle_entry_counter]['name'] = trim($tle_line);
                    $new_tle_entries[$tle_entry_counter]['update_id'] = $update_id;
                    $new_tle_entries[$tle_entry_counter]['created_on'] = time();
                }
                
                // Parse the second line
                if ($tle_line_counter == 1){
                    // Make sure the TLE line is the right length
                    if (strlen(trim($tle_line))!=69){
                        $valid_tle = false;
                        break;
                    }
                    
                    $new_tle_entries[$tle_entry_counter]['satellite_number'] = trim(substr($tle_line, 2, 5));
                    $new_tle_entries[$tle_entry_counter]['classification'] = trim(substr($tle_line, 7, 1));
                    $new_tle_entries[$tle_entry_counter]['launch_year'] = trim(substr($tle_line, 9, 2));
                    $new_tle_entries[$tle_entry_counter]['launch_number'] = trim(substr($tle_line, 11, 3));
                    $new_tle_entries[$tle_entry_counter]['launch_piece'] = trim(substr($tle_line, 14, 1));
                    $new_tle_entries[$tle_entry_counter]['epoch_year'] = trim(substr($tle_line, 18, 2));
                    $new_tle_entries[$tle_entry_counter]['epoch'] = trim(substr($tle_line, 20, 12));
                    $new_tle_entries[$tle_entry_counter]['ftd_mm_d2'] = trim(substr($tle_line, 33, 12));
                    $new_tle_entries[$tle_entry_counter]['std_mm_d6'] = trim(substr($tle_line, 45, 7));
                    $new_tle_entries[$tle_entry_counter]['bstar_drag'] = trim(substr($tle_line, 53, 8));
                    $new_tle_entries[$tle_entry_counter]['element_number'] = trim(substr($tle_line, 65, 3));
                    $new_tle_entries[$tle_entry_counter]['checksum_l1'] = trim(substr($tle_line, 68, 1));
                    $new_tle_entries[$tle_entry_counter]['raw_l1'] = trim($tle_line);
                }
                
                // Parse the third line
                if ($tle_line_counter == 2){
                    // Make sure the TLE line is the right length
                    if (strlen(trim($tle_line))!=69){
                        $valid_tle = false;
                        break;
                    }
                    
                    $new_tle_entries[$tle_entry_counter]['inclination'] = trim(substr($tle_line, 8, 8));
                    $new_tle_entries[$tle_entry_counter]['right_ascension'] = trim(substr($tle_line, 17, 8));
                    $new_tle_entries[$tle_entry_counter]['eccentricity'] = trim(substr($tle_line, 26, 7));
                    $new_tle_entries[$tle_entry_counter]['perigee'] = trim(substr($tle_line, 34, 8));
                    $new_tle_entries[$tle_entry_counter]['mean_anomaly'] = trim(substr($tle_line, 43, 8));
                    $new_tle_entries[$tle_entry_counter]['mean_motion'] = trim(substr($tle_line, 52, 11));
                    $new_tle_entries[$tle_entry_counter]['revs'] = trim(substr($tle_line, 63, 5));
                    $new_tle_entries[$tle_entry_counter]['checksum_l2'] = trim(substr($tle_line, 68, 1));
                    $new_tle_entries[$tle_entry_counter]['raw_l2'] = trim($tle_line);
                }
                
                $tle_line_counter++;
            }
            
            if ($valid_tle){
                // Add the TLE's
                $save_result = $this->saveMany($new_tle_entries, array('deep' => true));
                
                if ($save_result){
                    // Success
                    return true;
                } else {
                    // Error saving TLE's
                    return 'There was an error saving the TLE\'s from the source.';
                }
            } else {
                // Invalid TLE file
                return 'The TLE source was invalid (not a valid TLE file).';
            }
        } else {
            // Missing data
            return 'The TLE source was empty.';
        }
    }
}
?>
