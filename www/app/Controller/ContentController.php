<?php
/*
This controller is used to load and render static content.
*/

class ContentController extends AppController {
	var $uses = array('Source', 'Tle');

    function beforeFilter(){
        parent::beforeFilter();
        
        // Let the user access the login page
        $this->Auth->allow('documentation'); 
    }
	
    public function documentation(){
		/*
		This action displays the primary fetchTLE documentation.
		*/
		
		// Load the sources
		$this->Source->hasMany['Update']['limit'] = 1;
		$this->Source->hasMany['Update']['order'] = 'id DESC';
		$this->Source->recursive = 2;
		$this->set('sources', $this->Source->find('all'));
		
		$this->set("title_for_layout", "fetchTLE Documentation");
    }
}
?>
