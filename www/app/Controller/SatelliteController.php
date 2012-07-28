<?php
/*
This controller is primarily reponsible for fetching and displaying satellites for the satellites API.
*/

class SatelliteController extends AppController {
    var $uses = array('Source', 'Tle');
    var $components = array('RequestHandler');
    
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow('api_satellites'); 
    }
    
    public function api_satellites(){
        /*
        Returns the TLE information for the specified satellite(s).
        */
        
        // Load the required satellites
        $satellites = null;
        $timestamp = (isset($_GET['timestamp'])&&is_numeric($_GET['timestamp']))?$_GET['timestamp']:false;
        if (isset($this->request->params['satellites'])){
            // Load the specified sources
            $satellite_names = explode('_', $this->request->params['satellites']);
            $satellites = $this->Tle->api_loadsatellites($satellite_names, $timestamp);
            
            if (isset($this->request->params['ext']) && $this->request->params['ext']=='xml'){
                // Convert the array to an XML string
                $this->set('satellites', $this->Tle->arrayToXML($satellites));
            } else if (isset($this->request->params['ext']) && $this->request->params['ext']=='json'){
                // $sources is encoded to JSON in the view using json_encode
                $this->set('satellites', $satellites);
				$callback = (isset($_GET['callback'])&&!empty($_GET['callback']))?$_GET['callback']:false;
				$this->set('callback', $callback);
            } else {
                // Convert the response to RAW tle output and display
                $this->layout = 'ajax';
                $this->set('satellites', $this->Tle->arrayToRaw($satellites));
            }
        }
    }
}
?>
