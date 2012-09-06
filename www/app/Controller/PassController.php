<?php
/*
This controller is responsible for calculating and displaying pass times for specified satellites over ground stations.
*/

class PassController extends AppController {
    var $uses = array('Tle', 'Update', 'Configuration', 'Station', 'Source'); 
    var $components = array('RequestHandler');
		
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow('tools_index', 'tools_calculator', 'api_passes'); 
    }
    
    public function tools_index(){
        /*
        Displays the main satellite selection menu for the pass time calculation utility.
        */
        
        // Load all sources
        $this->Source->hasMany['Update']['limit'] = 1;
        $this->Source->hasMany['Update']['order'] = 'id DESC';
        $this->Source->recursive = 2;
        $this->set('sources', $this->Source->find('all'));
        
        $this->set("title_for_layout", "Pass Time Satellite Selection");
    }
    
    public function tools_calculator(){
        /*
        Displays the main pass time calculation tool for an individual satellite.
        */
        
        // Collect the parameters
        $satellite_name = $this->request->params['satellite'];
        
        // Verify the satellite
        $satellite = $this->Tle->tools_loadsatellites(Array($satellite_name));
        if ($satellite){
            // Satellite loaded
            $this->set('satellite', $satellite);
            
            // Load the ground stations
            $station_id_fetch = $this->Configuration->find('first', array(
                'conditions' => array(
                    'Configuration.name' => 'passes_available_ground_stations'
                )
            ));
            if ($station_id_fetch['Configuration']['value']){
                // Load the indicated ground stations
                $active_station_ids = explode(',', $station_id_fetch['Configuration']['value']);
                $ground_stations = $this->Station->find('all', array(
                    'conditions' => array(
                        'Station.id' => $active_station_ids
                    )
                ));
                if (!empty($ground_stations)){
                    // Ground stations loaded
                    $this->set('ground_stations', $ground_stations);
                    
                    // Load the configurations
                    $configurations = $this->Configuration->find('all', array(
                        'conditions' => array(
                            'Configuration.name' => array('passes_default_min_el', 'passes_default_pass_count', 'passes_max_pass_count')
                        )
                    ));
                    foreach($configurations as $configuration){
                        $this->set($configuration['Configuration']['name'], $configuration);
                    }
                } else {
                    // Ground stations not found
                    $this->set('ground_stations', false);
                }
            } else {
                // No ground stations enabled
                $this->set('ground_stations', false);
            }
        } else {
            // Satellite could not be found
            $this->Session->setFlash('That satellite could not be found. Please select one from the lists below.', 'default', array('class' => 'alert alert-error'));
            $this->redirect(array('controller' => 'pass', 'action' => 'index', 'tools' => true));
        }
    }
    
    public function api_passes(){
        /*
        Provides access to the pass time calculator API.
        */
        
        // Setup
        $pass_times = null;
        
        // Collect the parameters
        $minelevation = (isset($_GET['minelevation']))?$_GET['minelevation']:false;
        $passcount = (isset($_GET['passcount']))?$_GET['passcount']:false;
        $ground_stations = (isset($_GET['ground_stations']))?$_GET['ground_stations']:false;
        $timestamp = (isset($_GET['timestamp']))?$_GET['timestamp']:false;
        
        // Submit the request
        if (isset($this->request->params['satellite'])){
            // Parse the ground station names
            $satellite_name = $this->request->params['satellite'];
            $ground_station_names = ($ground_stations)?explode('_', $ground_stations):false;
            $pass_times = $this->Station->api_loadpasses($satellite_name, $ground_station_names, $minelevation, $passcount, $timestamp);
            
            if (isset($this->request->params['ext']) && $this->request->params['ext']=='xml'){
                // Convert the array to an XML string
                $this->set('passes', $this->Station->arrayToXMLPasses($pass_times));
            } else if (isset($this->request->params['ext']) && $this->request->params['ext']=='json'){
                // $sources is encoded to JSON in the view using json_encode
                $this->set('passes', $pass_times);
				$callback = (isset($_GET['callback'])&&!empty($_GET['callback']))?$_GET['callback']:false;
				$this->set('callback', $callback);
            } else {
                // Convert the response to RAW tle output and display
                $this->layout = 'ajax';
                $this->set('passes', $this->Station->arrayToRawPasses($pass_times));
            }
        }
    }
    
    public function admin_index(){
        /*
        Allows administrators to configure the pass time tool.
        */
        
        $this->set("title_for_layout","Manage Pass Times Tool");
        
        // Load and set the configuration options
        $configurations = $this->Configuration->find('all', array(
            'conditions' => array(
                'Configuration.name' => array('passes_default_min_el', 'passes_available_ground_stations', 'passes_default_ground_stations', 'passes_default_pass_count', 'passes_max_pass_count')
            )
        ));
        foreach($configurations as $configuration){
            $this->set($configuration['Configuration']['name'], $configuration);
        }
        
        // Load all of the available ground stations
        $this->set('ground_stations', $this->Station->find('all'));
    }
    
    public function admin_update(){
        /*
        Updates the pass tool configuration.
        */
        
        if ($this->request->is('post')){
            // Loop through the configuration fields
            $new_configuration = array();
            foreach($_POST['config'] as $field_id => $field_value){
                $temp_field = array(
                    'id' => $field_id,
                    'value' => $field_value
                );
                
                array_push($new_configuration, $temp_field);
            }
            
            // Package the available ground stations
            if (isset($_POST['available_ground_stations'])){
                $ground_station_string = implode(',', $_POST['available_ground_stations']);
                $temp_field = array(
                    'id' => $_POST['available_ground_stations_id'],
                    'value' => $ground_station_string
                );
                array_push($new_configuration, $temp_field);
            }
            
            // Package the default ground stations
            if (isset($_POST['default_ground_stations'])){
                $ground_station_string = implode(',', $_POST['default_ground_stations']);
                $temp_field = array(
                    'id' => $_POST['default_ground_stations_id'],
                    'value' => $ground_station_string
                );
                array_push($new_configuration, $temp_field);
            }
            
            // Save the configuration
            $save_attempt = $this->Configuration->saveMany($new_configuration);
            
            if ($save_attempt){
                $this->Session->setFlash('The pass tool configuration has been updated.', 'default', array('class' => 'alert alert-success'));
                CakeLog::write('admin', '[success] Pass tool configuration updated.');
                $this->redirect(array('controller' => 'pass', 'action' => 'index', 'admin' => true));
            } else {
                $this->Session->setFlash('There was an error updating the pass tool configuration.', 'default', array('class' => 'alert alert-error'));
                CakeLog::write('admin', '[error] Error saving the pass tool configuration, please try again.');
                $this->redirect(array('controller' => 'panel', 'action' => 'index'));
            }
        } else {
            // Redirect them to the index
            $this->redirect(array('controller' => 'panel', 'action' => 'index'));
        }
    }
}
?>
