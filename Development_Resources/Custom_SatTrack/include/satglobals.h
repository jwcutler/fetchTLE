/******************************************************************************/
/*                                                                            */
/*  Title       : satglobals.h                                                */
/*  Author      : Manfred Bester                                              */
/*  Date        : 03Mar92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Definitions of global variables for 'sattrack'.             */
/*                                                                            */
/*                                                                            */
/*  SatTrack is Copyright (c) 1992, 1993, 1994, 1995 by Manfred Bester.       */
/*  All Rights Reserved.                                                      */
/*                                                                            */
/*  Permission to use, copy, and distribute SatTrack and its documentation    */
/*  in its entirety for educational, research and non-profit purposes,        */
/*  without fee, and without a written agreement is hereby granted, provided  */
/*  that the above copyright notice and the following three paragraphs appear */
/*  in all copies. SatTrack may be modified for personal purposes, but        */
/*  modified versions may NOT be distributed without prior consent of the     */
/*  author.                                                                   */
/*                                                                            */
/*  Permission to incorporate this software into commercial products may be   */
/*  obtained from the author, Dr. Manfred Bester, 1636 M. L. King Jr. Way,    */
/*  Berkeley, CA 94709, USA. Note that distributing SatTrack 'bundled' in     */
/*  with ANY product is considered to be a 'commercial purpose'.              */
/*                                                                            */
/*  IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, */
/*  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF   */
/*  THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE AUTHOR HAS BEEN ADVISED  */
/*  OF THE POSSIBILITY OF SUCH DAMAGE.                                        */
/*                                                                            */
/*  THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT      */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A   */
/*  PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"      */
/*  BASIS, AND THE AUTHOR HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, */
/*  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.                                  */
/*                                                                            */
/******************************************************************************/

double precMatrix[3][3];               /* precession matrix                   */
double precMatrixTr[3][3];             /* transposed precession matrix        */
double siteRotMatGl[3][3];             /* site transformation matrix          */

double sunPosGl[3];                    /* in Right Ascension based system     */
double moonPosGl[3];                   /* in Right Ascension based system     */

double satPosGl[3];                    /* in Right Ascension based system     */
double satPosPrec[3];                  /* in Right Ascension based system     */
double satVelGl[3];                    /* velocity [km/s]                     */

double satPosS[3];                     /* in Right Ascension based system     */
double satVelS[3];                     /* velocity [km/s]                     */

double sitePosGl[3];                   /* in Right Ascension based system     */
double siteVelGl[3];                   /* velocity [km/s]                     */

double localVecSatGl[3];               /* in local geodetic system            */
double localVecSunGl[3];               /* in local geodetic system            */
double localVecMoonGl[3];              /* in local geodetic system            */

double siteVecGl[3];                   /* fixed site vector on the Earth      */

double satAttGl[3];                    /* attitude vector in Bahn coordinates */
double satSquintGl[3];                 /* squint vector in Bahn coordinates   */

double curTimeGl;                      /* global current time [d]             */
double epochDay, elsetEpochDay;        /* time of epoch                       */
double elsetEpoch;

double epochMeanAnomaly;               /* Mean Anomaly at epoch               */
double epochRaan;                      /* RAAN at epoch                       */
double epochMeanMotion;                /* [rev/day]                           */
double curMeanMotion;                  /* [rev/day]                           */
double decayRate;                      /* [rev/day^2]                         */
double decayRateDot;                   /* [rev/day^3]                         */
double bStarCoeff;                     /* BSTAR drag coefficient              */
double epochArgPerigee;                /* argument of perigee at epoch        */
double eccentricity;                   /* eccentricity of satellite orbit     */
double inclination;                    /* inclination of satellite orbit      */
double sunPhaseAngle;                  /* angle between ground station and    */
                                       /* the Sun as seen from the satellite  */

double sunRA, sunDec, sunAzimuth, sunElevation, sunDist, sunLat, sunLong;
double moonRA, moonDec, moonAzimuth, moonElevation, moonDist;

double refOrbit;                       /* orbit number at epoch               */
double curOrbit, orbitFract;           /* double current orbit number         */
double stsOrbit, stsOrbitFract;        /* current orbit number for STS        */
double curMotion, curMotionX;          /* current number of rev/day           */
double meanAnomaly, meanAnomalyX;      /* mean anomaly                        */
double trueAnomaly, trueAnomalyX;      /* mean anomaly corrected for ellipse  */
double semiMajorAxis;                  /* semimajor axis of satellite orbit   */
double perigee;                        /* perigee of satellite orbit          */
double perigeeHeight;                  /* perigee height of satellite orbit   */
double apogeeHeight;                   /* apogee height of the orbit          */
double satRadius;                      /* distance measured from geocenter    */
double curRaan, curRaanX;              /* RAAN                                */
double raanPrec;                       /* precession of RAAN                  */
double curArgPerigee, curArgPerigeeX;  /* argument of perigee                 */
double curArgNodeX;                    /* argument of the ascending node      */
double curInclination;                 /* inclination                         */
double curInclinationX;                /* inclination from SGP4/SDP4 model    */
double perigeePrec;                    /* precession of argument of perigee   */
double satLat, satLong, satHeight;     /* sub-satellite point                 */
double satAzimuth, satElevation;       /* [rad]                               */
double trackAzimuth, trackElevation;   /* [rad]                               */
double lastTrackAzi, lastTrackEle;     /* [rad]                               */
double trackAziRate, trackEleRate;     /* [rad/s]                             */
double satRange;                       /* [km] or [nm]                        */
double downlinkLoss, uplinkLoss;       /* downlink/uplink path loss [dB]      */
double squintAngle;                    /* squint angle [deg]                  */
double rangeRate;                      /* velocity w.r.t. ground station      */
double satVelocity;                    /* orbital velocity [km/s]             */
double downlinkFreq, uplinkFreq;       /* downlink/uplink frequency [Hz]      */
double downlinkDopp, uplinkDopp;       /* downlink/uplink Doppler shift [Hz]  */
double freqOffset, freqStep;           /* frequency tuning corrections [Hz]   */
double xponderBandwidth, xponderSign;

double perigeePhase;
double attLong, attLat;                /* spacecraft attitude [deg]           */
double satPhase, maxPhase;             /* phase units in 1 orbit              */

double siteLat, siteLong, siteAlt;     /* site parameters                     */
double effEarthRadius;
double prevDispEle, prevSatLat;
double defMinElevation, minElevation;  /* minimum elevation for pass calc.    */

double defDuration, duration;          /* duration of orbit prediction [d]    */
double defStepTime, stepTime;          /* step time for orbit prediction [s]  */
double startTime, stopTime, realTime;  /* time parameters [d]                 */
double utcTime;                        /* UTC [d]                             */
double julianDate;                     /* Julian date [d]                     */
double lastJulianDateSun;              /* Julian date [d]                     */
double lastJulianDatePrec;             /* Julian date [d]                     */
double lastJulianDateNute;             /* Julian date [d]                     */
double launchTime, launchEpoch;        /* launch time parameters [d]          */
double timeZone;                       /* time zone [h] (west = '+')          */
double gmsTime;                        /* Greenwich mean sidereal time        */
double gasTime;                        /* Greenwich apparent sidereal time    */
double lasTime;                        /* local apparent sidereal time        */
double lastSunGraphTime;               /* last time terminator updated [d]    */
double lastSatGraphHeight;             /* last height for visib circle [km]   */
double totPsi, totEps;                 /* nutation in longitude and obliquity */
double equEquinox;                     /* equation of the equinoxes           */
double cityLat, cityLong;              /* nearest city coordinates [deg]      */
double gndTrkDist;                     /* distance betw. SSP and nearest city */
double atmPressure;                    /* atmospheric pressure [mmHg]         */
double ambTemperature;                 /* ambient temperature [C]             */
double relHumidity;                    /* relative humidity [%]               */
double dummyd;                         /* dummy double precision variable     */

double cosInclination, sinInclination;
double cosTrueAnomaly, sinTrueAnomaly;

long   satNum;                         /* number of satellite                 */
long   dayNumber;                      /* day number since 1900               */
long   lastDayNum;
long   launchDate;                     /* launch date                         */
long   epochOrbitNum;                  /* integer orbit number of epoch       */
long   orbitNum, stsOrbitNum;          /* integer part of orbit number        */
long   curYearSec;                     /* seconds in the year for tracking    */
long   lastTrackYearSec;

int    satOrder[MAXSATS];

int    ephemerisType, launchYear, launchMonth, launchDay, launchHour;
int    launchMin, launchSec, launchFlag, attitudeFlag, elementSet;
int    numFreqs, freqPtr, numModes, propModelType, maidenHeadFlag; 
int    batchModeFlag, debugFlag, defaultsFileFlag, defaultsFileNum;
int    graphicsFlag, graphicsOpenFlag, modelFlag, quickStartFlag, verboseFlag;
int    timeZoneFlag, timeZones;
int    eclipseCode, eclipseRise, eclipseMax, eclipseSet;
int    satTypeFlag, initNorad, geoSyncFlag, elevationFlag, newRiseFlag;
int    passDispFlag, countdownFlag, checkCountdown, shortPredFlag;
int    satCrashFlag, preLaunchFlag, preOrbitFlag;
int    trackingFlag, trackCtrlFlag, trackDispFlag, trackingEnableFlag;
int    liveDispFlag, didMultDispFlag;
int    objectNumFlag, noPassFlag, xTermFlag, trueXtermFlag;
int    gndTrkFlag, newGndTrkFlag;
int    justAcquFlag, justLostFlag, pointerFlag, pageFlag;
int    trackObject, newTrackObjectFlag, firstLine, firstPage, headerCol;
int    numCities, oldCityNum, newCityNum, numSats, numSatsAuto, numTle;
int    numLines, numLinesX, numSatLines, satLinePtr, numGroundStations;
int    transitFlag, sunTransitFlag, sunProximity, sunTransit;
int    mainChoice, nX, nY, lastX, lastY, firstGraphics, dummyi;

char   modesDat[80], sitesDat[80], defaultsDat[80], defSatGroup[10];
char   defTimeZoneStr[10], timeZoneStr[10], elementFile[80];
char   satName[80], satTrackName[80], satAlias[80], fullSatAlias[80];
char   satFileName[80], siteName[80], siteNameID[80], fullSiteName[80];
char   satGroup[10], maidenHead[10];
char   defDispType[10], defSat[40], defSet[40], defSetTLE[40], defSetSTS[40];
char   defSetType[40], defSite[40], elementType[40], callSign[25];
char   downlinkMode[8], uplinkMode[8], modeString[10], timeString[80];
char   epochString[40];
char   launchString[40], updateString[40], header[80], propModel[20];
char   gndTrkDir[10], gndTrkCity[80], dispStr[80], termTypeX[20];
char   antennaIO[80], radioIOA[80], radioIOB[80];
char   batchTimeZone[10];
char   batchSiteName[80], batchSiteLat[80], batchSiteLong[80];
char   batchSatName[80], batchTleL1[80], batchTleL2[80];
char   batchTleType[80], batchPredType[80], batchStartDate[80];
char   batchStartTime[80], batchStepTime[80], batchDuration[80];
char   batchMinElev[80], batchHardcopy[80], helpStr[80], licenseFile[80];
char   graphicsTitle[80], xTimeStr[80], xMetStr[25], xGndTrkStr[80];
char   sattrackTitle[80];
char   *strpHome, *strpTerm, *strpDisp, *strpRgst;

FILE   *fpr, *outFile, *antennaFile, *radioFileA, *radioFileB;

cityType    city[MAXCITIES];
tleType     tle[MAXSATS];
satelType   satel[MAXSATS];
freqType    freqs[MAXFREQS];
modeType    modes[MAXMODES];
satType     sat[MAXSATS];
trackType   groundTrack[NSEGSGT];
stationType groundStation[MAXSTATIONS];

/******************************************************************************/
/*                                                                            */
/* End of include file satglobals.h                                           */
/*                                                                            */
/******************************************************************************/
