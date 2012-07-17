<?php
/*
Source Model

Handles all TLE source management operations.
*/

class Source extends AppModel {
    var $name = 'Source';
    
    // Define associations
    public $hasMany = 'Tle';
    
    // Validations
    public $validate = array(
        'name' => array(
            'name_unique' => array(
                'rule' => 'isUnique',
                'required' => true,
                'allowEmpty' => false
            )
        ),
        'url' => 'notEmpty'
    );
}
?>
