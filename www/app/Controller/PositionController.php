<?php
/*
This controller is primarily reponsible for fetching and displaying satellite position information for the satellite position API.
*/

class PositionController extends AppController {
    var $uses = array('Source', 'Tle');
    var $components = array('RequestHandler');
	
	// Setup Caching
	public $helpers = array('Cache');
    public $cacheAction = array(
		'api_positions' => 1800 // Cache API for 30 minutes (1800 seconds)
	);
    
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow('api_positions'); 
    }
    
    public function api_positions(){
        /*
        Returns the position information for the specified satellite(s).
        */
        
        // Load the required satellites
        $satellites = null;
        $start = (isset($_GET['start'])&&is_numeric($_GET['start']))?$_GET['start']:false;
        $end = (isset($_GET['end'])&&is_numeric($_GET['end']))?$_GET['end']:false;
        $resolution = (isset($_GET['resolution'])&&is_numeric($_GET['resolution']))?$_GET['resolution']:false;
        if (isset($this->request->params['satellites'])){
            // Load the specified sources
            $satellite_names = explode('_', $this->request->params['satellites']);
            $satellites = $this->Tle->api_loadpositions($satellite_names, $start, $end, $resolution);
            
            if (isset($this->request->params['ext']) && $this->request->params['ext']=='xml'){
                // Convert the array to an XML string
                $this->set('satellites', $this->Tle->arrayToXMLPositions($satellites));
            } else if (isset($this->request->params['ext']) && $this->request->params['ext']=='json'){
                // $sources is encoded to JSON in the view using json_encode
                $this->set('satellites', $satellites);
				$callback = (isset($_GET['callback'])&&!empty($_GET['callback']))?$_GET['callback']:false;
				$this->set('callback', $callback);
            } else {
                // Convert the response to RAW tle output and display
                $this->layout = 'ajax';
                $this->set('satellites', $this->Tle->arrayToRawPositions($satellites));
            }
        }
    }
}
?>
