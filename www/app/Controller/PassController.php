<?php
/*
This controller is responsible for calculating and displaying pass times for specified satellites over ground stations.
*/

class PassController extends AppController {
    var $uses = array('Tle', 'Update', 'Configuration', 'Station'); 
    var $components = array('RequestHandler');
		
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow('tools_index'); 
    }
    
    public function tools_index(){
        /*
        Displays the main panel for the pass times tool that allows users to view pass times over satellites.
        */
    }
    
    public function admin_index(){
        /*
        Allows administrators to configure the pass time tool.
        */
        
        $this->set("title_for_layout","Manage Pass Times Tool");
        
        // Load and set the configuration options
        $configurations = $this->Configuration->find('all', array(
            'conditions' => array(
                'Configuration.name' => array('passes_default_min_el', 'passes_available_ground_stations', 'passes_default_pass_count', 'passes_max_pass_count')
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
            
            // Package the default ground stations
            $ground_station_string = implode(',', $_POST['available_ground_stations']);
            $temp_field = array(
                'id' => $_POST['available_ground_stations_id'],
                'value' => $ground_station_string
            );
            array_push($new_configuration, $temp_field);
            
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
