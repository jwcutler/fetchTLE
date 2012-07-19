<?php
/*
TLE Model

This model handles all TLE management and interaction options.
*/

class Tle extends AppModel {
    var $name = 'Tle';
    
    // Define associations
    public $belongsTo = 'Update';
    
    public function parse_tle_source($raw_tle_data, $update_id){
        /*
        Parses the provided raw TLE source.
        
        @param $raw_tle_data: The raw TLE file from the source.
        @param $update_id: The ID of the parent update.
        
        Returns:
            True on success
            Human readable error message on failure
        */
        
        // Make sure some data is present
        if (!empty($raw_tle_data)){
            // Attempt to read in the TLE source
            $tle_line_counter = 0;
            $tle_entry_counter = 0;
            $new_tle_entries = array();
            $valid_tle = true;
            $tle_lines = explode("\n", $raw_tle_data);
            foreach ($tle_lines as $tle_line){
                // Skip empty lines
                if (empty($tle_line)){
                    continue;
                }
                
                // Check if the entry needs to be saved
                if ($tle_line_counter==3){
                    $tle_line_counter = 0;
                    $tle_entry_counter++;
                }
                
                // Add a new TLE entry array if needed
                if ($tle_line_counter == 0){
                    array_push($new_tle_entries, array());
                }
                
                // Parse the first line
                if ($tle_line_counter == 0){
                    $new_tle_entries[$tle_entry_counter]['name'] = trim($tle_line);
                    $new_tle_entries[$tle_entry_counter]['update_id'] = $update_id;
                    $new_tle_entries[$tle_entry_counter]['created_on'] = date('Y-m-d H:i:s', time());
                }
                
                // Parse the second line
                if ($tle_line_counter == 1){
                    // Make sure the TLE line is the right length
                    if (strlen(trim($tle_line))!=69){
                        $valid_tle = false;
                        break;
                    }
                    
                    $new_tle_entries[$tle_entry_counter]['satellite_number'] = trim(substr($tle_line, 2, 5));
                    $new_tle_entries[$tle_entry_counter]['classification'] = trim(substr($tle_line, 7, 1));
                    $new_tle_entries[$tle_entry_counter]['launch_year'] = trim(substr($tle_line, 9, 2));
                    $new_tle_entries[$tle_entry_counter]['launch_number'] = trim(substr($tle_line, 11, 3));
                    $new_tle_entries[$tle_entry_counter]['launch_piece'] = trim(substr($tle_line, 14, 1));
                    $new_tle_entries[$tle_entry_counter]['epoch_year'] = trim(substr($tle_line, 18, 2));
                    $new_tle_entries[$tle_entry_counter]['epoch'] = trim(substr($tle_line, 20, 12));
                    $new_tle_entries[$tle_entry_counter]['ftd_mm_d2'] = trim(substr($tle_line, 33, 12));
                    $new_tle_entries[$tle_entry_counter]['std_mm_d6'] = trim(substr($tle_line, 45, 7));
                    $new_tle_entries[$tle_entry_counter]['bstar_drag'] = trim(substr($tle_line, 53, 8));
                    $new_tle_entries[$tle_entry_counter]['element_number'] = trim(substr($tle_line, 65, 3));
                    $new_tle_entries[$tle_entry_counter]['checksum_l1'] = trim(substr($tle_line, 68, 1));
                    $new_tle_entries[$tle_entry_counter]['raw_l1'] = trim($tle_line);
                }
                
                // Parse the third line
                if ($tle_line_counter == 2){
                    // Make sure the TLE line is the right length
                    if (strlen(trim($tle_line))!=69){
                        $valid_tle = false;
                        break;
                    }
                    
                    $new_tle_entries[$tle_entry_counter]['inclination'] = trim(substr($tle_line, 8, 8));
                    $new_tle_entries[$tle_entry_counter]['right_ascension'] = trim(substr($tle_line, 17, 8));
                    $new_tle_entries[$tle_entry_counter]['eccentricity'] = trim(substr($tle_line, 26, 7));
                    $new_tle_entries[$tle_entry_counter]['perigee'] = trim(substr($tle_line, 34, 8));
                    $new_tle_entries[$tle_entry_counter]['mean_anomaly'] = trim(substr($tle_line, 43, 8));
                    $new_tle_entries[$tle_entry_counter]['mean_motion'] = trim(substr($tle_line, 52, 11));
                    $new_tle_entries[$tle_entry_counter]['revs'] = trim(substr($tle_line, 63, 5));
                    $new_tle_entries[$tle_entry_counter]['checksum_l2'] = trim(substr($tle_line, 68, 1));
                    $new_tle_entries[$tle_entry_counter]['raw_l2'] = trim($tle_line);
                }
                
                $tle_line_counter++;
            }
            
            if ($valid_tle){
                // Add the TLE's
                $save_result = $this->saveMany($new_tle_entries, array('deep' => true));
                
                if ($save_result){
                    // Success
                    return true;
                } else {
                    // Error saving TLE's
                    return 'There was an error saving the TLE\'s from the source.';
                }
            } else {
                // Invalid TLE file
                return 'The TLE source was invalid (not a valid TLE file).';
            }
        } else {
            // Missing data
            return 'The TLE source was empty.';
        }
    }
}
?>
