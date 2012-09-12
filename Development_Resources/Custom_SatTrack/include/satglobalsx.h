/******************************************************************************/
/*                                                                            */
/*  Title       : satglobalsx.h                                               */
/*  Author      : Manfred Bester                                              */
/*  Date        : 03Mar92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Declarations of external variables and functions for        */
/*                'sattrack'.                                                 */
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

#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* external functions                                                         */
/*                                                                            */
/******************************************************************************/

extern double absol(), scalar(), getElement(), reduce();

extern long   getDayNum();

extern int    checkKeyboard(), closeSatIO(), controlISI(), decodeDate(), 
              decodeTime(), getPosFromMaidenHead(), getSatListPtr(), 
              initISI(), initSatIO(), 
              readTle(), readTleFile(), satEclipse(), stopISI();

extern char   *dayName(), *getenv(), *monthName();

extern void   calcCityVectors(), calcCommonPerturb(), calcGroundTrack(), 
              calcSDP4(), calcSGP4(), 
              calendar(), changeElementUnits(), changeToNoradUnits(), 
              checkArguments(), checkExit(), checkAddGroundStation(), 
              checkHelp(), checkLicense(), checkPerigeeHeight(), 
              checkPropModelType(), checkSwitchFrequency(), checkSwitchSat(), 
              checkSwitchPage(), checkTimeZone(), checkToggleTrack(), 
              cleanSatName(), clipString(), convertTime(), cross(), 
              deepSpacePeriodics(), deepSpaceSecular(), disableTrack(), 
              dispMessage(), dispTitle(), dispVersion(), doLiveCalcs(), 
              doLongPrediction(), doMultiSatTrackLoop(), doNoradTest(), 
              doShortPrediction(), doSingleSatTrackLoop(), doSleep(), 
              getAziElev(), getDate(), getDefaultData(), getDoppler(), 
              getEnvironment(), getGroundTrack(), getMaidenHead(), getMetStr(), 
              getMode(), getNextPass(), getNoradStateVector(), getNutation(), 
              getNutationSeries(), getOrbitNumber(), getPathLoss(), 
              getPhase(), getPrecMatrix(), 
              getRange(), getRealTime(), getRefraction(), getSatModes(), 
              getSatParams(), getSatPrecession(), getSemiMajorAxis(), 
              getShuttleOrbit(), getSiderealTime(), getSiteParams(), 
              getSiteNameID(), getSiteVector(), getSquintAngle(), 
              getStateVector(), getStateVectorX(), getSubSatPoint(), 
              getSunPhaseAngle(), getSunVector(), 
              getTimeArgs(), getTimeParams(), getTimeZone(), 
              getTleSets(), getTopoVector(), getTrueAnomaly(), getUnixTime(), 
              initDeepSpace(), initMain(), initMultiSatLiveDisp(), 
              initMultiSatTrackLoop(), initRestart(), initSingleSatLiveDisp(), 
              initSingleSatTrackLoop(), initSortSats(), initTerminal(), 
              kepler(), keplerX(), killProgram(), loadSatData(), 
              lowerCase(), mailRegistration(), makeOutputFile(), mGets(), 
              milliSleep(), multMatVec(), onAlarm(), printDate(), 
              printElements(), printMET(), printMode(), printPredHeader(), 
              printTime(), readCities(), readSatList(), replaceBlanks(), 
              saveSatData(), saveSatElements(), 
              setupPrecmat(), shortString(), showMainMenu(), sortSats(), 
              trackAntenna(), trackingControl(), trackRadio(), truncBlanks(), 
              updateDrag(), updateGravity(), updateGravityAndDrag(), 
              updateLongPeriodics(), updateShortPeriodics(), 
              updateMultiSatLiveDisp(), updateSatPage(), updateSatPointer(), 
              updateSingleSatLiveDisp(), upperCase();

/******************************************************************************/
/*                                                                            */
/* external graphics functions                                                */
/*                                                                            */
/******************************************************************************/

extern int    StartGraphics();

extern void   CreateGraphicsTimeOut(), QuitGraphics(), UpdateGraphics();

#ifdef XWINDOW

extern void   CalcGrid(), CalcGroundTrack(), CalcMapLocation(),
              CalcVisibCircle(), CleanSegments(), 
              CreateColors(), DrawFrame(), DrawMap(), DrawMir(), DrawSat(), 
              DrawShuttle(), 
              LoadGroundSegments(), LoadTdrsSegments(), 
              RedrawMap(), ResizeMap(), 
              SaveGroundSegments(), SaveTdrsSegments(), 
              TimeEvent();

#endif

/******************************************************************************/
/*                                                                            */
/* external variables                                                         */
/*                                                                            */
/******************************************************************************/

extern double timeZoneHour[];

extern double precMatrix[3][3], precMatrixTr[3][3], siteRotMatGl[3][3], 
              sunPosGl[3], moonPosGl[3], satPosGl[3], satPosPrec[3], 
              satVelGl[3], satPosS[3], satVelS[3], sitePosGl[3], siteVelGl[3], 
              localVecSatGl[3], localVecSunGl[3], localVecMoonGl[3], 
              siteVecGl[3], satAttGl[3], satSquintGl[3];

extern double curTimeGl, epochDay, elsetEpochDay, elsetEpoch, 
              epochMeanAnomaly, epochRaan, epochMeanMotion, curMeanMotion, 
              decayRate, decayRateDot, bStarCoeff, epochArgPerigee, 
              eccentricity, inclination, sunRA, sunDec, sunAzimuth, 
              sunElevation, sunDist, sunPhaseAngle, sunLat, sunLong, 
              moonRA, moonDec, moonAzimuth, moonElevation, moonDist, 
              refOrbit, curOrbit, orbitFract, stsOrbit, stsOrbitFract, 
              curMotion, curMotionX, meanAnomaly, meanAnomalyX, 
              trueAnomaly, trueAnomalyX, semiMajorAxis, 
              perigee, perigeeHeight, perigeePrec, apogeeHeight, satRadius, 
              raanPrec, curRaan, curRaanX, curArgPerigee, curArgPerigeeX, 
              curArgNodeX, curInclination, curInclinationX, 
              satLat, satLong, satHeight, satAzimuth, 
              satElevation, satRange, trackAzimuth, trackElevation, 
              lastTrackAzi, lastTrackEle, trackAziRate, trackEleRate, 
              downlinkLoss, uplinkLoss, squintAngle, rangeRate, satVelocity, 
              downlinkFreq, uplinkFreq, downlinkDopp, uplinkDopp, 
              freqOffset, freqStep, xponderBandwidth, xponderSign, 
              perigeePhase, satPhase, maxPhase, 
              attLat, attLong, siteLat, siteLong, siteAlt, effEarthRadius, 
              prevDispEle, prevSatLat, defMinElevation, minElevation, 
              defDuration, duration, defStepTime, stepTime, startTime, 
              stopTime, realTime, utcTime, julianDate, lastJulianDateSun, 
              lastJulianDatePrec, lastJulianDateNute, totPsi, totEps, 
              equEquinox, launchTime, launchEpoch, timeZone, 
              gmsTime, gasTime, lasTime, 
              cityLat, cityLong, lastSunGraphTime, lastSatGraphHeight, 
              gndTrkDist, atmPressure, ambTemperature, relHumidity, dummyd, 
              cosInclination, sinInclination, cosTrueAnomaly, sinTrueAnomaly;

extern long   satNum, dayNumber, lastDayNum, launchDate, epochOrbitNum, 
              orbitNum, stsOrbitNum, curYearSec, lastTrackYearSec;

extern int    satOrder[MAXSATS];

extern int    ephemerisType, launchYear, launchMonth, launchDay, 
              launchHour, launchMin, launchSec, launchFlag, attitudeFlag, 
              elementSet, numFreqs, freqPtr, numModes, propModelType, 
              maidenHeadFlag, batchModeFlag, debugFlag, defaultsFileFlag, 
              defaultsFileNum, graphicsFlag, graphicsOpenFlag, modelFlag, 
              quickStartFlag, verboseFlag, timeZoneFlag, timeZones, 
              eclipseCode, eclipseRise, eclipseMax, eclipseSet, 
              satTypeFlag, initNorad, geoSyncFlag, elevationFlag, newRiseFlag, 
              passDispFlag, countdownFlag, checkCountdown, shortPredFlag, 
              satCrashFlag, preLaunchFlag, preOrbitFlag, trackingFlag, 
              trackCtrlFlag, trackDispFlag, trackingEnableFlag, liveDispFlag, 
              didMultDispFlag, objectNumFlag, pointerFlag, pageFlag, 
              noPassFlag, xTermFlag, trueXtermFlag, gndTrkFlag, 
              newGndTrkFlag, justAcquFlag, justLostFlag, 
              trackObject, newTrackObjectFlag, firstLine, firstPage, 
              headerCol, numCities, oldCityNum, newCityNum, numSats, 
              numSatsAuto, numTle, numLines, numLinesX, numSatLines, 
              numGroundStations, 
              satLinePtr, transitFlag, sunTransitFlag, sunProximity, 
              sunTransit, mainChoice, nX, nY, lastX, lastY, firstGraphics, 
              dummyi;

extern char   modesDat[80], sitesDat[80], defaultsDat[80], defSatGroup[10], 
              defTimeZoneStr[10], timeZoneStr[10], elementFile[80], 
              satName[80], satTrackName[80], satAlias[80], fullSatAlias[80], 
              satFileName[80], siteName[80], siteNameID[80], fullSiteName[80], 
              satGroup[10], maidenHead[10], defDispType[10], defSat[40], 
              defSet[40], defSetTLE[40], defSetSTS[40], defSetType[40], 
              defSite[40], elementType[40], downlinkMode[8], uplinkMode[8], 
              modeString[10], timeString[80], epochString[40], 
              launchString[40], updateString[40], header[80], propModel[20], 
              gndTrkDir[10], gndTrkCity[80], dispStr[80], callSign[25], 
              termTypeX[20], antennaIO[80], radioIOA[80], radioIOB[80], 
              batchTimeZone[10], batchSiteName[80], batchSiteLat[80],
              batchSiteLong[80], batchSatName[80], batchTleL1[80],
              batchTleL2[80], batchPredType[80], batchStartDate[80],
              batchStartTime[80], batchStepTime[80], batchDuration[80],
              batchMinElev[80], batchAcceptElev[80], batchHardcopy[80], helpStr[80], licenseFile[80],
              graphicsTitle[80], sattrackTitle[80], xTimeStr[80], xMetStr[25],
              xGndTrkStr[80];

extern char   sattrName[], sattrVersion[], sattrHeader[], predHeader[], 
              dispHeader[], singleSatHelp[], multiSatHelp[], 
              *visibCode[], *dayNames[], *monthNames[], *timeZoneList[], 
              *strpHome, *strpTerm, *strpDisp, *strpRgst;

extern FILE   *fpr, *outFile, *antennaFile, *radioFileA, *radioFileB;

/******************************************************************************/
/*                                                                            */
/* structures                                                                 */
/*                                                                            */
/******************************************************************************/

typedef struct {
    double lat, lng, alt, X, Y, Z;
    char   cty[MAXCITYLEN+3];
    } cityType;

extern cityType city[MAXCITIES];


typedef struct { 
    long tleSatNum;
    char tleLine0[20];
    char tleLine1[70];
    char tleLine2[70];
    } tleType;

extern tleType tle[MAXSATS];


typedef struct { 
    char satelName[MAXSATNAMELEN+3];
    } satelType;

extern satelType satel[MAXSATS];


typedef struct { 
    double downlink, uplink, bandwidth, sign;
    char   downlinkMode[8], uplinkMode[8];
    } freqType;

extern freqType freqs[MAXFREQS];


typedef struct { 
    double minPhase, maxPhase;
    char   modeStr[5];
    } modeType;

extern modeType modes[MAXMODES];


typedef struct {
    double   epDay, elSetEp, launchEp, epInc, epRaan, epEcc, epArgPer;
    double   epMeanAnom, epMeanMot, epDecRate, epDecRateDot, epBstar;
    double   dwnlnkFrq, uplnkFrq, dwnlnkDopp, uplnkDopp;
    double   attLtd, attLng, attX, attY, attZ, perigPh, maxPh;
    double   semiMajAxis, raanPr, periPr, refOrb, cosInc, sinInc;
    double   satOrb, stsOrb, satOrbFrac, stsOrbFrac;
    double   satAzi, satEle, satLastEle, satRng, satLtd, satLng, satHgt;
    double   nextRiseT, nextSetT, riseAzi, maxAzi, setAzi, maxEle, maxRng;
    double   ctyLtd, ctyLng, ctyDist;
    long     satIdNum, epOrbNum, satOrbNum, stsOrbNum;
    int      numMd, numFr, elSet, ephType, eclCode, eclRise, eclMax, eclSet;
    int      modelType, statusFl, satTypeFl, geosyncFl, crashFl;
    int      launchFl, attitudeFl, prelaunchFl, preorbitFl, nopassFl;
    char     satellite[20], satelliteAlias[20], ctyDir[5], ctyStr[40];
    char     model[20], xMetStr[25];
    freqType satFreqs[MAXFREQS];
    modeType satModes[MAXMODES];
    } satType;

extern satType sat[MAXSATS];


typedef struct {
    double lng, ltd;
    } trackType;

extern trackType groundTrack[NSEGSGT];


typedef struct {
    double lng, ltd;
    char gndStnID[40];
    } stationType;

extern stationType groundStation[MAXSTATIONS];


/******************************************************************************/
/*                                                                            */
/* End of include file satglobalsx.h                                          */
/*                                                                            */
/******************************************************************************/
