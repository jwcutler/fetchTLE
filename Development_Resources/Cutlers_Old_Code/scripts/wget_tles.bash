#!/bin/bash
#===============================================================================
#
# Quick script to download keps from online.  URL is the location of the TLE 
# file.  OUTPUT is the file to which the TLE file should be downloaded to.
#
# Todo:
#	- Modify code to check that URL exists.
#	- Convert source of info to a DB, not an environment variable
#===============================================================================

TMP_DIR="/tmp"
#TLE_URLS="http://celestrak.com/NORAD/elements/cubesat.txt http://celestrak.com/NORAD/elements/stations.txt"
TLE_URLS="http://celestrak.com/NORAD/elements/cubesat.txt"
FETCHTLE_PATH="./"

for url in $TLE_URLS
do
	# Download the file using a web page fetch program, wget.  Output to a 
	#temp directory.

	wget -O $TMP_DIR/tle.$$ $url

	# Insert TLEs into DB.
	cat tle.$$ | perl $FETCHTLE_PATH/insert_keps_into_db.pl 

	# Clean up

	rm $TMP_DIR/tle.$$

done
exit

