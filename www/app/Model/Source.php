<?php
/*
Source Model

Handles all TLE source management operations.
*/

class Source extends AppModel {
    var $name = 'Source';
    
    // Define associations
    public $hasMany = 'Update';
    
    // Validations
    public $validate = array(
        'name' => array(
            'name_unique' => array(
                'rule' => 'isUnique',
                'required' => true,
                'allowEmpty' => false
            )
        ),
        'url' => 'notEmpty'
    );
    
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
