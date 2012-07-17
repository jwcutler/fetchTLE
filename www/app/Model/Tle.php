<?php
/*
TLE Model

This model handles all TLE management and interaction options.
*/

class Tle extends AppModel {
    var $name = 'Tle';
    
    // Define associations
    public $belongsTo = 'Source';
}
?>
