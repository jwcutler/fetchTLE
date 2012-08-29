<?php
/**
 * Routes configuration
 *
 * In this file, you set up routes to your controllers and their actions.
 * Routes are very important mechanism that allows you to freely connect
 * different urls to chosen controllers and their actions (functions).
 *
 * PHP 5
 *
 * CakePHP(tm) : Rapid Development Framework (http://cakephp.org)
 * Copyright 2005-2012, Cake Software Foundation, Inc. (http://cakefoundation.org)
 *
 * Licensed under The MIT License
 * Redistributions of files must retain the above copyright notice.
 *
 * @copyright     Copyright 2005-2012, Cake Software Foundation, Inc. (http://cakefoundation.org)
 * @link          http://cakephp.org CakePHP(tm) Project
 * @package       app.Config
 * @since         CakePHP(tm) v 0.2.9
 * @license       MIT License (http://www.opensource.org/licenses/mit-license.php)
 */
/**
 * Here, we are connecting '/' (base path) to controller called 'Pages',
 * its action called 'display', and we pass a param to select the view file
 * to use (in this case, /app/View/Pages/home.ctp)...
 */

// Index location
Router::connect('/', array('controller' => 'content', 'action' => 'documentation'));

// Admin routes
Router::connect('/admin', array('controller' => 'panel', 'action' => 'index', 'admin' => true));
Router::connect('/admin/source/sourceupdate/:source', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sourceupdate', 'admin' => true));
Router::connect('/admin/source/add', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'add', 'admin' => true));
Router::connect('/admin/source/add', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'create', 'admin' => true));
Router::connect('/admin/source/:id/delete', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'remove', 'admin' => true));
Router::connect('/admin/source/:id/delete', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'delete', 'admin' => true));
Router::connect('/admin/source/:id/edit', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'edit', 'admin' => true));
Router::connect('/admin/source/:id/edit', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'change', 'admin' => true));
Router::connect('/admin/pass', array('[method]' => 'GET', 'controller' => 'pass', 'action' => 'index', 'admin' => true));
Router::connect('/admin/pass', array('[method]' => 'POST', 'controller' => 'pass', 'action' => 'update', 'admin' => true));
Router::connect('/admin/station/add', array('[method]' => 'POST', 'controller' => 'station', 'action' => 'create', 'admin' => true));
Router::connect('/admin/station/:id/delete', array('[method]' => 'GET', 'controller' => 'station', 'action' => 'remove', 'admin' => true));
Router::connect('/admin/station/:id/delete', array('[method]' => 'POST', 'controller' => 'station', 'action' => 'delete', 'admin' => true));
Router::connect('/admin/station/:id/edit', array('[method]' => 'GET', 'controller' => 'station', 'action' => 'edit', 'admin' => true));
Router::connect('/admin/station/:id/edit', array('[method]' => 'POST', 'controller' => 'station', 'action' => 'change', 'admin' => true));

// Tool routes
Router::connect('/tools/passes', array('[method]' => 'GET', 'controller' => 'pass', 'action' => 'index', 'tools' => true));
Router::connect('/tools/passes/:satellite', array('[method]' => 'GET', 'controller' => 'pass', 'action' => 'calculator', 'tools' => true));
Router::connect('/tools/passes/:satellite', array('[method]' => 'POST', 'controller' => 'pass', 'action' => 'calculate', 'tools' => true));

// API routes
Router::connect('/api/sources/:sources', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sources', 'api' => true));
Router::connect('/api/sources', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sources', 'api' => true));
Router::connect('/api/satellites/:satellites', array('[method]' => 'GET', 'controller' => 'satellite', 'action' => 'satellites', 'api' => true));
Router::connect('/api/positions/:satellites', array('[method]' => 'GET', 'controller' => 'position', 'action' => 'positions', 'api' => true));
Router::parseExtensions();

/**
 * Load all plugin routes.  See the CakePlugin documentation on 
 * how to customize the loading of plugin routes.
 */
	CakePlugin::routes();

/**
 * Load the CakePHP default routes. Remove this if you do not want to use
 * the built-in default routes.
 */
	require CAKE . 'Config' . DS . 'routes.php';
