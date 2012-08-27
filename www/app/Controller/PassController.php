<?php
/*
This controller is responsible for calculating and displaying pass times for specified satellites over ground stations.
*/

class PassController extends AppController {
    var $uses = array('Tle', 'Update'); 
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
}
?>
