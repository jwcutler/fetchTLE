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
        
$this->out('NAME:
    TleUpdate - fetchTLE Console TLE update utility.

SYNOPSIS:
    app/Console/cake TleUpdate update [source names]

DESCRIPTION:
    TleUpdate is a console utility that allows users to update individual or all TLE sources via the console or a CRON tab.

EXAMPLES:
    app/Console/cake TleUpdate update
        Updates all configured TLE sources.
    app/Console/cake TleUpdate update CUBESAT GPS
        Updates the CUBESAT and GPS sources.
');
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
