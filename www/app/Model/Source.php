<?php
/*
Source Model

Handles all TLE source management operations.
*/

// Load required classes
App::uses('Sanitize', 'Utility');

class Source extends AppModel {
    var $name = 'Source';
    
    // Define associations
    public $hasMany = array(
        'Update' => array(
            'dependent'=> true
        )
    ); 
    
    // Validations
    public $validate = array(
        'name' => array(
            'name_unique' => array(
                'rule' => 'isUnique',
                'required' => true,
                'allowEmpty' => false
            ),
            'name_alphanum' => array(
                'rule' => 'alphaNumeric',
                'message' => 'Source names may only contain letters and numbers.'
            )
        ),
        'url' => 'notEmpty'
    );
    
    public function api_loadsources($source_names = false, $timestamp = false){
        /*
        Fetches all updates and TLE's for the specified source(s) and timestamp (if set) and package it into a standard format for display.
        
        @param $source_names: And array containing the name of all sources to load.
        @param $timestamp: A UNIX timestamp to filter results.
        */
        
      	// Setup
      	$result = Array();
      	$mod_update = ClassRegistry::init('Update');
      		
      	// Decide which sources to load & assemble the query conditional
      	if ($source_names){
      	    // Construct a query out of the names
      	    $query_source_names = Array();
      	    foreach($source_names as $source_name){
      		array_push($query_source_names, 'Source.name=\''.Sanitize::escape($source_name).'\'');
      	    }
      	    $query_source_names = 'WHERE '.implode(' OR ',$query_source_names);
      	} else {
      	    // Load all sources
      	    $query_source_names = '';
      	}
      		
      	// Check if a timestamp was set
      	if ($timestamp){
      	    // Find the best update and construct the results into a standardized array (that is recognized by loop below)
      	    $sources = Array();
      	    
      	    // Load the sources first
      	    $sources_temp = $this->query('SELECT * FROM sources AS Source '.$query_source_names);
      	    foreach($sources_temp as $source_temp){
            		$temp_source = Array(
            		    'Source' => $source_temp['Source']
            		);
            					
            		// Find the update for this source that is closest to `timestamp`
            		$update_search = $mod_update->query('SELECT * FROM updates AS `Update` WHERE `Update`.source_id=\''.$source_temp['Source']['id'].'\' AND `Update`.update_message = \'\' ORDER BY ABS(`Update`.created_on - \''.Sanitize::clean($timestamp).'\') ASC LIMIT 1');
            		if (!empty($update_search)){
            		    $temp_source['Update'] = $update_search[0]['Update'];
            		    
            		    array_push($sources, $temp_source);
            		}
      	    }
      	} else {
      	    // Load the source and most recent successful update for each `source_names`
      	    $sources = $this->query('SELECT * FROM sources AS Source INNER JOIN updates AS `Update` ON (`Update`.id = Source.latest_successful_update AND `Update`.update_message = \'\') '.$query_source_names);
      	    $timestamp = time();
      	}
      		
      	if (empty($sources)){
      	    // No matching sources found
      	    $result['status']['status'] = 'error';
      	    $result['status']['message'] = 'None of the provided sources could be located or have been updated recently.';
      	    $result['status']['timestamp'] = time();
      	    $result['status']['sources_fetched'] = 0;
      	} else {
      	    // Results found, fetch the TLE's and assemble the result
      	    foreach($sources as $source){
      		      // Load the TLE's for the update
      		      $update = $mod_update->find('first', array(
      		          'conditions' => array('Update.id' => $source['Update']['id'])
      		      ));
      					
            		if (isset($update['Tle']) && !empty($update['Tle'])){
            		    // Loop through the TLE's and store them
            		    foreach($update['Tle'] as $tle){
            			      $result['sources'][$source['Source']['name']]['satellites'][$tle['name']] = $tle;
            			      unset($result['sources'][$source['Source']['name']]['satellites'][$tle['name']]['created_on']);
            			      unset($result['sources'][$source['Source']['name']]['satellites'][$tle['name']]['update_id']);
            			      unset($result['sources'][$source['Source']['name']]['satellites'][$tle['name']]['id']);
            		    }
            						
            		    // Store the source status
            		    $result['sources'][$source['Source']['name']]['status']['url'] = $source['Source']['url'];
            		    $result['sources'][$source['Source']['name']]['status']['description'] = $source['Source']['description'];
            		    $result['sources'][$source['Source']['name']]['status']['updated'] = intval($source['Update']['created_on']);
            		    $result['sources'][$source['Source']['name']]['status']['satellites_fetched'] = count($result['sources'][$source['Source']['name']]['satellites']);
            		} else {
            		    // No tle's found for the current source/update, skip it
            		    continue;
            		}
      	    }
      			
      	    // At least 1 source loaded successfully
      	    if (count($result)>=1){
      		      $result['status']['status'] = 'okay';
      		      $result['status']['message'] = 'At least one of the specified sources was loaded.';
      		      $result['status']['timestamp'] = time();
      		      $result['status']['sources_fetched'] = count($result);
      	    } else {
      		      $result['status']['status'] = 'error';
      		      $result['status']['message'] = 'None of the specified sources could be loaded.';
      		      $result['status']['timestamp'] = time();
      		      $result['status']['sources_fetched'] = 0;
      	    }
      	}
      	
      	// Add the request parameters
      	$result['status']['params']['timestamp'] = $timestamp;
      	$result['status']['params']['sources'] = array();
      	foreach($source_names as $temp_source_name){
      	    array_push($result['status']['params']['sources'], $temp_source_name);
      	}
        
        return $result;
    }
    
    public function update_sources($source_names = false){
        /*
        Update the specified (or all) sources.
        
        @param $source_names: Name(s) of the sources to update. If blank, update all sources. This must all ready be URL decoded.
        
        Returns:
            True if all sources update.
            False if at least one source fails to update.
        */
        
        // Setup the source names
	      $mod_update = Classregistry::init('Update');
	      $mod_tle = Classregistry::init('Tle');
        $sources = null;
        $success = true;
	      $this->recursive = 0;
        if (is_array($source_names)){
            // Multiple sources
            $source_conditions = array(
                'Source.name' => $source_names
            );
            $sources = $this->find('all', array('conditions' => $source_conditions));
        } else if ($source_names){
            // Single source
            $sources = Array();
            
            $temp_source = $this->find('first', array('conditions' => array('Source.name' => $source_names)));
            array_push($sources, $temp_source);
        } else {
            // Fetch all sources
            $sources = $this->find('all');
        }
		
        // Update each source
        foreach ($sources as $source){
	          $this->create();
            $temp_update_message = '';
	          $temp_source = Array();
            
            // Load the source URL
            $source_curl = curl_init();
            $timeout = 5; // Seconds
            curl_setopt($source_curl, CURLOPT_URL, $source['Source']['url']);
            curl_setopt($source_curl, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($source_curl, CURLOPT_CONNECTTIMEOUT, $timeout);
            $source_data = curl_exec($source_curl);
            curl_close($source_curl);
            
            if ($source_data){
                // Create a new update
                $mod_update->create();
                $new_update = Array();
                $new_update['Update']['source_id'] = $source['Source']['id'];
                $new_update['Update']['created_on'] = time();
                $new_update['Update']['update_message'] = '';
                $create_update_attempt = $mod_update->save($new_update);
				
                if ($create_update_attempt){
                    // Parse the response
                    $tle_parse_attempt = $mod_tle->parse_tle_source($source_data, $mod_update->id);

                    if ($tle_parse_attempt!==TRUE){
                        // Parse error occured
                        $temp_update_message = $tle_parse_attempt;
                    }
                } else {
                    // Error creating the update object, just skip this source for now
                    $success = false;
                    continue;
                }
            } else {
                // HTTP error
                $temp_update_message = 'An HTTP error occured while fetching the source. Make sure the URL still works.';
            }
            
            if (!empty($temp_update_message)){
                // An error occured, record it in the update
                $temp_update['Update']['id'] = $mod_update->id;
                $temp_update['Update']['update_message'] = $temp_update_message;
	            $temp_update['Update']['source_id'] = $source['Source']['id'];
                $temp_update['Update']['created_on'] = time();
                $mod_update->save($temp_update);
                
                CakeLog::write('source', '[error] There was an error updating the source \''.$source['Source']['name'].'\': \''.$temp_update_message.'\'');
                $success = false;
            } else {
	            $temp_source['Source']['latest_successful_update'] = $mod_update->id;
                CakeLog::write('source', '[success] The source \''.$source['Source']['name'].'\' was updated. Update ID: '.$mod_update->id);
            }
            
            // Update the `latest_update` field of the source, even if it failed
            $temp_source['Source']['id'] = $source['Source']['id'];
            $temp_source['Source']['url'] = $source['Source']['url']; // Appease the validator
            $temp_source['Source']['name'] = $source['Source']['name']; // Appease the validator
            $temp_source['Source']['latest_update'] = $mod_update->id;
            $this->save($temp_source);
        }
        
        return $success;
    }
    
    public static function arrayToXML($data){
        /*
        Convert the Source API array into an XML string.
        
	      @param $data: Array containing source data.
	
        Returns:
            An XML string representing the array.
        */
        
	      // Start XML document.
        $xml_string = '<?xml version="1.0"?>';
	      $xml_string .= '<api_sources>';
	
      	// Add the status element
      	$xml_string .= '<status>';
      	$xml_string .= '<status>'.htmlspecialchars($data['status']['status']).'</status>';
      	$xml_string .= '<message>'.htmlspecialchars($data['status']['message']).'</message>';
      	$xml_string .= '<timestamp>'.htmlspecialchars($data['status']['timestamp']).'</timestamp>';
      	$xml_string .= '<sources_fetched>'.htmlspecialchars($data['status']['sources_fetched']).'</sources_fetched>';
      	$xml_string .= '<params>';
      	    $xml_string .= '<timestamp>'.htmlspecialchars($data['status']['params']['timestamp']).'</timestamp>';
      	    $xml_string .= '<sources>';
      		    foreach ($data['status']['params']['sources'] as $temp_source){
      		        $xml_string .= '<source>'.htmlspecialchars($temp_source).'</source>';
      		    }
      	    $xml_string .= '</sources>';
      	$xml_string .= '</params>';
      	$xml_string .= '</status>';
	
      	// Add each source
      	if (isset($data['sources']) && !empty($data['sources'])){
      	    $xml_string .= '<sources>';
      	    foreach($data['sources'] as $source_name => $source){
      		      $xml_string .= '<source name="'.htmlspecialchars($source_name).'">';
      		
            		// Add the source status
            		$xml_string .= '<status>';
            		$xml_string .= '<url>'.htmlspecialchars($source['status']['url']).'</url>';
            		$xml_string .= '<description>'.htmlspecialchars($source['status']['description']).'</description>';
            		$xml_string .= '<updated>'.htmlspecialchars($source['status']['updated']).'</updated>';
            		$xml_string .= '<satellites_fetched>'.htmlspecialchars($source['status']['satellites_fetched']).'</satellites_fetched>';
            		$xml_string .= '</status>';
            		
            		// Add the source's satellites
            		if (isset($source['satellites']) && !empty($source['satellites'])){
            		    $xml_string .= '<satellites>';
            		    foreach($source['satellites'] as $satellite_name => $satellite){
                  			$xml_string .= '<satellite name="'.htmlspecialchars($satellite_name).'">';
                  			$xml_string .= '<name>'.htmlspecialchars($satellite['name']).'</name>';
                  			$xml_string .= '<satellite_number>'.htmlspecialchars($satellite['satellite_number']).'</satellite_number>';
                  			$xml_string .= '<classification>'.htmlspecialchars($satellite['classification']).'</classification>';
                  			$xml_string .= '<launch_year>'.htmlspecialchars($satellite['launch_year']).'</launch_year>';
                  			$xml_string .= '<launch_number>'.htmlspecialchars($satellite['launch_number']).'</launch_number>';
                  			$xml_string .= '<launch_piece>'.htmlspecialchars($satellite['launch_piece']).'</launch_piece>';
                  			$xml_string .= '<epoch_year>'.htmlspecialchars($satellite['epoch_year']).'</epoch_year>';
                  			$xml_string .= '<epoch>'.htmlspecialchars($satellite['epoch']).'</epoch>';
                  			$xml_string .= '<ftd_mm_d2>'.htmlspecialchars($satellite['ftd_mm_d2']).'</ftd_mm_d2>';
                  			$xml_string .= '<std_mm_d6>'.htmlspecialchars($satellite['std_mm_d6']).'</std_mm_d6>';
                  			$xml_string .= '<bstar_drag>'.htmlspecialchars($satellite['bstar_drag']).'</bstar_drag>';
                  			$xml_string .= '<element_number>'.htmlspecialchars($satellite['element_number']).'</element_number>';
                  			$xml_string .= '<checksum_l1>'.htmlspecialchars($satellite['checksum_l1']).'</checksum_l1>';
                  			$xml_string .= '<inclination>'.htmlspecialchars($satellite['inclination']).'</inclination>';
                  			$xml_string .= '<right_ascension>'.htmlspecialchars($satellite['right_ascension']).'</right_ascension>';
                  			$xml_string .= '<eccentricity>'.htmlspecialchars($satellite['eccentricity']).'</eccentricity>';
                  			$xml_string .= '<perigee>'.htmlspecialchars($satellite['perigee']).'</perigee>';
                  			$xml_string .= '<mean_anomaly>'.htmlspecialchars($satellite['mean_anomaly']).'</mean_anomaly>';
                  			$xml_string .= '<mean_motion>'.htmlspecialchars($satellite['mean_motion']).'</mean_motion>';
                  			$xml_string .= '<revs>'.htmlspecialchars($satellite['revs']).'</revs>';
                  			$xml_string .= '<checksum_l2>'.htmlspecialchars($satellite['checksum_l2']).'</checksum_l2>';
                  			$xml_string .= '<raw_l1>'.$satellite['raw_l1'].'</raw_l1>';
                  			$xml_string .= '<raw_l2>'.$satellite['raw_l2'].'</raw_l2>';
                  			$xml_string .= '</satellite>';
            		    }
            		    $xml_string .= '</satellites>';
            		}
            			
            		$xml_string .= '</source>';
            }
            
            $xml_string .= '</sources>';
      	}
      	
      	// Close XML
      	$xml_string .= '</api_sources>';
      	
      	return $xml_string;
    }
    
    public function arrayToRaw($sources){
      	/*
      	Converts the provided array to an array containing RAW tle elements for display. If there are duplicates, the most recent one is used.
      	
      	@param $sources: Array containing sources & tle information.
      	
      	Returns:
      	    An array containing the raw TLE information.
      	*/
      	
      	$raw_tles = Array();
      	
      	// Loop through each source and store the raw TLE information
      	if (isset($sources['sources']) && !empty($sources['sources'])){
      	    foreach ($sources['sources'] as $source){
            		$update_timestamp = $source['status']['updated'];
            		if (isset($source['satellites']) && !empty($source['satellites'])){
            		    // Loop through the satellites and store the TLE's
            		    foreach ($source['satellites'] as $satellite_name => $satellite){
                  			// Check for duplicates 
                  			$add_satellite = true;
                  			if (isset($raw_tles[$satellite_name])){
                  			    // Check  the timestamp
                  			    if ($raw_tles[$satellite_name]['timestamp'] > $update_timestamp){
                        				// Existing is newer, use it
                        				$add_satellite = false; 
                  			    }
                  			}
                  			
                  			if ($add_satellite){
                  			    $temp_satellite = Array(
                        				'timestamp' => $update_timestamp,
                        				'name' => $satellite_name,
                        				'raw_l1' => $satellite['raw_l1'],
                        				'raw_l2' => $satellite['raw_l2']
                  			    );
                  						
                  			    $raw_tles[$satellite_name] = $temp_satellite;
                  			}
            		    }
            		}
      	    }
      	}
	
	      return $raw_tles;
    }
}
?>
