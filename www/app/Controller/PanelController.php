<?php
/*
This controller is responsible for fetchTLE administrator functionality.
*/

class PanelController extends AppController {
    var $uses = array('Source', 'Tle', 'Update'); 
    var $components = array('RequestHandler');
    
    function beforeFilter(){
        parent::beforeFilter();
        
        // Let the user access the login page
        $this->Auth->allow('admin_login', 'admin_makehash', 'admin_generatehash'); 
    }
    
    public function admin_index() {
        /*
        Displays the main administrator menu.
        */
        
        // Load all updates that have errors
		$this->Update->recursive = 0; // If you fetch all of the tle's at once you're gonna have a bad time
        $options['joins'] = array(
            array('table' => 'sources',
                'alias' => 'SourceTemp',
                'type' => 'INNER',
                'conditions' => array(
                    'SourceTemp.latest_update = Update.id'
                )
            )
        );
        $options['conditions'] = array(
            'Update.update_message !=' => ''
        );
		
        $this->set('error_updates', $this->Update->find('all', $options));
    }
    
    public function admin_generatehash(){
        /*
        Generates a password hash for the posted password.
        */
        
        $password = (isset($_POST['password']))?$_POST['password']:'';
        $this->layout = 'ajax';
        
        if (isset($password)&&!empty($password)){
            // Generate the password hash
            $password_hash = $this->Auth->password($password);
            $this->set('hash_response', $password_hash);
        } else {
            // No password specified
            $this->set('hash_response', 'No password was specifed. Please try again.');
        }
    }
    
    public function admin_makehash(){
        /*
        Displays the form to generate a password hash.
        */
    }
    
    public function admin_tleupdate(){
        /*
        Processes the TLE source file and stores each entry in the database.
        
        Accessed via Ajax. Outputs 'okay' on success and 'error' on error.
        */
        
        // Attempt to update the TLE's
        $update_status = $this->Tle->updateTles();
        
        // Log the update
        if ($update_status){
            CakeLog::write('tles', '[success] TLE\'s successfully updated.');
        } else {
            CakeLog::write('tles', '[error] There was an error updating the TLE\'s.');
        }
        
        $this->set('update_status', $update_status);
    }
    
    public function admin_login(){
        /*
        Process administrator login requests.
        */
        
		if ($this->Auth->user()){
			$this->redirect(array('controller' => 'panel', 'action' => 'index', 'admin' => true));
		} else {
			if ($this->request->is('post')) {
				// Store user credentials
				$this->data['Admin']['username'] = $_POST['username'];
				$this->data['Admin']['password'] = $_POST['password'];
				
				if ($this->Auth->login($this->data)) {
					$this->Session->setFlash('You have been logged in. Welcome back.', 'default', array('class' => 'alert alert-success'));
					$this->redirect($this->Auth->redirect());
				} else {
					$this->Session->setFlash('Invalid username or password. Please try again.', 'default', array('class' => 'alert alert-error'));
				}
			}
		}
    }

    public function admin_logout(){
        /*
        Process logout requests.
        */
        
        $this->redirect($this->Auth->logout());
    }
}
?>
