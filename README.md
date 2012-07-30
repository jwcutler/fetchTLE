#fetchTLE#

fetchTLE is a web-based application that aggregates satellite TLEs from various sources and presents them using a RESTful API in a variety of formats.

Dependencies
------------
fetchTLE was developed on top of the popular CakePHP MVC framework and shares its requirements. Namely:
* An HTTP server (e.g. Apache)
* PHP >= 5.2.8
* MySQL >= 4

In addition, cURL needs to be installed and loaded into PHP.

Installation
------------
To install fetchTle, perform the following procedure:

1. Modify app/Config/database.php.default to reflect your database and save it as 'database.php'
2. Import Development_Resources/SQL_Schema/fetchtle.sql into your newly created database.
3. Modify app/Config/core.php.default by changing the salt (line 187) and seed (line 192) to random values specific to your application and save it as 'core.php'.
4. Change the permissions of the app/tmp directory to 777.
5. Generate an admin password hash by visiting fetchtlelocation.com/admin/panel/makehash.
6. Update the 'admin' user in the database by replacing 'dummypassword' with the hash you just created.

``` php
/**
 * A random string used in security hashing methods.
 */
Configure::write('Security.salt', 'yoursaltvalue');

/**
 * A random numeric string (digits only) used to encrypt/decrypt strings.
 */
Configure::write('Security.cipherSeed', 'yourcipherseedvalue');
```

Using fetchTLE
----------------
After fetchTLE has been installed, you can access the administration panel at fetchtlelocation.com/admin using the 'admin' user with the password you created in step 5 above. From the administration panel you can:
* Manually update TLE sources all at once or individually
* Manage TLE sources
* View TLE update errors

For details on how to use the API, simply visit fetchtlelocation.com for a guide.

### CRON Updates
fetchTLE's sources can be updated by using a standard CRON tab. To make use of this feature, simply set up a CRON tab to call:
```
/path/to/fetchTLE/app/Console/cake TleUpdate update [source names]
```
If no source names are included, all of them will be updated.

