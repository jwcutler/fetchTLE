/******************************************************************************/
/*                                                                            */
/*  Title       : satread.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 03Mar92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines for reading data from resource files and other     */
/*                I/O functions.                                              */
/*                                                                            */
/*                                                                            */
/*  Resources   : cities.dat, defaults.dat, modes.dat, satlist_nnn.dat,       */
/*                tlex.dat                                                    */
/*                                                                            */
/*  cities.dat:      contains information on various gorund stations          */
/*                   format: `name`,lat,long,alt                              */
/*                                                                            */
/*  defaults.dat:    contains default parameters                              */
/*                                                                            */
/*  modes.dat:       contains information on launch dates, frequencies,       */
/*                   transmission modes, attitudes etc.                       */
/*                                                                            */
/*  satlist_nnn.dat: contains list of satellite names for multisat tracking   */
/*                   display ('nnn' denotes the satellite group)              */
/*                                                                            */
/*  tle.dat:         contains NORAD two-line orbital element sets             */
/*                   (also sts.dat, sts-55.dat, tvsat.dat, etc.)              */
/*                                                                            */
/*  tlex.dat:        contains concatenated NORAD/NASA two-line orbital        */
/*                   element sets                                             */
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

#include <stdio.h>
#include <math.h>
#include <string.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

#ifdef HPTERM
#include "hpterm.h"               /* definitions of hpterm macros             */
#else
#include "vt100.h"                /* definitions of VT100 macros              */
#endif

/******************************************************************************/
/*                                                                            */
/* checkTimeZone: enter time zone                                             */
/*                                                                            */
/******************************************************************************/

void checkTimeZone()

{
    if (timeZoneFlag)
    {
        nl();
        printf("Time zone                      ");
        printf("<%4s> : ",defTimeZoneStr);
        mGets(timeZoneStr,80,stdin);
        upperCase(timeZoneStr);

        if (!strlen(timeZoneStr))
            strcpy(timeZoneStr,defTimeZoneStr);

        getTimeZone();
        printf("Time zone set to %s (%+.2f h)\n",
                timeZoneStr,timeZone*24.0);
    }

    if (batchModeFlag)
    {
        strcpy(timeZoneStr,batchTimeZone);
        getTimeZone();
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getTimeZone: gets time zone [h] from the time zone string information      */
/*                                                                            */
/******************************************************************************/

void getTimeZone()

{
    int i;

    timeZone = 0.0;

    for (i = 0; i < timeZones; i++)
    {
        if (!strcmp(timeZoneList[i],timeZoneStr))
            timeZone = timeZoneHour[i] / 24.0;          /* gets time zone [d] */
    }

    if (fabs(timeZone) < ONEPPM)
    {
        if (strlen(timeZoneStr) && fabs((double) atof(timeZoneStr)) < 12.000001
                                && fabs((double) atof(timeZoneStr)) >  0.000001)
        {
            timeZone = (double) atof(timeZoneStr) / 24.0;
            sprintf(timeZoneStr,"LOC");
        }

        else
            sprintf(timeZoneStr,"UTC");
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getDefaultData: reads default parameters from 'defaults.dat'               */
/*                                                                            */
/******************************************************************************/

void getDefaultData()

{
    char defLine[120];
    FILE *defFile;

    if (defaultsFileFlag)
        sprintf(defaultsDat,"%s/%s/defaults%d.dat",
                strpHome,DATA,defaultsFileNum);
    else
        sprintf(defaultsDat,"%s/%s/%s",strpHome,DATA,DEFAULTS);

    if ((defFile = fopen(defaultsDat,"r")) == NULL)
    {
        doBeep(); nl(); reverseBlink();
        printf(" Cannot open input file '%s' \n",defaultsDat);
        normal(); nl();
        exit(-1);
    }

    sprintf(defSite,"%s",DEFSITE);
    sprintf(callSign,"%s",DEFCALLSIGN);
    sprintf(defSat,"%s",DEFSAT);
    sprintf(defSatGroup,"%s",DEFSATGROUP);
    sprintf(defSetTLE,"%s",DEFSETTLE);
    sprintf(defSetSTS,"%s",DEFSETSTS);
    sprintf(defSetType,"%s",DEFSETTYPE);
    sprintf(defDispType,"%s",DEFDISPTYPE);
    sprintf(defTimeZoneStr,"%s",DEFTIMEZONE);
    sprintf(antennaIO,"%s",DEFANTENNADEV);
    sprintf(radioIOA,"%s",DEFRADIODEVA);
    sprintf(radioIOB,"%s",DEFRADIODEVB);
    sprintf(termTypeX,"%s",DEFXTERM);

    defDuration     = DEFDURATION;
    defMinElevation = DEFMINELEVATION;
    trackingFlag    = DEFTRACKINGFLAG;
    numLines        = DEFDISPLINES;
    numLinesX       = DEFXDISPLINES;
    atmPressure     = DEFATMPRESS;
    ambTemperature  = DEFAMBTEMP;
    relHumidity     = DEFRELHUMID;

    while (fgets(defLine,100,defFile))
    {
        defLine[strlen(defLine) - 1] = '\0';
        truncBlanks(defLine);

        if (!strncmp(defLine,"Site:",5))
        {
            clipString(defLine,5);
            sprintf(defSite,"%s",defLine);
        }

        if (!strncmp(defLine,"Call sign:",10))
        {
            clipString(defLine,10);
            sprintf(callSign,"%s",defLine);
        }

        if (!strncmp(defLine,"Satellite:",10))
        {
            clipString(defLine,10);
            sprintf(defSat,"%s",defLine);
        }

        sscanf(defLine,"Satellite group: %s",defSatGroup);
        sscanf(defLine,"Element set TLE: %s",defSetTLE);
        sscanf(defLine,"Element set STS: %s",defSetSTS);
        sscanf(defLine,"Element set type: %s",defSetType);
        sscanf(defLine,"Live display type: %s",defDispType);
        sscanf(defLine,"Time zone: %s",defTimeZoneStr);
        sscanf(defLine,"Duration: %lf",&defDuration);
        sscanf(defLine,"Min elevation: %lf",&defMinElevation);
        sscanf(defLine,"Tracking: %d",&trackingFlag);
        sscanf(defLine,"Antenna: %s",antennaIO);
        sscanf(defLine,"Radio A: %s",radioIOA);
        sscanf(defLine,"Radio B: %s",radioIOB);
        sscanf(defLine,"Display lines: %d",&numLines);
        sscanf(defLine,"X display lines: %d",&numLinesX);
        sscanf(defLine,"X terminal type: %s",termTypeX);
        sscanf(defLine,"Pressure: %lf",&atmPressure);
        sscanf(defLine,"Temperature: %lf",&ambTemperature);
        sscanf(defLine,"Humidity: %lf",&relHumidity);
    }

    upperCase(defDispType);

    strcpy(timeZoneStr,defTimeZoneStr);
    getTimeZone();

    fclose(defFile);

    if (verboseFlag)
    {
        nl();
        printf("Default parameters:\n\n");
        printf("Ground station    : %20s\n",defSite);
        printf("Call sign         : %20s\n",callSign);
        printf("Satellite         : %20s\n",defSat);
        printf("Element set TLE   : %20s\n",defSetTLE);
        printf("Element set STS   : %20s\n",defSetSTS);
        printf("Element set type  : %20s\n",defSetType);
        printf("Live display type : %20s\n",defDispType);
        printf("Satellite group   : %20s\n",defSatGroup);
        printf("Time zone         : %20s (%+.1f h)\n",
                timeZoneStr,timeZone*24.0);
        printf("Duration          : %20.1f d\n",defDuration);
        printf("Min elevation     : %20.1f deg\n",defMinElevation);
        printf("Tracking          : %20d\n",trackingFlag);
        printf("Antenna interface : %20s\n",antennaIO);
        printf("Radio interface A : %20s\n",radioIOA);
        printf("Radio interface B : %20s\n",radioIOB);
        printf("Display lines     : %20d\n",numLines);
        printf("X display lines   : %20d\n",numLinesX);
        printf("X terminal type   : %20s\n",termTypeX);
        printf("Pressure          : %20.1f mmHg\n",atmPressure);
        printf("Temperature       : %20.1f C\n",ambTemperature);
        printf("Humidity          : %20.1f %%\n",relHumidity);
        nl();
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSatFileName: gets reduced satellite name for creating output file, i.e. */
/*                 replace blanks and "/" in file name with "_", delete "("   */
/*                 and ")"                                                    */
/*                                                                            */
/******************************************************************************/

void getSatFileName()

{
    int i, j;

    i = 0;
    j = 0;

    do
    {
        if (satName[i] != ' ' && satName[i] != '(' &&
            satName[i] != ')' && satName[i] != '"' &&
            satName[i] != '/')
        {
            satFileName[j] = satName[i];
            i++; j++;
        }

        if (satName[i] == ' ' || satName[i] == '/')
        {
            satFileName[j] = '_';
            i++; j++;
        }

        if (satName[i] == '(' || satName[i] == ')' || satName[i] == '"')
        {
            i++;
        }
    }
    while (satName[i] != '\0');

    satFileName[i] = '\0';
    return;
}

/******************************************************************************/
/*                                                                            */
/* getSatParams: reads satellite parameters from data file with the two-line  */
/*               orbital element sets                                         */
/*                                                                            */
/******************************************************************************/

void getSatParams()

{
    int    error, error1, firstTry;
    char   str[80], stsName[20];

    error = TRUE;

    while (error)
    {
        if (batchModeFlag)
            strcpy(satName,batchSatName);

        else
        {
            if (quickStartFlag)
                *satName = '\0';

            else
            {
                printf("Satellite name <%20s> : ",defSat);
                mGets(satName,80,stdin);
            }
        }

        if (!strlen(satName))
            sprintf(satName,"%s",defSat);

        cleanSatName(satName);
        strcpy(stsName,satName);
        upperCase(stsName);

        satTypeFlag = BASIC;

        if (!strncmp(stsName,"STS",3))
            satTypeFlag = STS;

        if (!strncmp(stsName,"MIR",3) && (int) strlen(stsName) == 3)
            satTypeFlag = MIR;

        sprintf(defSet,"%s",(satTypeFlag == STS) ? defSetSTS : defSetTLE);

        if (batchModeFlag)
            strcpy(elementFile,batchTleFile);

        else
        {
            if (quickStartFlag)
                *elementFile = '\0';

            else
            {
                printf("Two-line elements <%17s> : ",defSet);
                mGets(elementFile,80,stdin);
            }
        }

        if (!strlen(elementFile))
            sprintf(elementFile,"%s",defSet);

        if (strcmp(TLEX,elementFile))
        {
            error = readTleFile(strpHome,elementFile,numTle);

            if (error)
            {
                doBeep(); nl(); reverseBlink();
                printf(" File '%s' not found \n",elementFile);
                normal(); nl();

                if (batchModeFlag || quickStartFlag)
                    exit(-1);
            }
        }

        error = readTle(TRUE,satName,&satNum,&elementSet,
                   &epochDay,&inclination,&epochRaan, &eccentricity,
                   &epochArgPerigee,&epochMeanAnomaly,&epochMeanMotion,
                   &decayRate,&decayRateDot,&bStarCoeff,
                   &epochOrbitNum,&ephemerisType);

        if (error)
        {
            doBeep(); nl(); reverseBlink();

            if (error == 1 || error == 2)
                printf(" %s: check sum error in TLE set, line %d \n",
                       satName,error);

            if (error == 3)
                printf(" %s: no matching satellite name or object number \n",
                       satName);

            if (error == 4)
                printf(" Specify one satellite \n");

            normal(); nl();

            if (batchModeFlag || quickStartFlag)
                exit(-1);
        }
    }

    sprintf(defSat,"%ld",satNum);
    strcpy(stsName,satName);
    upperCase(stsName);

    satTypeFlag = BASIC;                               /* restore satTypeFlag */

    if (!strncmp(stsName,"STS",3))
        satTypeFlag = STS;

    if (!strncmp(stsName,"MIR",3) && (int) strlen(stsName) == 3)
        satTypeFlag = MIR;

    if (satTypeFlag == STS)
    {
        error1   = TRUE;
        firstTry = TRUE;

        while (error1)
        {
            if (batchModeFlag)
                strcpy(elementType,batchTleType);

            else
            {
                if (quickStartFlag)
                    *elementType = '\0';

                else
                {
                    if (firstTry)
                        printf("Element set type              ");
                    else
                        printf("Element set type (NASA,NORAD) ");

                    printf("<%5s> : ",defSetType);
                    mGets(elementType,80,stdin);
                    firstTry = FALSE;
                }
            }

            if (!strlen(elementType))
                sprintf(elementType,"%s",defSetType);
            else
                upperCase(elementType);

            error1 = (!strcmp(elementType,"NASA") || 
                      !strcmp(elementType,"NORAD")) ? FALSE : TRUE;

            if ((batchModeFlag || quickStartFlag) && error1)
            {
                doBeep(); nl(); reverseBlink();
                printf(" Type of element set specified incorrectly \n");
                normal(); nl();
                exit(-1);
            }

            if (!batchModeFlag && !quickStartFlag && error1)
                doBeep();
        }
    }

    else
        sprintf(elementType,"%s",defSetType);

    getSatFileName();                                   /* remove blanks etc. */
    changeElementUnits();
    checkPropModelType();

    if (realTime-elsetEpoch > 0.0)
        sprintf(updateString,"%.1f days ago",realTime-elsetEpoch);
    else
        sprintf(updateString,"%.1f days ahead",elsetEpoch-realTime);

    if (verboseFlag)
    {
        printElements();

        if (geoSyncFlag)
        {
            if (geoSyncFlag == GEOSYNC)
                printf("Geosynchronous satellite\n");
            else
                printf("Geostationary satellite\n");

            nl();
        }

        if (satCrashFlag)
        {
            reverseBlink();

            if (satCrashFlag)
                printf(" Satellite has crashed already \n");

            normal();
            nl();
        }
    }

    else
    {
        if (!batchModeFlag && !quickStartFlag)
        {
            sprintf(str,"%ld, %s",satNum,satName);
            printf("Last update for %-21s : ",str);
            printf("%s (#%d, Orbit %ld)\n\n",
                    updateString,elementSet,epochOrbitNum);
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* changeElementUnits: changes units of Keplerian elements etc.               */
/*                                                                            */
/******************************************************************************/

void changeElementUnits()

{
    double epochDayTime, epochH, epochM, epochS, epochSec;
    int    epochYear, epochMonth, epochMonthDay, epochD, epochHour, epochMin;

    elsetEpochDay = epochDay;
    epochYear     = (int) (epochDay  / 1000.0);
    epochDay     -= (double) epochYear * 1000.0;
    elsetEpoch    = epochDay + (double) getDayNum(0,1,epochYear);
    epochD        = (int) epochDay;
    epochDayTime  = epochDay - (double) epochD;
    epochDay     += (double) getDayNum(0,1,epochYear);

    epochH        = 24.0 * epochDayTime;
    epochM        = 60.0 * modf(epochH,&epochH);
    epochS        = 60.0 * modf(epochM,&epochM);

    if (fabs(epochS - 60.0) < 5.0e-4)
    {
        epochS  = 0.0;
        epochM += 1.0;

        if (fabs(epochM - 60.0) < 1.0e-4)
        {
            epochM  = 0.0;
            epochH += 1.0;

            if (fabs(epochH - 24.0) < 1.0e-4)
            {
                epochH  = 0.0;
                epochD += 1;
            }
        }
    }

    epochHour = (int) epochH;
    epochMin  = (int) epochM;
    epochSec  = epochS;

    calendar(epochYear,epochD,&epochMonth,&epochMonthDay);

    sprintf(epochString,"%02d%3s%02d  %02d:%02d:%06.3f UTC",
            epochMonthDay,monthName(epochMonth),epochYear,
            epochHour,epochMin,epochSec);

    inclination      *= CDR;
    epochRaan        *= CDR;
    epochArgPerigee  *= CDR;
    epochMeanAnomaly *= CDR;

    cosInclination    = cos(inclination);
    sinInclination    = sin(inclination);

    geoSyncFlag = (fabs(epochMeanMotion - 1.0) < GEOSYNCMOT) ? GEOSYNC : FALSE;

    if (geoSyncFlag)
        geoSyncFlag = (fabs(inclination) < GEOSTATINC) ? GEOSTAT : GEOSYNC;

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSatModes: reads modes of the specified satellite                        */
/*                                                                            */
/******************************************************************************/

void getSatModes(satelliteName)

char *satelliteName;

{
    double cosAttLat, sinAttLat, cosAttLong, sinAttLong;
    long   objNum, objNumber;
    int    readLines, foundSat, foundMode, gotMode;
    char   str[40], strSat[120], modeLine[120], lDate[12];
    FILE   *modeFile;

    foundSat     = FALSE;
    foundMode    = FALSE;
    gotMode      = FALSE;
    launchFlag   = FALSE;
    launchEpoch  = 0.0;
    attitudeFlag = FALSE;
    attLat       = 0.0;
    attLong      = 0.0;
    perigeePhase = 0.0;
    maxPhase     = MAXPHASE;
    numFreqs     = 0;
    numModes     = 0;
    objNum       = 0L;
    readLines    = 0;

    *fullSatAlias = '\0';
    *satAlias     = '\0';
    sprintf(modesDat,"%s/%s/%s",strpHome,DATA,MODES);
    sprintf(str,"%s",satelliteName);
    objNumber = atol(str);

    if ((modeFile = fopen(modesDat,"r")) != NULL)
    {
        while (fgets(modeLine,100,modeFile) && !gotMode)
        {
            modeLine[strlen(modeLine) - 1] = '\0';
            truncBlanks(modeLine);

            if (!foundMode)
            {
                if (!strncmp(modeLine,"Satellite:",10))
                {
                    clipString(modeLine,10);
                    sprintf(strSat,"%s",modeLine);
                }

                if (!strncmp(modeLine,"Object:",7))
                {
                    clipString(modeLine,7);
                    sscanf(modeLine,"%ld",&objNum);
                }

                if (!strncmp(str,strSat,strlen(strSat)) || 
                    objNum == objNumber)
                {
                    readLines++;

                    if (objNum != objNumber)
                        readLines = 2;

                    foundSat = TRUE;
                }

                if (foundSat && readLines == 2)
                    foundMode = TRUE;
            }

            if (foundMode)
            {
                if (!strncmp(modeLine,"Alias:",6))
                {
                    clipString(modeLine,6);
                    sprintf(fullSatAlias,"%s",modeLine);
                    strncpy(satAlias,fullSatAlias,15);

                    if (verboseFlag)
                        printf("Alias name        : %s\n",fullSatAlias);
                }

                if (!strncmp(modeLine,"Launch:",7))
                {
                    clipString(modeLine,7);
                    sscanf(modeLine,"%s %d:%d:%d",
                        lDate,&launchHour,&launchMin,&launchSec);

                    sprintf(launchString,"%s  %02d:%02d:%02d.000 UTC",
                        lDate,launchHour,launchMin,launchSec);

                    dummyi = decodeDate(lDate,
                                 &launchDay,&launchMonth,&launchYear);

                    launchFlag = TRUE;

                    if (verboseFlag)
                        printf("Launch date/time  : %s\n",launchString);
                }

                if (!strncmp(modeLine,"Attitude:",9))
                {
                    clipString(modeLine,9);
                    sscanf(modeLine,"%lf %lf",&attLong,&attLat);
                    attitudeFlag = TRUE;

                    if (verboseFlag)
                    {
                        printf("Attitude          : ");
                        printf("%.3f %.3f deg\n",attLong,attLat);
                    }
                }

                if (!strncmp(modeLine,"Frequency:",10) && numFreqs < MAXFREQS)
                {
                    clipString(modeLine,10);

                    sscanf(modeLine,"%s %s %lf %lf %lf %lf",
                        freqs[numFreqs].downlinkMode,
                        freqs[numFreqs].uplinkMode,
                        &freqs[numFreqs].downlink,
                        &freqs[numFreqs].uplink,
                        &freqs[numFreqs].bandwidth,
                        &freqs[numFreqs].sign);

                    if (verboseFlag)
                    {
                        printf("Frequency #%-2d     : ",numFreqs+1);
                        printf("%-4s %-4s %10.4f %10.4f %6.1f %3.0f\n",
                            freqs[numFreqs].downlinkMode,
                            freqs[numFreqs].uplinkMode,
                            freqs[numFreqs].downlink,
                            freqs[numFreqs].uplink,
                            freqs[numFreqs].bandwidth,
                            freqs[numFreqs].sign);
                    }

                    freqs[numFreqs].downlink  *= CMHZHZ;
                    freqs[numFreqs].uplink    *= CMHZHZ;
                    freqs[numFreqs].bandwidth *= CKHZHZ;
                    numFreqs++;
                }

                if (!strncmp(modeLine,"Perigee phase:",14))
                {
                    clipString(modeLine,14);
                    sscanf(modeLine,"%lf",&perigeePhase);

                    if (verboseFlag)
                        printf("Perigee phase     : %.6f MHz\n",perigeePhase);
                }

                if (!strncmp(modeLine,"Max phase:",10))
                {
                    clipString(modeLine,10);
                    sscanf(modeLine,"%lf",&maxPhase);

                    if (verboseFlag)
                        printf("Max phase         : %.6f\n",maxPhase);
                }

                if (!strncmp(modeLine,"Mode:",5) && numModes < MAXMODES)
                {
                    clipString(modeLine,5);

                    sscanf(modeLine,"%s %lf - %lf",
                        modes[numModes].modeStr,
                        &modes[numModes].minPhase,
                        &modes[numModes].maxPhase);

                    if (verboseFlag)
                    {
                        printf("Mode              : %-2s  %3.0f - %3.0f\n",
                        modes[numModes].modeStr,
                        modes[numModes].minPhase,
                        modes[numModes].maxPhase);
                    }

                    numModes++;
                }

                if (strlen(modeLine) <= 2)
                    gotMode = TRUE;
            }
        }

        fclose(modeFile);

        if (verboseFlag) nl();

        if (launchFlag)
        {
            launchDate  = getDayNum(launchDay,launchMonth,launchYear);
            launchTime  = (double) (launchHour+launchMin/60.0+launchSec/3600.0);
            launchEpoch = (double) launchDate + launchTime/24.0;
        }

        if (attitudeFlag)
        {
            attLat     *= CDR;
            attLong    *= CDR;

            cosAttLat   = cos(attLat);
            sinAttLat   = sin(attLat);
            cosAttLong  = cos(attLong);
            sinAttLong  = sin(attLong);

            satAttGl[0] = cosAttLong * cosAttLat;
            satAttGl[1] = sinAttLong * cosAttLat;
            satAttGl[2] = sinAttLat;
        }
    }

    if (numFreqs)
    {
        strcpy(downlinkMode,freqs[0].downlinkMode);
        strcpy(uplinkMode,freqs[0].uplinkMode);
        downlinkFreq     = freqs[0].downlink;
        uplinkFreq       = freqs[0].uplink;
        xponderBandwidth = freqs[0].bandwidth;
        xponderSign      = freqs[0].sign;
    }

    else
    {
        strcpy(downlinkMode,"FM");
        strcpy(uplinkMode,"NONE");
        downlinkFreq     = DOWNLINKFREQ * CMHZHZ;
        uplinkFreq       = UPLINKFREQ * CMHZHZ;
        xponderBandwidth = ZERO;
        xponderSign      = ZERO;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSiteParams: reads site parameters from input file 'cities.dat'          */
/*                                                                            */
/******************************************************************************/
 
void getSiteParams()

{
    double cosSiteLat, sinSiteLat, cosSiteLng, sinSiteLng;
    double sqf, a, c, s, h;
    int    i, k, error, siteNameLen, foundSite;
    char   siteNameStr[80], saveStr[80], ctyStr[80], str[80];

    maidenHeadFlag = FALSE;
    foundSite      = FALSE;

    while (!foundSite)
    {
        if (batchModeFlag)
            strcpy(siteNameStr,batchSiteName);

        else
        {
            if (quickStartFlag)
                *siteNameStr = '\0';

            else
            {
                nl();
                printf("Ground station <%20s> : ",defSite);
                mGets(siteNameStr,80,stdin);
            }
        }

        if (!strlen(siteNameStr))
            sprintf(siteNameStr,"%s",defSite);

        upperCase(siteNameStr);
        siteNameLen = strlen(siteNameStr);

        k = 0;

        for (i = 0; i < numCities; i++)        /* check for ambiguous entries */
        {
            if (k == 1)
                strcpy(saveStr,ctyStr);

            strcpy(str,city[i].cty);
            upperCase(str);

            if (!strncmp(siteNameStr,str, (unsigned int) siteNameLen))
            {
                strcpy(ctyStr,city[i].cty);

                if (k == 1)
                    printf("\n%s\n",saveStr);

                if (k >= 1)
                    printf("%s\n",city[i].cty);

                k++;
            }
        }

        i = 0;

        do
        {
            strcpy(str,city[i].cty);
            upperCase(str);

            if (k == 1 && !strncmp(str,siteNameStr, (unsigned int) siteNameLen))
            {
                foundSite = TRUE;

                strcpy(fullSiteName,city[i].cty);
                strncpy(siteName,city[i].cty,15);
                shortString(siteName,fullSiteName,15);

                siteLat  = city[i].lat;               /* geodetic location    */
                siteLong = city[i].lng;
                siteAlt  = city[i].alt * CMKM;

                getMaidenHead(siteLat,siteLong,maidenHead);
            }

            i++;
        }
        while (i < numCities && !foundSite);

        if (!foundSite)
        {
            error = getPosFromMaidenHead(siteNameStr,&siteLat,&siteLong);

            if (error)
            {
                doBeep(); nl(); reverseBlink();

                if (k == 0)
                    printf(" %s: no matching ground station or locator error ",
                        siteNameStr);
                else
                    printf(" Specify one ground station ");

                normal(); nl();

                if (batchModeFlag || quickStartFlag)
                {
                    nl(); exit(-1);
                }
            }

            else
            {
                maidenHeadFlag = TRUE;
                foundSite      = TRUE;
                strcpy(siteName,siteNameStr);
                strcpy(fullSiteName,siteNameStr);
                strcpy(siteNameID,siteNameStr);
                *maidenHead = '\0';
                siteAlt = 0.0;

                getGroundTrack(siteLat,siteLong,
                    &dummyd,&dummyd,&gndTrkDist,gndTrkDir,gndTrkCity);

                if (!batchModeFlag)
                    printf("%s is %.1f km %s of %s\n\n",
                        siteName,gndTrkDist,gndTrkDir,gndTrkCity);
            }
        }
    }

    if (!maidenHeadFlag)
    {
        strcpy(siteNameID,fullSiteName);
        getSiteNameID(siteNameID);
    }

    groundStation[0].ltd = siteLat;
    groundStation[0].lng = siteLong;
    strcpy(groundStation[0].gndStnID,siteNameID);

    if (verboseFlag)
    {
        nl();
        printf("Ground station    : %20s\n",fullSiteName);
        printf("Latitude          : %20.6f deg ",fabs(siteLat*CRD));

        (siteLat >= 0.0) ? printf("N\n") : printf("S\n");

        printf("Longitude         : %20.6f deg ",fabs(siteLong*CRD));

        (siteLong >= 0.0) ? printf("W\n") : printf("E\n");

        printf("Altitude          : %20.6f m\n",siteAlt/CMKM);
        printf("Maidenhead locator: %20s\n",maidenHead);
        nl();
    }

    else
    {
        if (!batchModeFlag && !quickStartFlag && !maidenHeadFlag && 
            strcmp(fullSiteName,defSite))
            printf("%-38s: %s\n\n",fullSiteName,maidenHead);
    }

    /* now calculate the site vector in geocentric coordinates */
    /* this is needed only for the squint angle calculations   */

    cosSiteLat = cos(siteLat);
    sinSiteLat = sin(siteLat);
    cosSiteLng = cos(siteLong);
    sinSiteLng = sin(siteLong);

    sqf = SQR(1.0 - EARTHFLAT);
    a   = EARTHRADIUS;
    c   = 1.0 / sqrt(SQR(cosSiteLat) + sqf * SQR(sinSiteLat));
    s   = sqf * c;
    h   = siteAlt * CMKM;

    siteVecGl[0] = (a*c + h) * cosSiteLat * cosSiteLng;
    siteVecGl[1] = (a*c + h) * cosSiteLat * sinSiteLng;
    siteVecGl[2] = (a*s + h) * sinSiteLat;

    return;
}
 
/******************************************************************************/
/*                                                                            */
/* getTimeParams: reads additional parameters for orbit calculations          */
/*                                                                            */
/******************************************************************************/

void getTimeParams(timeArg)

double timeArg;

{
    double hour;
    long   sysDayNum;
    int    sysYear, sysMonth, sysDay, sysYearDay, sysHour, sysMin, sysSec;
    int    error;
    char   sysDate[20], sysTime[20];
 
    timeArg  += timeZone;
    sysDayNum = (long) timeArg;
    convertTime(timeArg,&dummyi,&sysHour,&sysMin,&sysSec);
    getDate(sysDayNum,&sysYear,&sysMonth,&sysDay,&sysYearDay);
    sprintf(sysDate,"%02d%3s%02d",sysDay,monthName(sysMonth),sysYear%100);
    sprintf(sysTime,"%02d:%02d:%02d",sysHour,sysMin,sysSec);

    if (batchModeFlag)
    {
        if (!strcmp(batchStartDate,NOW))
            strcpy(dispStr,YES);
    }

    else
    {
        printf("Start time  %7s %8s %-4s <Y> ? ",sysDate,sysTime,timeZoneStr);

        mGets(dispStr,80,stdin);
        upperCase(dispStr);
    }

    if (strlen(dispStr) && strncmp(dispStr,YES,1))
    {
        error = TRUE;

        while(error)
        {
            if (batchModeFlag)
                strcpy(dispStr,batchStartDate);

            else
            {
                printf("Start date (%-4s)           <%7s> : ",
                    timeZoneStr,sysDate);
                mGets(dispStr,80,stdin);

                if (!strlen(dispStr))
                    sprintf(dispStr,"%s",sysDate);
            }

            error = decodeDate(dispStr,&sysDay,&sysMonth,&sysYear);

            if (error) doBeep();
        }

        error = TRUE;

        while(error)
        {
            if (batchModeFlag)
                strcpy(dispStr,batchStartTime);

            else
            {
                printf("Start time (%-4s)          <%8s> : ",
                    timeZoneStr,sysTime);
                mGets(dispStr,80,stdin);

                if (!strlen(dispStr))
                    sprintf(dispStr,"%s",sysTime);
            }

            error = decodeTime(dispStr,&sysHour,&sysMin,&sysSec);

            if (error) doBeep();
        }
    }

    defStepTime = 1.0 / epochMeanMotion * SPD / 125.0;
    defStepTime = (double) (((int) (defStepTime / 10.0)) * 5.0);

    if (!shortPredFlag)
    {
        if (sunTransitFlag)
            stepTime = 0.25;

        else
        {
            if (batchModeFlag)
            {
                if (!strcmp(batchStepTime,AUTO))
                    *dispStr = '\0';
                else
                    strcpy(dispStr,batchStepTime);
            }

            else
            {
                printf("Time step [s]                 <%5.0f> : ",defStepTime);
                mGets(dispStr,80,stdin);
            }

            stepTime = (!strlen(dispStr)) ? defStepTime : atof(dispStr);
        }
    }

    if (batchModeFlag)
        strcpy(dispStr,batchDuration);

    else
    {
        printf("Duration [d]                   <%4.1f> : ",defDuration);
        mGets(dispStr,80,stdin);
    }

    duration   = (!strlen(dispStr)) ? defDuration : atof(dispStr);
    startTime  = (double) getDayNum(sysDay,sysMonth,sysYear);          /* [d] */
    hour       = (double) (sysHour + sysMin/60.0 + sysSec/3600.0);     /* [h] */
    startTime += hour / 24.0 - timeZone;
    stepTime  /= SPD;                                                  /* [d] */
    stopTime   = startTime + duration;                                 /* [d] */

    if (!shortPredFlag)
    {
        if (sunTransitFlag)
            minElevation = 0.0;

        else
        {
            if (batchModeFlag)
                strcpy(dispStr,batchMinElev);

            else
            {
                printf("Minimum elevation [deg]        ");
                printf("<%4.1f> : ",defMinElevation);
                mGets(dispStr,80,stdin);
            }

            minElevation = (!strlen(dispStr)) ? defMinElevation : atof(dispStr);
            minElevation *= CDR;
        }
    }

    return;
}
 
/******************************************************************************/
/*                                                                            */
/* printMode: prints phase-dependent mode of satellite into output file       */
/*                                                                            */
/******************************************************************************/

void printMode(outFile)

FILE *outFile;

{
    getMode();
    fprintf(outFile," %2s",modeString);
    return;
}
 
/******************************************************************************/
/*                                                                            */
/*  readSatList: reads names of satellites from the input file                */
/*                                                                            */
/******************************************************************************/

void readSatList()

{
    int  i, j, error, foundSat;
    char data[80], satListName[80], satListFileName[80], inputFileName[120];
    char str1[80], str2[80];
    FILE *inputFile;

    error = TRUE;

    while (error)
    {
        if (quickStartFlag)
            *satGroup = '\0';

        else
        {
            printf("Enter satellite group    <%10s> : ",defSatGroup);
            mGets(satGroup,80,stdin);
        }

        if (!strlen(satGroup))
            sprintf(satGroup,"%s",defSatGroup);

        lowerCase(satGroup);
        sprintf(data,"%s/%s",strpHome,DATA);
        sprintf(satListFileName,"satlist_%s.dat",satGroup);
        sprintf(inputFileName,"%s/%s",data,satListFileName);

        if ((inputFile = fopen(inputFileName,"r")) == NULL)
        {
            doBeep(); nl(); reverseBlink();
            printf(" File '%s' not found \n",satListFileName);
            normal(); nl();
            error = TRUE;
            fclose(inputFile);

            if (quickStartFlag)
                exit(-1);
        }

        else
            error = FALSE;
    }

    if (!quickStartFlag)
        printf("\nreading satellite names for group '%s' ...\n",satGroup);

    upperCase(satGroup);

    i = 0;

    while (fgets(satListName,80,inputFile))
    {
        satListName[strlen(satListName) - 1] = '\0';
        truncBlanks(satListName);
        foundSat = FALSE;

        for (j = 0; j < i; j++)                 /* filter out duplicate names */
        {
            strcpy(str1,satel[j].satelName);
            strcpy(str2,satListName);
            upperCase(str1);
            upperCase(str2);

            if (!strcmp(str1,str2))
            {
                printf("duplicate entry: %s\n",str1);
                foundSat = TRUE;
            }
        }

        if (!foundSat)
        {
            cleanSatName(satListName);
            strcpy(satel[i].satelName,satListName);
            i++;
        }
    }

    fclose(inputFile);
    numSats  = i;
    foundSat = FALSE;

    for (i = 0; i < numSats; i++)             /* check if menu sat is in list */
    {
        if (!strncmp(satName,satel[i].satelName,strlen(satel[i].satelName)) || 
             satNum == atol(satel[i].satelName))
            foundSat = TRUE;
    }

    if (!foundSat)                            /* if not, add menu sat to list */
    {
        sprintf(satel[numSats].satelName,"%ld",satNum);
        numSats++;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* saveSatElements: saves all Keplerian elements and other parameters in a    */
/*                  structure                                                 */
/*                                                                            */
/******************************************************************************/

void saveSatElements()

{
    int  i, j, n, error, grandError;
    long satNumSelect;
    char str[80], stsStr[80];

    grandError   = FALSE;
    satNumSelect = satNum;
    n = 0;

    if (!quickStartFlag)
        printf("loading element sets for %d satellites ...\n",numSats);

    for (i = 0; i < numSats; i++)
    {
        strcpy(str,satel[i].satelName);
        strcpy(stsStr,str);
        upperCase(stsStr);

        satTypeFlag = BASIC;                           /* restore satTypeFlag */

        if (!strncmp(stsStr,"STS",3))
            satTypeFlag = STS;

        if (!strncmp(stsStr,"MIR",3) && (int) strlen(stsStr) == 3)
            satTypeFlag = MIR;

        if (satTypeFlag == STS && !strcmp(str,satName))
            sprintf(elementFile,"%s",defSetSTS);
        else
            sprintf(elementFile,"%s",defSetTLE);

        error = readTle(FALSE,str,&satNum,&elementSet,
                   &epochDay,&inclination,&epochRaan,&eccentricity,
                   &epochArgPerigee,&epochMeanAnomaly,&epochMeanMotion,
                   &decayRate,&decayRateDot,&bStarCoeff,
                   &epochOrbitNum,&ephemerisType);

        /* if object number was specified on string 'str', readTle() comes    */
        /* back with the satellite name on the same variable 'str'            */

        strcpy(stsStr,str);      /* need to restore satTypeFlag, since object */
        upperCase(stsStr);       /* number might have been specified          */
                              
        satTypeFlag = BASIC;

        if (!strncmp(stsStr,"STS",3))
            satTypeFlag = STS;

        if (!strncmp(stsStr,"MIR",3) && (int) strlen(stsStr) == 3)
            satTypeFlag = MIR;

        if (error)
        {
            doBeep(); reverseBlink(); nl();

            if (error == 1 || error == 2)
                printf(" %s: check sum error in TLE set, line %d ",str,error);

            if (error == 3)
                printf(" %s: no matching satellite name or object number ",str);

            if (error == 4)
                printf(" %s: multiple entries ",str);

            nl(); normal(); nl();

            grandError++;
        }

        else
        {
            getSatModes(&satel[i].satelName[0]);
            changeElementUnits();
            getSatPrecession();
            checkPropModelType();

            strcpy(sat[n].satellite,str);
            strcpy(sat[n].satelliteAlias,satAlias);
            strcpy(sat[n].model,propModel);

            sat[n].satIdNum     = satNum;
            sat[n].elSet        = elementSet;

            sat[n].epDay        = epochDay;
            sat[n].elSetEp      = elsetEpoch;
            sat[n].launchEp     = launchEpoch;

            sat[n].epInc        = inclination;
            sat[n].epRaan       = epochRaan;
            sat[n].epEcc        = eccentricity;
            sat[n].epArgPer     = epochArgPerigee;
            sat[n].epMeanAnom   = epochMeanAnomaly;
            sat[n].epMeanMot    = epochMeanMotion;
            sat[n].epDecRate    = decayRate;
            sat[n].epDecRateDot = decayRateDot;
            sat[n].epBstar      = bStarCoeff;
            sat[n].epOrbNum     = epochOrbitNum;
            sat[n].ephType      = ephemerisType;
            sat[n].modelType    = propModelType;

            sat[n].dwnlnkFrq    = downlinkFreq;
            sat[n].uplnkFrq     = uplinkFreq;
            sat[n].perigPh      = perigeePhase;
            sat[n].maxPh        = maxPhase;
            sat[n].numFr        = numFreqs;
            sat[n].numMd        = numModes;
            sat[n].attitudeFl   = attitudeFlag;
            sat[n].attLtd       = attLat;
            sat[n].attLng       = attLong;
            sat[n].attX         = satAttGl[0];
            sat[n].attY         = satAttGl[1];
            sat[n].attZ         = satAttGl[2];
            
            sat[n].semiMajAxis  = semiMajorAxis;
            sat[n].raanPr       = raanPrec;
            sat[n].periPr       = perigeePrec;
            sat[n].refOrb       = refOrbit;

            sat[n].cosInc       = cosInclination;
            sat[n].sinInc       = sinInclination;

            sat[n].satTypeFl    = satTypeFlag;
            sat[n].launchFl     = launchFlag;
            sat[n].prelaunchFl  = FALSE;
            sat[n].preorbitFl   = FALSE;
            sat[n].geosyncFl    = geoSyncFlag;
            sat[n].crashFl      = satCrashFlag;

            sat[n].statusFl     = (satNumSelect == satNum) ? 
                                   SELECT : FALSE;

            if (numFreqs)
            {
                for (j = 0; j < numFreqs; j++)
                {
                    strcpy(sat[n].satFreqs[j].downlinkMode,
                           freqs[j].downlinkMode);
                    strcpy(sat[n].satFreqs[j].uplinkMode,
                           freqs[j].uplinkMode);
                    sat[n].satFreqs[j].downlink  = freqs[j].downlink; 
                    sat[n].satFreqs[j].uplink    = freqs[j].uplink;
                    sat[n].satFreqs[j].bandwidth = freqs[j].bandwidth;
                    sat[n].satFreqs[j].sign      = freqs[j].sign;
                }
            }

            if (numModes)
            {
                for (j = 0; j < numModes; j++)
                {
                    strcpy(sat[n].satModes[j].modeStr,modes[j].modeStr);
                    sat[n].satModes[j].minPhase = modes[j].minPhase; 
                    sat[n].satModes[j].maxPhase = modes[j].maxPhase;
                }
            }

            n++;
        }
    }

    numSats = n;

    if (grandError)
    {
        printf("\nhit RETURN to continue ");
        mGets(str,80,stdin);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* printElements: prints orbital elements                                     */
/*                                                                            */
/******************************************************************************/

void printElements()

{
    nl();
    printf("Satellite name    : %20s\n",satName);
    printf("Satellite number  : %20ld\n",satNum);
    printf("Element set       : %20d\n",elementSet);
    printf("Ephemeris type    : %20d\n",ephemerisType);
    printf("Epoch             : %20.9f d        ",elsetEpochDay);
    printf("%s\n",epochString);
    printf("Mean anomaly      : %20.9f deg\n",epochMeanAnomaly * CRD);
    printf("Arg of perigee    : %20.9f deg\n",epochArgPerigee * CRD);
    printf("RAAN              : %20.9f deg\n",epochRaan * CRD);
    printf("Inclination       : %20.9f deg\n",inclination * CRD);
    printf("Eccentricity      : %20.9f\n",eccentricity);
    printf("Semimajor axis    : %20.9f km\n",semiMajorAxis);
    printf("Perigee height    : %20.9f km\n",perigeeHeight);
    printf("Apogee height     : %20.9f km\n",apogeeHeight);
    printf("Mean motion       : %20.9f rev/d\n",epochMeanMotion);
    printf("Decay rate        : %20.9f rev/d^2\n",decayRate);
    printf("Decay rate dot    : %20.9f rev/d^3\n",decayRateDot);
    printf("BSTAR drag coeff. : %20.9f\n",bStarCoeff);
    printf("Orbit             : %20ld\n",epochOrbitNum);
    printf("Propagation model : %20s\n",propModel);
    nl();

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satread.c                                            */
/*                                                                            */
/******************************************************************************/
