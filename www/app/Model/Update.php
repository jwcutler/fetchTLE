<?php
/*
Update Model

This model keeps track of updates and the TLE's parsed in each update. It is used to query TLE's by date.
*/

class Update extends AppModel {
    // Set up model
    var $name = 'Update';
    public $hasMany = 'Tle';
    public $belongsTo = 'Source';
}
?>
