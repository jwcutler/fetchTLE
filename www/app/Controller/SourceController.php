<?php
/*
This controller is responsible for fetchTLE source management and display.
*/

class SourceController extends AppController {
    var $uses = array('Source', 'Tle', 'Update'); 
    var $components = array('RequestHandler');
    
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow('cron_update', 'api_sources'); 
    }
    
    public function api_sources(){
        /*
        Returns all of the satellites and source information for the specified source(s).
        */
        
        // Load the required sources
        $sources = null;
        $timestamp = (isset($_GET['timestamp'])&&is_numeric($_GET['timestamp']))?$_GET['timestamp']:false;
        if (isset($this->request->params['sources'])){
            // Load the specified sources
            $source_names = explode('_', $this->request->params['sources']);
            $sources = $this->Source->api_loadsources($source_names, $timestamp);
        } else {
            // Load all sources
            $sources = $this->Source->api_loadsources(false, $timestamp);
        }
		
		if (isset($this->request->params['ext']) && $this->request->params['ext']=='xml'){
			// Convert the array to an XML string
            $this->set('sources', $this->Source->arrayToXML($sources));
		} else if (isset($this->request->params['ext']) && $this->request->params['ext']=='json'){
            // $sources is encoded to JSON in the view using json_encode
			$this->set('sources', $sources);
			$callback = (isset($_GET['callback'])&&!empty($_GET['callback']))?$_GET['callback']:false;
			$this->set('callback', $callback);
		} else {
            // Convert the response to RAW tle output and display
            $this->layout = 'ajax';
            $this->set('sources', $this->Source->arrayToRaw($sources));
        }
    }
    
    public function admin_sourceupdate(){
        /*
        Update the specified source(s) when called from the admin panel.
        
        Accessed via Ajax. Outputs 'okay' on success or 'error' on error.
        */
        
        // Setup
        $update_attempt = null;
        $this->layout = 'ajax';
        
        if (isset($this->params->source) && !empty($this->params->source)){
            // Update the specified source
            $update_attempt = $this->Source->update_sources(urldecode($this->params->source));
        } else {
            // Update all sources
            $update_attempt = $this->Source->update_sources();
        }
        
        if ($update_attempt){
            $this->Session->setFlash('All specified sources were successfully updated.', 'default', array('class' => 'alert alert-success'));
            $this->set('update_status', 'okay');
        } else {
            $this->Session->setFlash('At least one source failed to update. The error will be displayed below.', 'default', array('class' => 'alert'));
            $this->set('update_status', 'error');
        }
    }
    
    public function admin_index(){
        /*
        Display the main index of sources.
        */
        
        // Load all configured sources
        $this->set('sources', $this->Source->find('all'));
    }
    
    public function admin_edit(){
        /*
        Displayes the source edit page.
        */
        
        $this->set("title_for_layout", "Edit a TLE Source");
        
        // Load the source
		$update_limit = 10;
		$this->set('update_limit', $update_limit);
		$this->Source->hasMany['Update']['limit'] = $update_limit;
		$this->Source->hasMany['Update']['order'] = 'Update.created_on DESC';
        $source = $this->Source->find('first', array('conditions' => array('Source.id' => $this->params->id)));
        if($source){
            $this->set('source', $source);
        } else {
            $this->Session->setFlash('That source could not be found.', 'default', array('class' => 'alert alert-error'));
            $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
        }
		
		// Load the most recent TLE's
		$latest_tles = $this->Update->find('first', array(
			'conditions' => array('Update.id' => $source['Source']['latest_successful_update'])
		));
		$this->set('latest_tles', $latest_tles);
    }
    
    public function admin_change(){
        /*
        Handles form submissions from admin_edit().
        
        Method: POST
        */
        
        // Load the source
        $source = $this->Source->find('first', array('conditions' => array('Source.id' => $this->params->id)));
        if($source){
            // Edit the source
            $source_changes['Source']['name'] = $_POST['source_name'];
            $source_changes['Source']['url'] = $_POST['source_url'];
            $source_changes['Source']['description'] = $_POST['source_description'];
            $source_changes['Source']['updated_on'] = date('Y-m-d H:i:s', time());
            $source_changes['Source']['id'] = $this->params->id;
            
            $edit_attempt = $this->Source->save($source_changes);
            if ($edit_attempt){
                $this->Session->setFlash('Source \''.$source['Source']['name'].'\' successfully edited', 'default', array('class' => 'alert alert-success'));
                CakeLog::write('admin', '[success] Source \''.$source['Source']['name'].'\' edited.');
                $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
            } else {
                $this->Session->setFlash('There was an error editing the \''.$source['Source']['name'].'\' TLE souce.', 'default', array('class' => 'alert alert-error'));
                CakeLog::write('admin', '[success] Source \''.$satellite['Satellite']['name'].'\' could not be edited.');
                $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
            }
        } else {
            $this->Session->setFlash('That source could not be found.', 'default', array('class' => 'alert alert-error'));
            $this->redirect(array('controller' => 'satellite', 'action' => 'index', 'admin' => true));
        }
    }
    
    public function admin_remove(){
        /*
        Displays the source remove confirmation.
        */
        
        $this->set("title_for_layout","Remove a TLE Source");
        
        // Load the source
        $source = $this->Source->find('first', array('conditions' => array('Source.id' => $this->params->id)));
        if($source){
            $this->set('source', $source);
        } else {
            $this->Session->setFlash('That source could not be found.', 'default', array('class' => 'alert alert-error'));
            $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
        }
    }
    
    public function admin_delete(){
        /*
        Handle form submissions from admin_remove().
        
        Method: POST
        */
        
        // Load the source
        $source = $this->Source->find('first', array('conditions' => array('Source.id' => $this->params->id)));
        if($source){
            // Delete the source
            $delete_attempt = $this->Source->delete($this->params->id);
            if ($delete_attempt){
                $this->Session->setFlash('Source \''.$source['Source']['name'].'\' successfully deleted.', 'default', array('class' => 'alert alert-success'));
                CakeLog::write('admin', '[success] Source \''.$source['Source']['name'].'\' deleted.');
                $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
            } else {
                $this->Session->setFlash('There was an error deleting the \''.$source['Source']['name'].'\' TLE source.', 'default', array('class' => 'alert alert-error'));
                CakeLog::write('admin', '[success] Source \''.$source['Source']['name'].'\' could not be deleted.');
                $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
            }
        } else {
            $this->Session->setFlash('That source could not be found.', 'default', array('class' => 'alert alert-error'));
            $this->redirect(array('controller' => 'source', 'action' => 'index', 'admin' => true));
        }
    }
    
    public function admin_add(){
        /*
        Display the source add page.
        */
        
        $this->set("title_for_layout","Create a TLE Source");
    }
    
    public function admin_create(){
        /*
        Process the form submission from admin_add()
        */
        
        if ($this->request->is('post')) {
            // Make sure a source with this name doesn't exist
            $name_check = $this->Source->find('first', array(
                'conditions' => array('Source.name' => $_POST['source_name'])
            ));
            
            if ($name_check){
                // Source exists all ready
                $this->Session->setFlash('A source with that name all ready exists. Source names must be unique.', 'default', array('class' => 'alert alert-error'));
                $this->redirect(array('controller' => 'source', 'action' => 'add'));
            } else {
                // Source doesn't exist, try to create it
                $new_source['Source']['name'] = $_POST['source_name'];
                $new_source['Source']['url'] = $_POST['source_url'];
                $new_source['Source']['description'] = $_POST['source_description'];
                $new_source['Source']['created_on'] = date('Y-m-d H:i:s', time());
                $new_source['Source']['updated_on'] = date('Y-m-d H:i:s', time());
                
                $save_source = $this->Source->save($new_source);
                
                if ($save_source){
                    $this->Session->setFlash('The source has been added successfully.', 'default', array('class' => 'alert alert-success'));
                    CakeLog::write('admin', '[success] New source \''.$_POST['source_name'].'\' added.');
                } else {
                    $this->Session->setFlash('An error occured while adding that source. Please try again.', 'default', array('class' => 'alert alert-error'));
                    CakeLog::write('admin', '[error] Error adding source \''.$_POST['source_name'].'\' added.');
                }
                
                $this->redirect(array('controller' => 'source', 'action' => 'index'));
            }
        } else {
            // Redirect them to the add form
            $this->redirect(array('controller' => 'source', 'action' => 'add'));
        }
    }
}
?>
