<?php
/*
Update Model

This model keeps track of updates and the TLE's parsed in each update. It is used to query TLE's by date.
*/

class Update extends AppModel {
    // Define Associations
    var $name = 'Update';
    public $belongsTo = 'Source';
    public $hasMany = array(
        'Tle' => array(
            'dependent'=> true
        )
    ); 
}
?>
