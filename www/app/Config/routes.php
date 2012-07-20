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
Router::connect('/', array('controller' => 'display', 'action' => 'index'));

// Admin routes
Router::connect('/admin', array('controller' => 'panel', 'action' => 'index', 'admin' => true));
Router::connect('/admin/source/sourceupdate/:source', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sourceupdate', 'admin' => true));
Router::connect('/admin/source/add', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'add', 'admin' => true));
Router::connect('/admin/source/add', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'create', 'admin' => true));
Router::connect('/admin/source/:id/delete', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'remove', 'admin' => true));
Router::connect('/admin/source/:id/delete', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'delete', 'admin' => true));
Router::connect('/admin/source/:id/edit', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'edit', 'admin' => true));
Router::connect('/admin/source/:id/edit', array('[method]' => 'POST', 'controller' => 'source', 'action' => 'change', 'admin' => true));

// API routes
Router::connect('/api/sources/:sources', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sources', 'api' => true));
Router::connect('/api/sources', array('[method]' => 'GET', 'controller' => 'source', 'action' => 'sources', 'api' => true));
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
