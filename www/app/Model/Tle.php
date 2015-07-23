<?php
/*
TLE Model

This model handles all TLE management and interaction options.
*/

// Load required classess
App::uses('Sanitize', 'Utility', 'Configure');

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
            array_push($query_satellite_names, 'Tle.name=\''.Sanitize::escape($satellite_name).'\'');
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
    
    public function api_loadsatellites($satellite_names, $timestamp = false, $start = false, $count = 1, $page = 1){
        /*
        Fetches the TLE information for the specified satellite(s) and packages it into a custom array for use with the API.
        
        @param $satellite_names: An array containing the names of all satellites to load.
        @param $timestamp: A UNIX timestamp used to query for the closest TLE.
        @param $start: A UNIX timestamp indicating the start of a TLE range. Takes precedence over $timestamp.
        @param $count: The number of TLEs, per satellite, to return per page. Only used when a start time has been specified.
        @param $page: The current page of the result set, starting from $start. Starts from 1.
        */
        
        // Setup
        $result = Array();
        $satellites = Array();
        $tles_fetched = 0;
        $mod_update = ClassRegistry::init('Update');
        $timestamp = is_numeric($timestamp) ? $timestamp : false;

        foreach($satellite_names as $satellite_name){
            if ($start){
                // Load the TLEs for each satellite that are newer than $start for the current page and count
                $count = (!is_numeric($count) || $count < 1) ? Configure::read('fetchTLE.tleCount') : $count;
                $count = ($count > Configure::read('fetchTLE.tleCountMax')) ? Configure::read('fetchTLE.tleCountMax') : $count;
                $page = (!is_numeric($page) || $page < 1) ? 1 : $page;
                $offset = ($page - 1) * $count;

                $satellites[$satellite_name] = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE Tle.created_on >= \''.Sanitize::escape($start).'\' AND Tle.name=\''.Sanitize::escape($satellite_name).'\' ORDER BY Tle.created_on ASC LIMIT '.Sanitize::escape($offset).','.Sanitize::escape($count));
            } else if ($timestamp) {
                // Load the TLEs that are closest to $timestamp for each satellite (only 1 TLE will be returned per satellite)
                $satellites[$satellite_name] = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($timestamp).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($timestamp).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND Tle.name=\''.Sanitize::escape($satellite_name).'\'');
            } else {
                // Load the most recent TLEs for each satellite (only 1 TLE will be returned per satellite)
                $satellites[$satellite_name] = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND Tle.created_on < TleAlt.created_on) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND Tle.name=\''.Sanitize::escape($satellite_name).'\'');
                $timestamp = time();
            }

            $tles_fetched += count($satellites[$satellite_name]);
        }
        
        if ($tles_fetched <= 0){
            // No matching satellites found
            $result['status']['status'] = 'error';
            $result['status']['message'] = 'None of the provided satellites could be located or have TLEs that match the request parameters.';
            $result['status']['timestamp'] = time();
            $result['status']['satellites_fetched'] = 0;
            $result['status']['tles_fetched'] = 0;
        } else {
            // Results found, loop through each satellite and assemble the result
            foreach ($satellites as $satellite_name => $tles){
                $result['satellites'][$satellite_name] = Array();

                foreach($tles as $tle){
                    $temp_tle = Array();

                    $temp_tle['status']['loaded'] = intval($tle['Tle']['created_on']);
                    $temp_tle['tle'] = $tle['Tle'];
                    unset($temp_tle['tle']['id']);
                    unset($temp_tle['tle']['created_on']);
                    unset($temp_tle['tle']['update_id']);

                    array_push($result['satellites'][$satellite_name], $temp_tle);
                }
            }
                
            // Generate request status
            $result['status']['status'] = 'okay';
            $result['status']['message'] = 'At least one TLE was returned for the specified satellites.';
            $result['status']['timestamp'] = time();
            $result['status']['satellites_fetched'] = count($result['satellites']);
            $result['status']['tles_fetched'] = $tles_fetched;
        }
        
        // Add the request parameters
        if ($start){
            $result['status']['params']['start'] = intval($start);
            $result['status']['params']['count'] = intval($count);
            $result['status']['params']['page'] = intval($page);
        } else {
            $result['status']['params']['timestamp'] = intval($timestamp);
        }
        
        $result['status']['params']['satellites'] = array();
        foreach($satellite_names as $temp_satellite_name){
            array_push($result['status']['params']['satellites'], $temp_satellite_name);
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
                array_push($query_satellite_names, 'Tle.name=\''.Sanitize::escape($satellite_name).'\'');
            }
            $query_satellite_names = implode(' OR ',$query_satellite_names);
            
            // Load the TLEs that are closest to the starting timestamp
            $satellites = $this->query('SELECT Tle.*,`Update`.* FROM tles Tle LEFT JOIN tles TleAlt ON (Tle.name = TleAlt.name AND ABS(Tle.created_on - \''.Sanitize::clean($start).'\') > ABS(TleAlt.created_on - \''.Sanitize::clean($start).'\')) INNER JOIN updates AS `Update` ON (`Update`.id = Tle.update_id) WHERE TleAlt.id IS NULL AND ('.$query_satellite_names.')');
            
            //echo $this->getLastQuery().'<br />';
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
                    exec(APP.'Vendor/Satellite_Position/satpos "'.$satellite_name.'" "'.$satellite['Tle']['raw_l1'].'" "'.$satellite['Tle']['raw_l2'].'" '.escapeshellarg($start).' '.escapeshellarg($end).' '.escapeshellarg($resolution), $position_results);

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
                        $result['satellites'][$satellite_name]['status']['timestamp_start'] = intval($start);
                        $result['satellites'][$satellite_name]['status']['timestamp_end'] = intval($end);
                        $result['satellites'][$satellite_name]['status']['resolution'] = intval($resolution);
                    } else {
                        // Parse the calculation results
                        foreach($position_results as $position_result){
                            list($timestamp, $latitude, $longitude, $altitude) = explode(':', $position_result);
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['timestamp'] = intval($timestamp);
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['latitude'] = floatval($latitude);
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['longitude'] = floatval($longitude);
                            $result['satellites'][$satellite_name]['positions'][$timestamp]['altitude'] = floatval($altitude);
                        }
                        
                        // Add the satellite status
                        $result['satellites'][$satellite_name]['status']['status'] = 'okay';
                        $result['satellites'][$satellite_name]['status']['message'] = 'Satellite positions calculated successfully.';
                        $result['satellites'][$satellite_name]['status']['generated_at'] = time();
                        $result['satellites'][$satellite_name]['status']['positions_calculated'] = sizeof($position_results);
                        $result['satellites'][$satellite_name]['status']['name'] = $satellite_name;
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_1'] = $satellite['Tle']['raw_l1'];
                        $result['satellites'][$satellite_name]['status']['raw_tle_line_2'] = $satellite['Tle']['raw_l2'];
                        $result['satellites'][$satellite_name]['status']['timestamp_start'] = intval($start);
                        $result['satellites'][$satellite_name]['status']['timestamp_end'] = intval($end);
                        $result['satellites'][$satellite_name]['status']['resolution'] = intval($resolution);
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
        
        // Add the request parameters
        $result['status']['params']['start'] = intval($start);
        $result['status']['params']['end'] = intval($end);
        $result['status']['params']['resolution'] = intval($resolution);
        $result['status']['params']['satellites'] = array();
        foreach($satellite_names as $temp_satellite_name){
            array_push($result['status']['params']['satellites'], $temp_satellite_name);
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
        $xml_string .= '<tles_fetched>'.htmlspecialchars($satellites['status']['tles_fetched']).'</tles_fetched>';
            $xml_string .= '<params>';
                if (isset($satellites['status']['params']['timestamp'])){
                    $xml_string .= '<timestamp>'.htmlspecialchars($satellites['status']['params']['timestamp']).'</timestamp>';
                }
                if (isset($satellites['status']['params']['start'])){
                    $xml_string .= '<start>'.htmlspecialchars($satellites['status']['params']['start']).'</start>';
                    $xml_string .= '<count>'.htmlspecialchars($satellites['status']['params']['count']).'</count>';
                    $xml_string .= '<page>'.htmlspecialchars($satellites['status']['params']['page']).'</page>';
                }
                $xml_string .= '<satellites>';
                    foreach ($satellites['status']['params']['satellites'] as $temp_satellite){
                        $xml_string .= '<satellite>'.htmlspecialchars($temp_satellite).'</satellite>';
                    }
                $xml_string .= '</satellites>';
            $xml_string .= '</params>';
        $xml_string .= '</status>';
        
        // Add the satellites
        if (isset($satellites['satellites']) && !empty($satellites['satellites'])){
            $xml_string .= '<satellites>';
            foreach($satellites['satellites'] as $satellite_name => $tles){
                $xml_string .= '<satellite name=\''.htmlspecialchars($satellite_name).'\'>';
                    $xml_string .= "<tles>";
                    
                    foreach ($tles as $tle){
                        // Add the TLE information
                        $xml_string .= '<tle>';
                        $xml_string .= '<name>'.htmlspecialchars($tle['tle']['name']).'</name>';
                        $xml_string .= '<satellite_number>'.htmlspecialchars($tle['tle']['satellite_number']).'</satellite_number>';
                        $xml_string .= '<classification>'.htmlspecialchars($tle['tle']['classification']).'</classification>';
                        $xml_string .= '<launch_year>'.htmlspecialchars($tle['tle']['launch_year']).'</launch_year>';
                        $xml_string .= '<launch_number>'.htmlspecialchars($tle['tle']['launch_number']).'</launch_number>';
                        $xml_string .= '<launch_piece>'.htmlspecialchars($tle['tle']['launch_piece']).'</launch_piece>';
                        $xml_string .= '<epoch_year>'.htmlspecialchars($tle['tle']['epoch_year']).'</epoch_year>';
                        $xml_string .= '<epoch>'.htmlspecialchars($tle['tle']['epoch']).'</epoch>';
                        $xml_string .= '<ftd_mm_d2>'.htmlspecialchars($tle['tle']['ftd_mm_d2']).'</ftd_mm_d2>';
                        $xml_string .= '<std_mm_d6>'.htmlspecialchars($tle['tle']['std_mm_d6']).'</std_mm_d6>';
                        $xml_string .= '<bstar_drag>'.htmlspecialchars($tle['tle']['bstar_drag']).'</bstar_drag>';
                        $xml_string .= '<element_number>'.htmlspecialchars($tle['tle']['element_number']).'</element_number>';
                        $xml_string .= '<checksum_l1>'.htmlspecialchars($tle['tle']['checksum_l1']).'</checksum_l1>';
                        $xml_string .= '<inclination>'.htmlspecialchars($tle['tle']['inclination']).'</inclination>';
                        $xml_string .= '<right_ascension>'.htmlspecialchars($tle['tle']['right_ascension']).'</right_ascension>';
                        $xml_string .= '<eccentricity>'.htmlspecialchars($tle['tle']['eccentricity']).'</eccentricity>';
                        $xml_string .= '<perigee>'.htmlspecialchars($tle['tle']['perigee']).'</perigee>';
                        $xml_string .= '<mean_anomaly>'.htmlspecialchars($tle['tle']['mean_anomaly']).'</mean_anomaly>';
                        $xml_string .= '<mean_motion>'.htmlspecialchars($tle['tle']['mean_motion']).'</mean_motion>';
                        $xml_string .= '<revs>'.htmlspecialchars($tle['tle']['revs']).'</revs>';
                        $xml_string .= '<checksum_l2>'.htmlspecialchars($tle['tle']['checksum_l2']).'</checksum_l2>';
                        $xml_string .= '<raw_l1>'.$tle['tle']['raw_l1'].'</raw_l1>';
                        $xml_string .= '<raw_l2>'.$tle['tle']['raw_l2'].'</raw_l2>';
                        $xml_string .= '<status>';
                            $xml_string .= '<loaded>'.htmlspecialchars($tle['status']['loaded']).'</loaded>';
                        $xml_string .= '</status>';
                        $xml_string .= '</tle>';
                    }
                    
                    $xml_string .= "</tles>";
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
        $xml_string .= '<params>';
            $xml_string .= '<start>'.htmlspecialchars($satellites['status']['params']['start']).'</start>';
            $xml_string .= '<end>'.htmlspecialchars($satellites['status']['params']['end']).'</end>';
            $xml_string .= '<resolution>'.htmlspecialchars($satellites['status']['params']['resolution']).'</resolution>';
            $xml_string .= '<satellites>';
                foreach ($satellites['status']['params']['satellites'] as $temp_satellite){
                    $xml_string .= '<satellite>'.htmlspecialchars($temp_satellite).'</satellite>';
                }
            $xml_string .= '</satellites>';
        $xml_string .= '</params>';
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
            foreach($satellites['satellites'] as $tles){
                foreach($tles as $tle){
                    $temp_tle = Array(
                        'name' => $tle['tle']['name'],
                        'raw_l1' => $tle['tle']['raw_l1'],
                        'raw_l2' => $tle['tle']['raw_l2']
                    );
                    
                    array_push($raw_tles, $temp_tle);
                }
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
            if (strpos($raw_tle_data,"\n") !== false){
              $tle_lines = explode("\n", $raw_tle_data);
            } else {
              $tle_lines = explode("\r", $raw_tle_data);
            }
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

                    // Make sure that the launch information isn't blank
                    $trim_year = trim(substr($tle_line, 9, 2));
                    $launch_year = empty($trim_year) ? "00" : $trim_year;
                    $trim_number = trim(substr($tle_line, 11, 3));
                    $launch_number = empty($trim_number) ? "000" : $trim_number;
                    $trim_piece = trim(substr($tle_line, 14, 1));
                    $launch_piece = empty($trim_piece) ? "A" : $trim_piece;
                    
                    $new_tle_entries[$tle_entry_counter]['satellite_number'] = trim(substr($tle_line, 2, 5));
                    $new_tle_entries[$tle_entry_counter]['classification'] = trim(substr($tle_line, 7, 1));
                    $new_tle_entries[$tle_entry_counter]['launch_year'] = $launch_year;
                    $new_tle_entries[$tle_entry_counter]['launch_number'] = $launch_number;
                    $new_tle_entries[$tle_entry_counter]['launch_piece'] = $launch_piece;
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
                var_dump($save_result);

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
