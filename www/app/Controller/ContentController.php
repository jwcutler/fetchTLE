<?php
/*
This controller is used to load and render static content.
*/

class ContentController extends AppController {
	function beforeFilter(){
        parent::beforeFilter();
        
        // Let the user access the login page
        $this->Auth->allow('documentation'); 
    }
	
	public function documentation(){
		/*
		This action displays the primary fetchTLE documentation.
		*/
	}
}
?>