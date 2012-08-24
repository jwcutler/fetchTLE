/*
This program calculates the longitude, latitude, and altitude of the
specified satellite at periodic intervals and outputs the results to
the console.

For use with the fetchTLE API.
*/

// Required Libraries
#include "sgp4sdp4.h"

int main(int argc, char *argv[]){
	// Variable setup
	char sat_name[ARGLENGTH], sat_line_1[ARGLENGTH], sat_line_2[ARGLENGTH], sat_start[ARGLENGTH], sat_end[ARGLENGTH], sat_resolution[ARGLENGTH], ephem[5];
	int tle_load_attempt, start, end, resolution, curr_time;
	struct tm utc;
	double jul_epoch, jul_utc, time_since_epoch;
	double sat_lat, sat_lon, sat_alt;
	tle_t sat_tle;
	vector_t zero_vector = {0,0,0,0};
	vector_t vel = zero_vector;
	vector_t pos = zero_vector;
	vector_t obs_set;
	geodetic_t sat_geodetic;
	geodetic_t obs_geodetic = {0.7368, -1.4615, 0.251, 0.0}; // This doesn't matter
	
	// Validate the number of arguments
	if (argc != 7){
		printf("[ERROR] Invalid number of arguments.\n");
		exit(1);
	}
	
	// Collect the arguments
	strcpy(sat_name, argv[1]);
	strcpy(sat_line_1, argv[2]);
	strcpy(sat_line_2, argv[3]);
	strcpy(sat_start, argv[4]);
	strcpy(sat_end, argv[5]);
	strcpy(sat_resolution, argv[6]);
	start = atoi(sat_start);
	end = atoi(sat_end);
	resolution = atoi(sat_resolution);
	
	// Parse the TLEs into the library
	tle_load_attempt = Input_Tle_Set(sat_name, sat_line_1, sat_line_2, &sat_tle);
	if (tle_load_attempt == -1){
		printf("[ERROR] Invalid TLE information supplied.\n");
	}
	
	// Setup propagator
	ClearFlag(ALL_FLAGS);
	select_ephemeris(&sat_tle);
	
	// Loop through the time range
	curr_time = start;
	while (1){
		// Convert the time
		UTC_Calendar_Now(&utc, curr_time);
		jul_utc = Julian_Date(&utc);
		jul_epoch = Julian_Date_of_Epoch(sat_tle.epoch);
		time_since_epoch = (jul_utc - jul_epoch) * xmnpda;
		
		// Check for deep space flag
		if(isFlagSet(DEEP_SPACE_EPHEM_FLAG)){
			strcpy(ephem,"SDP4");
		} else {
			strcpy(ephem,"SGP4");
		}
		
		// Call the appropriate propagation model
		if(isFlagSet(DEEP_SPACE_EPHEM_FLAG)){
			SDP4(time_since_epoch, &sat_tle, &pos, &vel);
		} else {
			SGP4(time_since_epoch, &sat_tle, &pos, &vel);
		}
		
		// Convert vectors to km and km/sec
		Convert_Sat_State(&pos, &vel);
		
		// Calculate satellite velocity
		Magnitude(&vel);
		
		// Calculate satellite Azi, Ele, Range and Range-rate
		Calculate_Obs(jul_utc, &pos, &vel, &obs_geodetic, &obs_set);
		
		// Calculate satellite Lat North, Lon East and Alt
		Calculate_LatLonAlt(jul_utc, &pos, &sat_geodetic);
		
		// Collect and output satellite parameters
		sat_lat = Degrees(sat_geodetic.lat);
		sat_lon = Degrees(sat_geodetic.lon);
		sat_alt = sat_geodetic.alt;
		printf("%d:%f:%f:%f\n", curr_time, sat_lat, sat_lon, sat_alt);
		
		// Increment time
		curr_time = curr_time + resolution;
		if (curr_time >= end){
			break;
		}
	}
	
	return(0);
}
