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
        Fetches all updates and TLE's for the specified source(s) and timestamp (if set).
        
        @param $source_names: And array containing the name of all sources to load.
        @param $timestamp: A UNIX timestamp to filter results.
        */
        
        // Load the sources
        $result = Array();
        if ($source_names){
            if ($timestamp){
                // Load the updates for `source_names` that are closest to `timestamp`
            } else {
                // Load the source and most recent successful update for each `source_names`
                $query_source_names = Array();
                foreach($source_names as $source_name){
                    array_push($query_source_names, 'Source.name=\''.Sanitize::clean($source_name).'\'');
                }
                $query_source_names = implode(' OR ',$query_source_names);
                $sources = $this->query('SELECT * FROM sources AS Source INNER JOIN updates AS `Update` ON (`Update`.id = Source.latest_successful_update) WHERE '.$query_source_names);
            }
            
            if (empty($sources)){
                // No matching sources found
                $result['status']['status'] = 'error';
                $result['status']['message'] = 'None of the provided sources could be located.';
                $result['status']['timestamp'] = time();
                $result['status']['number_fetched'] = 0;
            } else {
                // Results found, fetch the TLE's and assemble the result
                foreach($sources as $source){
                    // Load the TLE's for the update
                    $mod_update = ClassRegistry::init('Update');
                    $update = $mod_update->find('first', array(
                        'conditions' => array('Update.id' => '500')
                    ));
                    
                    if (isset($update['Tle']) && !empty($update['Tle'])){
                        // Loop through the TLE's and store them
                        foreach($update['Tle'] as $tle){
                            $result[$source['Source']['name']]['satellites'][$tle['name']] = $tle;
                            unset($result[$source['Source']['name']]['satellites'][$tle['name']]['created_on']);
                            unset($result[$source['Source']['name']]['satellites'][$tle['name']]['update_id']);
                            unset($result[$source['Source']['name']]['satellites'][$tle['name']]['id']);
                        }
                        
                        // Store the source status
                        $result[$source['Source']['name']]['status']['url'] = $source['Source']['url'];
                        $result[$source['Source']['name']]['status']['description'] = $source['Source']['description'];
                        $result[$source['Source']['name']]['status']['updated'] = strtotime($source['Update']['created_on']);
                        $result[$source['Source']['name']]['status']['satellites_fetched'] = count($result[$source['Source']['name']]['satellites']);
                    } else {
                        // No tle's found for the current source/update, skip it
                        continue;
                    }
                }
                
                // At least 1 source loaded successfully
                if ($count($result)>=1)
                    $result['status']['status'] = 'okay';
                    $result['status']['message'] = 'At least one of the specified sources was loaded.';
                    $result['status']['timestamp'] = time();
                    $result['status']['sources_fetched'] = count($result)-1;
                } else {
                    $result['status']['status'] = 'error';
                    $result['status']['message'] = 'None of the specified sources could be loaded.';
                    $result['status']['timestamp'] = time();
                    $result['status']['sources_fetched'] = 0;
                }
            }
            
            var_dump($result);
        } else {
            
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
                $new_update['Update']['created_on'] = date('Y-m-d H:i:s', time());
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
}
?>
