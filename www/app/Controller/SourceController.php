<?php
/*
This controller is responsible for fetchTLE source management and display.
*/

class SourceController extends AppController {
    var $uses = array('Source', 'Tle'); 
    var $components = array('RequestHandler');
    
    function beforeFilter(){
        parent::beforeFilter();
        
        $this->Auth->allow(); 
    }
    
    public function admin_index(){
        /*
        Display the main index of sources.
        */
        
        // Load all configured sources
        $this->set('sources', $this->Source->find('all'));
    }
    
    public function admin_add(){
        /*
        Display the source add page.
        */
        
        $this->set("title_for_layout","Create a TLE Source");
    }
    
    public function admin_create(){
        /*
        Process the form submission from admin_add()
        */
        
        if ($this->request->is('post')) {
            // Make sure a source with this name doesn't exist
            $name_check = $this->Source->find('first', array(
                'conditions' => array('Source.name' => $_POST['source_name'])
            ));
            
            if ($name_check){
                // Source exists all ready
                $this->Session->setFlash('A source with that name all ready exists. Source names must be unique.', 'default', array('class' => 'alert alert-error'));
                $this->redirect(array('controller' => 'source', 'action' => 'add'));
            } else {
                // Source doesn't exist, try to create it
                $new_source['Source']['name'] = $_POST['source_name'];
                $new_source['Source']['url'] = $_POST['source_url'];
                $new_source['Source']['description'] = $_POST['source_description'];
                $new_source['Source']['created_on'] = date('Y-m-d H:i:s', time());
                $new_source['Source']['updated_on'] = date('Y-m-d H:i:s', time());
                
                $save_source = $this->Source->save($new_source);
                
                if ($save_source){
                    $this->Session->setFlash('The source has been added successfully.', 'default', array('class' => 'alert alert-success'));
                    CakeLog::write('admin', '[success] New source \''.$_POST['source_name'].'\' added.');
                } else {
                    $this->Session->setFlash('An error occured while adding that source. Please try again.', 'default', array('class' => 'alert alert-error'));
                    CakeLog::write('admin', '[error] Error adding source \''.$_POST['source_name'].'\' added.');
                }
                
                $this->redirect(array('controller' => 'source', 'action' => 'index'));
            }
        } else {
            // Redirect them to the add form
            $this->redirect(array('controller' => 'source', 'action' => 'add'));
        }
    }
}
?>
