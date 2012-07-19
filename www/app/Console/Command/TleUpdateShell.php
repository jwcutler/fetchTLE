<?php
/*
TleUpdateShell

This shell updates the specified source(s) via console/cron access.
*/

class TleUpdateShell extends AppShell {
    // Setup models
    public $uses = array('Source');
    
    public function main() {
        /*
        Just display a quick readme. The main function can't receive parameters.
        */
        
        $this->out('TleUpdateShell\n\nThis script allows users to update TLE sources via a console or CRON tab.');
    }
    
    public function update(){
        /*
        Parses the provided arguments and connects to the Source model to update the specified TLE source(s).
        */
        
        // Setup
        $update_attempt = null;
        $this->stdout->styles('success', array('text' => 'green'));
        
        $this->out('Updating TLE sources...');
        
        // Grab the sources
        if (empty($this->args)){
            // Update all sources
            $update_attempt = $this->Source->update_sources();
        } else {
            // Assemble the specified sources into an array
            $sources_to_update = Array();
            foreach ($this->args as $source_name){
                array_push($sources_to_update, $source_name);
            }
            
            // Update the specified sources
            $update_attempt = $this->Source->update_sources($sources_to_update);
        }
        
        if ($update_attempt){
            $this->out('<success>All specified sources were successfully updated without error.</success>');
        } else {
            $this->out('<warning>At least one source failed to update. For more information, see the admin panel.</warning>');
        }
    }
}
?>
