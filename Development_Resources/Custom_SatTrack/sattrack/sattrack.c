/******************************************************************************/
/*                                                                            */
/*  Title       : sattrack.c                                                  */
/*  Author      : Manfred Bester                                              */
/*  Date        : 24Feb92                                                     */
/*  Last change : 06May96                                                     */
/*                                                                            */
/*  Synopsis    : Satellite orbit prediction and live tracking program.       */
/*                V3.1.5 patch.                                               */
/*                                                                            */
/*  Resources   : cities.dat       modes.dat           tlex.dat               */
/*                defaults.dat     satlist_nnn.dat                            */
/*                                                                            */
/*                                                                            */
/*  SatTrack is Copyright (c) 1992, 1993, 1994, 1995, 1996 Manfred Bester.    */
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
#include <sys/types.h>
#include <signal.h>
#include <memory.h>

#ifdef TERMIO
#include <sys/termio.h>
#else
#include <sys/termios.h>
#endif

#ifdef IOCTL
#include <sys/ioctl.h>
#endif

#ifdef SVR4
#include <unistd.h>
#endif

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "satglobalspx.h"
#include "satglobals.h"
#include "satglobalsp.h"
#include "satstrings.h"
#include "sattrack.h"

#ifdef HPTERM
#include "hpterm.h"               /* definitions of hpterm macros             */
#else
#include "vt100.h"                /* definitions of VT100 macros              */
#endif

/******************************************************************************/
/*                                                                            */
/*  global variables used with the main program                               */
/*                                                                            */
/******************************************************************************/

double riseAzimuth, setAzimuth;        /* [rad]                               */
double maxAzimuth;                     /* [rad]                               */
double maxElevation, relMaxElev;       /* [rad]                               */
double maxRange;                       /* range  at MEL [km]                  */
double maxHeight;                      /* height at MEL [km]                  */
double acceptableElevation;

double curTime, tmpTime, stepTime;     /* time [d]                            */
double orbitTime;                      /* time [d]                            */
double dT, dT2;                        /* time [d]                            */
double predTime, predStopTime;         /* time [d]                            */
double fastStepTime, medStepTime;      /* time [d]                            */
double slowStepTime;
double nextRiseTime, nextSetTime;      /* rise and set times [d]              */
double nextMaxTime;
double nextStartElTime, nextEndElTime; /* The start and end time of the acceptable elevation range */
double maxDays;

double sunProxLimit;
double dAziSatSun, dEleSatSun;
double cUnit;

long   riseOrbitNum, printOrbitNum, lastYearSec;
long   trackInt, singleSatUpdateInt, multiSatUpdateInt;

int    sysYear, sysMonth, sysDay, sysYearDay, sysHour, sysMin, sysSec;
int    ns, sameOrbitPass, foundSatFlag, foundRiseFlag, foundSetFlag, foundStartElFlag, foundEndElFlag;
int    startFlag, predictionFlag, writeFlag, firstMultiSat, firstMultiSatCalc;
int    finishPass, newPassFlag, dispUpdateFlag, switchSatFlag, visibPassesFlag;
int    didSwitchFlag, reinitSingleSat, reinitMultiSat, key, keyBoard;

char   fileName[100], sysComm[120], buf[80], sUnit[10], font[30];
char   str[80], satNameStr[80];

/******************************************************************************/
/*                                                                            */
/* killProgram: kills program                                                 */
/*                                                                            */
/******************************************************************************/

void killProgram()

{
    if (graphicsOpenFlag)
        QuitGraphics();

    gotoXY(1,1);
    clearScreen();
    normal();
    fflush(stdout);
    exit(-1);
}

/******************************************************************************/
/*                                                                            */
/* makeOutputFile: makes output file for predictions                          */
/*                                                                            */
/******************************************************************************/

void makeOutputFile()

{
    if (predictionFlag){
        outFile = stdout;
    } else {
        if (writeFlag)
        {
            sprintf(fileName,"%s/%s/%s",strpHome,PRED,satFileName);

            if ((outFile = fopen(fileName,"w")) == NULL)
            {
                printf("\ncannot open output file %s\n\n",fileName);
                exit(-1);
            }

            if (!batchModeFlag)
                printf("\ndata are written into '%s'\n\n",fileName);
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* printPredHeader: prints prediction header                                  */
/*                                                                            */
/******************************************************************************/

void printPredHeader()

{
    checkPropModelType();
    getMaidenHead(siteLat,siteLong,maidenHead);

    fprintf(outFile,"\n%s %s %s %s\n\n",
        callSign,sattrName,sattrVersion,predHeader);
    fprintf(outFile,"Satellite #%-6ld : %s",satNum,satName);

    if (strlen(satAlias))
        fprintf(outFile," (%s)",fullSatAlias);

    fprintf(outFile,"\n");
    fprintf(outFile,"Data File         : %s\n",elementFile);

    if (satTypeFlag == STS)
        fprintf(outFile,"Element Set Type  : %s\n",elementType);

    fprintf(outFile,"Element Set Number: %d ",elementSet);
    fprintf(outFile,"(Orbit %ld)\n",epochOrbitNum);
    fprintf(outFile,"Element Set Epoch : %s   ",epochString);
    fprintf(outFile,"(%s)\n",updateString);

    if (launchFlag)
        fprintf(outFile,"Launch Date/Time  : %s\n",launchString);

    if (satTypeFlag == STS)
        fprintf(outFile,"Orbit Geometry    : %.2f nm x %.2f nm at %.3f deg\n",
            perigeeHeight*CKMNM,apogeeHeight*CKMNM,inclination*CRD);
    else
        fprintf(outFile,"Orbit Geometry    : %.2f km x %.2f km at %.3f deg\n",
            perigeeHeight,apogeeHeight,inclination*CRD);

    fprintf(outFile,"Propagation Model : %s\n",propModel);
    fprintf(outFile,"Ground Station    : %s",fullSiteName);

    if (!maidenHeadFlag)
        fprintf(outFile,"   ---   %s",maidenHead);

    fprintf(outFile,"\n");

    fprintf(outFile,"Time Zone         : %s (%+.2f h)\n",
            timeZoneStr,timeZone*24.0);

    if (!shortPredFlag && !sunTransitFlag && !visibPassesFlag)
        fprintf(outFile,"Downlink Frequency: %.4f MHz\n",downlinkFreq*CHZMHZ);

    if (!shortPredFlag && sunTransitFlag)
        fprintf(outFile,"\nPASSES NEAR OR ACROSS THE SOLAR DISK:\n\n");

    if (shortPredFlag && !sunTransitFlag && visibPassesFlag)
        fprintf(outFile,"\nVISIBLE PASSES:\n\n");

    return;
}

/******************************************************************************/
/*                                                                            */
/* doLongPrediction: performs orbit prediction in long format                 */
/*                                                                            */
/******************************************************************************/

void doLongPrediction()

{
    slowStepTime  = 1.0 / epochMeanMotion / 750.0;
    medStepTime   = slowStepTime * 3.0;
    fastStepTime  = medStepTime * 15.0;

    firstLine     = TRUE;
    firstPage     = TRUE;
    sameOrbitPass = FALSE;
    initNorad     = TRUE;

    if (curTime < realTime)
        predTime = curTime;
    else
        predTime = (elsetEpoch > curTime) ? elsetEpoch : curTime;

    predStopTime = predTime + (stopTime - startTime);

    do
    {
        getSiderealTime(predTime);
        getSiteVector();

        getNoradStateVector(predTime);
        getStateVector(predTime);

        getAziElev(SAT);

        if (satElevation >= minElevation && predTime >= startTime)
        {
            getRange();
            getShuttleOrbit();
            getSunVector(0);
            getAziElev(SUN);
            getSubSatPoint(SAT);
            getDoppler();
            getPathLoss();
            getSquintAngle();
            getSunPhaseAngle(); 
            getPhase();
            getMode();

            if (sunTransitFlag)
            {
                dAziSatSun = sunAzimuth - satAzimuth;
                dEleSatSun = sunElevation - satElevation;

                sunProximity = (fabs(dAziSatSun) < sunProxLimit &&
                                fabs(dEleSatSun) < sunProxLimit) ? TRUE : FALSE;

                sunTransit   = (fabs(dAziSatSun) < SUNDISKRAD &&
                                fabs(dEleSatSun) < SUNDISKRAD) ? TRUE : FALSE;
            }

            if (!sameOrbitPass || firstLine)
            {
                if (satTypeFlag == STS)
                {
                    cUnit = CKMNM;
                    sprintf(sUnit,"nm");
                    printOrbitNum = stsOrbitNum;
                }

                else
                {
                    cUnit = 1.0;
                    sprintf(sUnit,"km");
                    printOrbitNum = orbitNum;
                }

                if (printOrbitNum > 0 && 
                    (!sunTransitFlag || (sunTransitFlag && sunProximity)))
                {
                    if (!firstPage)
                        fprintf(outFile,"\f");

                    if ((firstPage && !writeFlag) || writeFlag)
                        printPredHeader();

                    firstPage = FALSE;

                    fprintf(outFile,"\n\n");
                    printDate(outFile,predTime+timeZone);
                    fprintf(outFile,"  ---  Orbit %ld\n\n",printOrbitNum);

                    fprintf(outFile,"  %4s",timeZoneStr);

                    if (launchFlag && satTypeFlag == STS)
                        fprintf(outFile,"          MET ");

                    if (sunTransitFlag)
                    {
                        fprintf(outFile,"    Sat Azi  Sat Ele");
                        fprintf(outFile,"    Sun-Sat  Azi Ele");
                    }

                    else
                    {
                        fprintf(outFile,"    Azimuth  Elev");
                    }

                    fprintf(outFile,"    Range");

                    if (!sunTransitFlag)
                    {
                        if (visibPassesFlag)
                        {
                            fprintf(outFile,"    Height");
                            fprintf(outFile,"  Sun Ang");
                            fprintf(outFile,"  V");
                        }

                        else
                        {
                            fprintf(outFile,"  Sun Ang  Doppler  Loss");

                            if (attitudeFlag)
                                fprintf(outFile,"  Squ Ang");

                            fprintf(outFile,"  Phs Md V");
                        }
                    }

                    fprintf(outFile,"\n");
                    fprintf(outFile,"      ");

                    if (launchFlag && satTypeFlag == STS)
                        fprintf(outFile,"              ");

                    if (sunTransitFlag)
                    {
                        fprintf(outFile,"     [deg]    [deg]");
                        fprintf(outFile,"         [arcmin]    ");
                    }

                    else
                        fprintf(outFile,"     [deg]  [deg]");

                    fprintf(outFile,"     [%2s]",sUnit);

                    if (!sunTransitFlag)
                    {
                        if (visibPassesFlag)
                        {
                            fprintf(outFile,"     [%2s]",sUnit);
                            fprintf(outFile,"    [deg]");
                        }

                        else
                        {
                            fprintf(outFile,"    [deg]   [kHz]   [dB]");

                            if (attitudeFlag)
                                fprintf(outFile,"    [deg]");
                        }
                    }

                    fprintf(outFile,"\n\n");
                    firstLine = FALSE;
                }

                if (writeFlag && !batchModeFlag && !sunTransitFlag)
                {
                    printf(".");
                    fflush(stdout);
                }
            }

            eclipseCode = satEclipse(satElevation);

            if (printOrbitNum > 0 && 
                (!sunTransitFlag || (sunTransitFlag && sunProximity)))
            {
                printTime(outFile,predTime+timeZone);

                if (launchFlag && satTypeFlag == STS)
                {
                    fprintf(outFile,"  ");
                    printMET(outFile,predTime-launchEpoch);
                }

                if (sunTransitFlag)
                {
                    fprintf(outFile,"  %7.3f",satAzimuth*CRD);
                    fprintf(outFile,"  %7.3f",satElevation*CRD);
                    fprintf(outFile,"   %8.3f",dAziSatSun*CRAM);
                    fprintf(outFile," %8.3f",dEleSatSun*CRAM);
                }

                else
                {
                    fprintf(outFile,"   %5.1f",satAzimuth*CRD);
                    fprintf(outFile,"  %5.1f",satElevation*CRD);
                }

                fprintf(outFile," %8.1f",satRange*cUnit);

                if (!sunTransitFlag)
                {
                    if (visibPassesFlag)
                    {
                        fprintf(outFile,"  %8.1f",satHeight*cUnit);
                        fprintf(outFile," %7.1f",sunPhaseAngle*CRD);
                        fprintf(outFile,"   %s",visibCode[eclipseCode]);
                    }

                    else
                    {
                        fprintf(outFile,"  %7.1f",sunPhaseAngle*CRD);
                        fprintf(outFile,"  %+6.2f",downlinkDopp*CHZKHZ);
                        fprintf(outFile,"  %5.1f",downlinkLoss);

                        if (attitudeFlag)
                            fprintf(outFile,"  %7.1f",squintAngle*CRD);

                        fprintf(outFile,"  %3.0f",satPhase);
                        printMode(outFile);
                        fprintf(outFile," %s",visibCode[eclipseCode]);
                    }

                    if (sunPhaseAngle < VISPHSLIMIT && 
                        satElevation  > VISELELIMIT && 
                        eclipseCode  == VISIBLE)
                        printf(" *");
                }

                else
                    fprintf(outFile," ");

                if (sunTransit)
                    fprintf(outFile,"  ###");

                fprintf(outFile,"\n");
                sameOrbitPass = TRUE;
            }
        }

        else
            sameOrbitPass = FALSE;

        predTime  += (satElevation < VLOWELEV) ? fastStepTime : stepTime;
        finishPass = (satElevation >= minElevation) ? TRUE : FALSE;
    }
    while (predTime <= predStopTime || finishPass);

    return;
}

/******************************************************************************/
/*                                                                            */
/* doShortPrediction: performs orbit prediction in short format               */
/*                                                                            */
/******************************************************************************/

void doShortPrediction()

{
    //printPredHeader();
    printf("SUCCESS\n");

    if (curTime < realTime)
        predTime = curTime;
    else
        predTime = (elsetEpoch > curTime) ? elsetEpoch : curTime;

    predStopTime = predTime + (stopTime - startTime);
    firstLine    = TRUE;
    lastDayNum   = 0;

    cUnit = (satTypeFlag == STS) ? CKMNM : 1.0;

    //fprintf(outFile,"\n\n");
    //fprintf(outFile," Date (%s)        ",timeZoneStr);
    //if (strlen(timeZoneStr) == 3) fprintf(outFile," ");
    //fprintf(outFile," Time (%s) of       ",timeZoneStr);
    //if (strlen(timeZoneStr) == 3) fprintf(outFile," ");

    /*if (visibPassesFlag)
    {
        fprintf(outFile,"  Azimuth at   Peak  Height  Vis Orbit");

        if (launchFlag && satTypeFlag == STS && writeFlag)
            fprintf(outFile,"   MET at AOS");

        fprintf(outFile,"\n");
        fprintf(outFile,"                Rise     Peak      Set");
        fprintf(outFile,"     Ris  Pk Set   Elev   at Pk\n");
    }

    else
    {
        fprintf(outFile,"Duration   Azimuth at  Peak  Vis Orbit");

        if (launchFlag && satTypeFlag == STS && writeFlag)
            fprintf(outFile,"   MET at AOS");

        fprintf(outFile,"\n");
        fprintf(outFile,"                 AOS      MEL      LOS     ");
        fprintf(outFile,"of Pass  AOS MEL LOS  Elev\n");
    }*/

    do
    {
        getNextPass();

        if (riseOrbitNum > 0)
        {
            if (visibPassesFlag)
            {
                if (eclipseRise == VISIBLE || eclipseMax == VISIBLE ||
                    eclipseSet  == VISIBLE)
                {
                    printDate(outFile,nextRiseTime+timeZone);
                    fprintf(outFile,"  ");
                    printTime(outFile,nextRiseTime+timeZone);
                    fprintf(outFile," ");
                    printTime(outFile,nextMaxTime+timeZone);
                    fprintf(outFile," ");
                    printTime(outFile,nextSetTime+timeZone);
                    fprintf(outFile,"   %3.0f %3.0f %3.0f",
                        riseAzimuth*CRD,maxAzimuth*CRD,setAzimuth*CRD);
                    fprintf(outFile,"   %4.1f",maxElevation*CRD);

                    if (maxElevation > MAXELELIMIT)
                        fprintf(outFile,"*");
                    else
                        fprintf(outFile," ");

                    fprintf(outFile," %6.0f",maxHeight*cUnit);
                    fprintf(outFile,"  %s%s%s",visibCode[eclipseRise],
                        visibCode[eclipseMax],visibCode[eclipseSet]);
                    fprintf(outFile," %5ld",riseOrbitNum);

                    if (launchFlag && satTypeFlag == STS && writeFlag)
                    {
                        fprintf(outFile," ");
                        printMET(outFile,nextRiseTime-launchEpoch);
                    }

                    fprintf(outFile,"\n");
                }
            }

            else
            {
                printDate(outFile,nextRiseTime+timeZone);
                printTime(outFile,nextRiseTime+timeZone);
                fprintf(outFile,"$");
                printTime(outFile,nextMaxTime+timeZone);
                fprintf(outFile,"$");
                printTime(outFile,nextSetTime+timeZone);
                fprintf(outFile,"$");
                printTime(outFile,nextStartElTime+timeZone);
                fprintf(outFile,"$");
                printTime(outFile,nextEndElTime+timeZone);
                fprintf(outFile,"$");
                printTime(outFile,nextSetTime-nextRiseTime);
                fprintf(outFile,"$%.0f$%.0f$%.0f",riseAzimuth*CRD,maxAzimuth*CRD,setAzimuth*CRD);
                fprintf(outFile,"$%.1f",maxElevation*CRD);

                //if (maxElevation > MAXELELIMIT)
                 //   fprintf(outFile,"*");

                fprintf(outFile,"$%s%s%s",visibCode[eclipseRise],
                    visibCode[eclipseMax],visibCode[eclipseSet]);
                fprintf(outFile,"$%5ld",riseOrbitNum);

                if (launchFlag && satTypeFlag == STS && writeFlag)
                {
                    fprintf(outFile," ");
                    printMET(outFile,nextRiseTime-launchEpoch);
                }

                fprintf(outFile,"\n");
            }

            firstLine = FALSE;
        }

        predTime = tmpTime + fastStepTime;
    }
    while (predTime <= predStopTime);

    return;
}

/******************************************************************************/
/*                                                                            */
/* doPrediction: run orbit prediction                                         */
/*                                                                            */
/******************************************************************************/

void doPrediction()

{
    makeOutputFile();

    if (predictionFlag || writeFlag)
    {
        getTimeParams(curTime);
        curTime = startTime;

        if (!batchModeFlag)
        {
            printf("\ncalculating passes of %s over %s ...\n",
                    satName,fullSiteName);
        }

        if (shortPredFlag)
            doShortPrediction();
        else
            doLongPrediction();

        if (writeFlag)
        {
            fclose(outFile);

            if (!satCrashFlag)
            {
                if (batchModeFlag)
                {
                    upperCase(batchHardcopy);

                    if (!strcmp(batchHardcopy,HARDCOPY))
                        strcpy(dispStr,YES);
                    else
                        strcpy(dispStr,NO);
                }

                else
                {
                    printf("\nHardcopy (Y,N,Q)                  <Y> ? ");
                    mGets(dispStr);
                    upperCase(dispStr);

                    if (!strncmp(dispStr,QX,1))
                        killProgram();
                }

                if (!strlen(dispStr) || !strncmp(dispStr,YES,1))
                {
                    if (!batchModeFlag)
                        printf("\nprinting %s ...\n",fileName);

                    if (shortPredFlag && !launchFlag && !transitFlag)
                        sprintf(font,"%s%s",FONT,SIZE10);

                    if (shortPredFlag && launchFlag && !transitFlag)
                        sprintf(font,"%s%s",FONT,SIZE9);

                    if (!shortPredFlag && launchFlag && !transitFlag)
                        sprintf(font,"%s%s",FONT,SIZE10);

                    if (!shortPredFlag && !launchFlag && !transitFlag)
                        sprintf(font,"%s%s",FONT,SIZE10);

                    if (transitFlag)
                        sprintf(font,"%s%s",FONT,SIZE9);

                    sprintf(sysComm,"%s %s %s %s",
                        PRINTCMD,PRINTOPT,font,fileName);

                    if (verboseFlag)
                        printf("%s\n",sysComm);

                    system(sysComm);
                }
            }
        }

        if (!batchModeFlag)
            nl();
    }

    if (batchModeFlag)
        exit(-1);

    return;
}

/******************************************************************************/
/*                                                                            */
/* getNextPass: finds circumstances of the next pass                          */
/*                                                                            */
/******************************************************************************/

void getNextPass()

{
    slowStepTime = 1.0 / epochMeanMotion / 750.0;
    medStepTime  = slowStepTime * 3.0;
    fastStepTime = medStepTime * 15.0;
    
    // Reset the acceptable elevation time range
    nextStartElTime = 0;
    nextEndElTime = 0;

    tmpTime      = predTime;
    riseOrbitNum = -1;
    initNorad    = TRUE;
    acceptableElevation = atof(batchAcceptElev);
    
    maxDays = (geoSyncFlag == GEOSYNC) ? MAXDAYSSYNC : MAXDAYS;

    do
    {
        maxElevation  = -HALFPI;
        relMaxElev    = -HALFPI;
        riseAzimuth   = ZERO;
        foundRiseFlag = FALSE;
        foundSetFlag  = FALSE;
        foundStartElFlag = FALSE;
        foundEndElFlag = FALSE;
        noPassFlag    = FALSE;

        do
        {
            getSiderealTime(tmpTime);
            getSiteVector();

            getNoradStateVector(tmpTime);
            getStateVector(tmpTime);

            getAziElev(SAT);
            
            if (satElevation > ZERO && !foundRiseFlag)
            {
                getShuttleOrbit();
                getSunVector(0);
                getAziElev(SUN);

                riseOrbitNum  = (satTypeFlag == STS) ? stsOrbitNum : orbitNum;
                nextRiseTime  = tmpTime;
                riseAzimuth   = satAzimuth;
                eclipseRise   = satEclipse(CASR);
                foundRiseFlag = TRUE;
            }
            
            // Record the start time of the acceptable elevation range
            //printf("%d - %f - %f \n", foundStartElFlag, satElevation*CRD, acceptableElevation);
            if (!foundStartElFlag && satElevation*CRD > acceptableElevation){
                nextStartElTime = tmpTime;
                foundStartElFlag = TRUE;
            }
            
            // Record the end time of the acceptable elevation range
            if (foundStartElFlag && !foundEndElFlag && satElevation*CRD < acceptableElevation){
                nextEndElTime = tmpTime;
                foundEndElFlag = TRUE;
            }

            if (foundRiseFlag && satElevation > maxElevation)
            {
                getRange();
                getSubSatPoint(SAT);
                getSunVector(0);
                getAziElev(SUN);

                maxAzimuth    = satAzimuth;
                maxElevation  = satElevation;
                maxRange      = satRange;
                maxHeight     = satHeight;
                nextMaxTime   = tmpTime;
                eclipseMax    = satEclipse(satElevation);
            }

            if (satElevation < ZERO && foundRiseFlag)
            {
                getSunVector(0);
                getAziElev(SUN);

                nextSetTime   = tmpTime;
                setAzimuth    = satAzimuth;
                eclipseSet    = satEclipse(CASR);
                foundSetFlag  = TRUE;
            }

            if (satElevation < VLOWELEV || (satElevation > VLOWELEV && 
                satElevation < relMaxElev && !foundRiseFlag))
                tmpTime += fastStepTime;

            if (satElevation > VLOWELEV && satElevation > relMaxElev && 
                satElevation < LOWELEV)
                tmpTime += medStepTime;

            if (satElevation > LOWELEV && satElevation > relMaxElev && 
                !foundRiseFlag)
                tmpTime += slowStepTime;

            if ((foundRiseFlag && fabs(satElevation - maxElevation) < CASR) || 
                satElevation > -LOWELEV)
                tmpTime += slowStepTime;

            if (satElevation > ZERO && satElevation < maxElevation && 
                satElevation < -LOWELEV)
                tmpTime += slowStepTime;

            if (tmpTime > predTime + maxDays)           /* stop after maxDays */
            {
                if (foundRiseFlag && !foundSetFlag)
                {
                    getSunVector(0);
                    getAziElev(SUN);

                    nextSetTime  = tmpTime;
                    setAzimuth   = satAzimuth;
                    eclipseSet   = satEclipse(CASR);
                    foundSetFlag = TRUE;
                }

                if (!foundRiseFlag && !foundSetFlag)
                {
                    nextRiseTime  = tmpTime;
                    nextSetTime   = tmpTime;
                    nextStartElTime = tmpTime;
                    nextEndElTime = tmpTime;
                    riseAzimuth   = ZERO;
                    maxAzimuth    = ZERO;
                    setAzimuth    = ZERO;
                    maxElevation  = ZERO;
                    maxRange      = ZERO;
                    eclipseRise   = BLANK;
                    eclipseMax    = BLANK;
                    eclipseSet    = BLANK;
                    foundRiseFlag = TRUE;
                    foundSetFlag  = TRUE;
                    noPassFlag    = TRUE;
                }
            }

            relMaxElev = satElevation;
        }
        while (!foundSetFlag);

        if (writeFlag && !batchModeFlag)
        {
            printf(".");
            fflush(stdout);
        }
    }
    while (riseOrbitNum <= 0 && !noPassFlag);

    newRiseFlag = TRUE;                            /* for controlling display */
    return;
}

/******************************************************************************/
/*                                                                            */
/* calcGroundTrack: calculates ground track                                   */
/*                                                                            */
/******************************************************************************/

void calcGroundTrack()

{
    int k;

    if (!graphicsOpenFlag)
        return;

    if (curTime - orbitTime > 1.0 / epochMeanMotion || orbitTime < ONEPPM)
    {
        orbitTime = 1.0 / epochMeanMotion;
        stepTime  = orbitTime / (double) NSEGS;

        tmpTime   = (elsetEpoch > curTime) ? elsetEpoch : 
                                             curTime - orbitTime / 20.0;

        orbitTime = curTime;
        initNorad = TRUE;

        for (k = 0; k < NSEGSGT; k++)
        {
            getSiderealTime(tmpTime);
            getNoradStateVector(tmpTime);
            getStateVector(tmpTime);
            getSubSatPoint(SAT);

            groundTrack[k].ltd = satLat  * CRD;
            groundTrack[k].lng = satLong * CRD;

            tmpTime += stepTime;
        }

        newGndTrkFlag = TRUE;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* dispVersion: displays program version                                      */
/*                                                                            */
/******************************************************************************/

void dispVersion()

{
    if (!batchModeFlag && !quickStartFlag)
    {
        gotoXY(1,1);
        clearScreen();
        printf("%s %s\n",sattrName,sattrVersion);

        if (defaultsFileFlag)
        {
            printf("\nUsing 'defaults%d.dat' ...\n",defaultsFileNum);
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getEnvironment: gets the environment variables                             */
/*                                                                            */
/******************************************************************************/

void getEnvironment()

{
#ifdef HOMEDIR
    strpHome = getenv("HOME");
#else
    strpHome = SATDIR;
#endif

    strpRgst = getenv("HOME");
    strpTerm = getenv("TERM");
    strpDisp = getenv("DISPLAY");

    return;
}

/******************************************************************************/
/*                                                                            */
/* getRealTime: gets UTC date and time from the UNIX system clock             */
/*                                                                            */
/*              If SatTrack is running on one machine and the real-time       */
/*              tracking control is done by another machine, the calculations */
/*              may have to be performed ahead of time. Therefore, the time   */
/*              offset TRACKTIMEAHEAD specified in sattrack.h is added to the */
/*              system time.                                                  */
/*                                                                            */
/*              If the machine SatTrack is running on is the one that also    */
/*              performs the tracking control, the parameter TRACKTIMEAHEAD   */
/*              has to be set to 0 in 'sattrack.h'.                           */
/*                                                                            */
/******************************************************************************/

void getRealTime()

{
    getUnixTime(&sysDay,&sysMonth,&sysYear,&sysYearDay,
                &sysHour,&sysMin,&sysSec);

    if (trackingFlag > 0)
        sysSec += TRACKTIMEAHEAD;

    curYearSec = (long) 
                 ((sysYearDay - 1)*86400 + sysHour*3600 + sysMin*60 + sysSec);
    dayNumber  = getDayNum(sysDay,sysMonth,sysYear);
    utcTime    = (double) (sysHour + sysMin/60.0 + sysSec/3600.0) / 24.0;
    curTime    = (double) dayNumber + utcTime;
    curTimeGl  = curTime;
    realTime   = curTime;

    if (curYearSec == 0L)              /* needed to track past new year (UTC) */
        lastYearSec = -1L;

    return;
}

/******************************************************************************/
/*                                                                            */
/* disableTrack: disables tracking                                            */
/*                                                                            */
/******************************************************************************/

void disableTrack(disableFlag)

int disableFlag;

{
    int error;

    if (disableFlag || trackingFlag)
    {
        trackingFlag  = OFF;
        trackDispFlag = TRUE;

        if (ANTENNATYPE == ISITELESCOPE)
            error = stopISI();

        else
            error = closeSatIO();
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* doSleep: sleeps for a while and checks the keyboard when waking up         */
/*                                                                            */
/******************************************************************************/

void doSleep()

{
    milliSleep(SLEEPTIME);

    key = checkKeyboard(FALSE);

    switch(key)
    {
        case -1:                                                    /* no key */
            keyBoard = VOID;
            break;

        case 10: case 13: case 27:                             /* LF, CR, ESC */
            keyBoard = EXIT;
            disableTrack(0);
            break;

        case 48:                                                         /* 0 */
            if (liveDispFlag == SINGLESAT)
            {
                trackObject        = SAT;
                newTrackObjectFlag = TRUE;
                disableTrack(0);
            }

            break;

        case 49:                                                         /* 1 */
            if (liveDispFlag == SINGLESAT)
            {
                trackObject        = SUN;
                newTrackObjectFlag = TRUE;
                disableTrack(0);
            }

            break;

        case 50:                                                         /* 2 */
            if (liveDispFlag == SINGLESAT)
            {
                trackObject        = MOON;
                newTrackObjectFlag = TRUE;
                disableTrack(0);
            }

            break;

        case 65:                                                    /* A -> a */
            key = 97;
            break;

        case 67:                                                    /* C -> c */
            key = 99;
            break;

        case 70:                                                    /* F -> f */
            key = 102;
            break;

        case 71:                                                    /* G -> g */
            key = 103;
            break;

        case 72: case 104: case 63:                                /* H, h, ? */
            key = 104;
            break;

        case 77: case 109:                                            /* M, m */
            switchSatFlag = TRUE;
            keyBoard      = MULTISAT;
            break;

        case 78:                                                    /* N -> n */
            key = 110;
            break;

        case 79:                                                    /* O -> o */
            key = 111;
            break;

        case 81: case 113:                                            /* Q, q */
            keyBoard = QUIT;
            disableTrack(0);
            break;

        case 82: case 114:                                            /* R, r */
            if (liveDispFlag == SINGLESAT)
            {
                keyBoard = SINGLESAT;
                reinitSingleSat = TRUE;
            }

            else
            {
                keyBoard = MULTISAT;
                reinitMultiSat = TRUE;
            }

            break;

        case 83: case 115:                                            /* S, s */
            keyBoard = SINGLESAT;
            break;

        default:
            keyBoard = VOID;
            break;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkToggleTrack: checks if tracking control is changed (t,T)              */
/*                                                                            */
/* the functions of the tracking flags used are:                              */
/*                                                                            */
/* trackingFlag:  main tracking control flag                                  */
/*                TRUE when tracking is turned on, FALSE, when off            */
/*                                                                            */
/* trackCtrlFlag: used to deal with azimuth and elevation rates properly;     */
/*                TRUE after first antenna control command, so that rates     */
/*                can be calculated                                           */
/*                                                                            */
/* trackDispFlag: used to update tracking status in live display when it is   */
/*                has changed                                                 */
/*                TRUE until live display has been updated                    */
/*                                                                            */
/******************************************************************************/

void checkToggleTrack()

{
    int i, m, error;

    if (key == 116 || key == 84)                                      /* t, T */
    {
        if (key == 116)
            trackingFlag = (trackingFlag) ? OFF : ON;

        if (key == 84)
        {
            switch (trackingFlag)
            {
                case OFF:
                    trackingFlag = (numSatsAuto > 1) ? AUTOTRK : ON;
                    break;

                case ON: case AUTOTRK: case ERROR:
                    trackingFlag = OFF;
                    break;

                default:
                    break;
            }

            if (trackingFlag == AUTOTRK && numSatsAuto > 1)
            {
                i = getSatListPtr(ORDER);
                m = getSatListPtr(AUTOTRK);

                if (i > m)
                {
                    sat[satOrder[m]].statusFl = SELECT;
                    sat[satOrder[i]].statusFl = MARK;
                    orbitTime = ZERO;
                }
            }
        }

        trackDispFlag    = TRUE;
        trackCtrlFlag    = FALSE;
        lastTrackYearSec = -1L;

        if (!trackingFlag)
            lastYearSec = -1L;

        if (liveDispFlag == SINGLESAT)
            trackInt = (trackingFlag) ? (long) TRACKINT : singleSatUpdateInt;
        else
            trackInt = (trackingFlag) ? (long) TRACKINT : multiSatUpdateInt;

        if (trackingFlag > 0)
        {
            if (ANTENNATYPE == ISITELESCOPE)
            {
                if (didMultDispFlag)
                {
                    i = getSatListPtr(ORDER);
                    strcpy(satTrackName,sat[satOrder[i]].satellite);
                }

                else
                    strcpy(satTrackName,satName);

                error = initISI();
            }

            else
            {
                error = initSatIO();
            }

            if (error)
            {
                trackingFlag  = ERROR;
                trackDispFlag = TRUE;
            }
        }

        else
            disableTrack(1);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkSwitchSat: switches satellite to be tracked (u,U,d,D,a,A,n,N)         */
/*                 or finds satellite that is currently tracked (f,F)         */
/*                 or adds/deletes satellites to/from autotrack list (l,L)    */
/*                                                                            */
/******************************************************************************/

void checkSwitchSat()

{
    int i, k, m, n, error;
    int j = 0;

    pageFlag      = FALSE;
    pointerFlag   = FALSE;
    didSwitchFlag = FALSE;

    i = getSatListPtr(ORDER);            /* gets presently selected satellite */

    if (trackingFlag == AUTOTRK)
    {
        if ((sat[satOrder[i]].satEle > TRACKLIMIT2 && 
             sat[satOrder[i]].nextRiseT - curTime > TENMIN) || 
            (sat[satOrder[i]].satEle <= TRACKLIMIT2 && switchSatFlag))
        {
            m = getSatListPtr(AUTOTRK);        /* gets first marked satellite */

            if (sat[satOrder[m]].statusFl != SELECT)
            {
                sat[satOrder[i]].statusFl = MARK;
                sat[satOrder[m]].statusFl = SELECT;

                if (ANTENNATYPE == ISITELESCOPE)
                {
                    strcpy(satTrackName,sat[satOrder[m]].satellite);
                    error = initISI();
                }

                else
                {
                    error = initSatIO();
                }

                if (error)
                {
                    trackingFlag  = ERROR;
                    trackDispFlag = TRUE;
                }

                trackCtrlFlag = FALSE;
                switchSatFlag = FALSE;
                pointerFlag   = TRUE;
                didSwitchFlag = TRUE;
            }
        }
    }

    if (key == 117 && i > 0)                                             /* u */
    {
        if (sat[satOrder[i-1]].statusFl == MARK)
            numSatsAuto--;

        sat[satOrder[i]].statusFl   = FALSE;
        sat[satOrder[i-1]].statusFl = SELECT;

        disableTrack(0);
        pointerFlag   = TRUE;
        didSwitchFlag = TRUE;
    }

    if (key == 85)                                                       /* U */
    {
        sat[satOrder[i]].statusFl = FALSE;

        if (i > 9)
        {
            if (sat[satOrder[i-10]].statusFl == MARK)
                numSatsAuto--;

            sat[satOrder[i-10]].statusFl = SELECT;
        }

        else
        {
            if (sat[satOrder[0]].statusFl == MARK)
                numSatsAuto--;

            sat[satOrder[0]].statusFl = SELECT;
        }

        disableTrack(0);
        pointerFlag   = TRUE;
        didSwitchFlag = TRUE;
    }

    if (key == 100 && i < numSats - 1)                                   /* d */
    {
        if (sat[satOrder[i+1]].statusFl == MARK)
            numSatsAuto--;

        sat[satOrder[i]].statusFl   = FALSE;
        sat[satOrder[i+1]].statusFl = SELECT;

        disableTrack(0);
        pointerFlag   = TRUE;
        didSwitchFlag = TRUE;
    }

    if (key == 68)                                                       /* D */
    {
        sat[satOrder[i]].statusFl = FALSE;

        if (i < numSats - 10)
        {
            if (sat[satOrder[i+10]].statusFl == MARK)
                numSatsAuto--;

            sat[satOrder[i+10]].statusFl = SELECT;
        }

        else
        {
            if (sat[satOrder[numSats-1]].statusFl == MARK)
                numSatsAuto--;

            sat[satOrder[numSats-1]].statusFl = SELECT;
        }

        disableTrack(0);
        pointerFlag   = TRUE;
        didSwitchFlag = TRUE;
    }

    if (key == 97)                                                       /* a */
    {
        j = getSatListPtr(ELEV);

        if (sat[satOrder[j]].statusFl == MARK)
            numSatsAuto--;

        sat[satOrder[i]].statusFl = FALSE;
        sat[satOrder[j]].statusFl = SELECT;

        disableTrack(0);

        if (numSats > numSatLines)
            pageFlag = TRUE;
        else
            pointerFlag = TRUE;

        didSwitchFlag = TRUE;
    }

    if (key == 102)                                                      /* f */
    {
        j = i;

        if (numSats > numSatLines)
            pageFlag = TRUE;
        else
            pointerFlag = TRUE;
    }

    if ((key == 108 && !objectNumFlag) || key == 110)                 /* l, n */
    {
        n = (xTermFlag) ? numLinesX : numLines;
        n = (xTermFlag && numSats < n - 9) ? numSats + 9 : n;

        clearLine(1,n);
        clearLine(1,n-1);

        printf(" Enter satellite name: ");
        mGets(satNameStr);

        j = getSatListPtr(NAME);

        if (j >= 0)
        {
            if (key == 108)                                              /* l */
            {
                if (sat[satOrder[j]].statusFl == SELECT)
                {
                    if (numSatsAuto > 1)
                    {
                        sat[satOrder[j]].statusFl = FALSE;
                        numSatsAuto--;

                        m = getSatListPtr(AUTOTRK);
                        sat[satOrder[m]].statusFl = SELECT;
                    }
                }

                else
                {
                    if (sat[satOrder[j]].statusFl == MARK)
                    {
                        sat[satOrder[j]].statusFl = FALSE;
                        numSatsAuto--;
                    }

                    else
                    {
                        sat[satOrder[j]].statusFl = MARK;
                        numSatsAuto++;
                    }
                }

                pointerFlag   = TRUE;
                didSwitchFlag = TRUE;
            }

            if (key == 110)                                              /* n */
            {
                sat[satOrder[i]].statusFl = FALSE;
                sat[satOrder[j]].statusFl = SELECT;

                if (numSats > numSatLines)
                    pageFlag = TRUE;
                else
                    pointerFlag = TRUE;

                didSwitchFlag = TRUE;
            }

            disableTrack(0);
        }

        else
        {
            clearLine(1,n);

            if (!strlen(satNameStr))
                printf(" no change");
            else
                printf(" not found, no change");

            fflush(stdout);
            j = i;
        }

        objectNumFlag = FALSE;
    }

    if ((key == 108 && objectNumFlag) || key == 111)                  /* l, o */
    {
        n = (xTermFlag) ? numLinesX : numLines;
        n = (xTermFlag && numSats < n - 9) ? numSats + 9 : n;

        clearLine(1,n);
        clearLine(1,n-1);

        printf(" Enter object number: ");
        mGets(satNameStr);

        j = getSatListPtr(OBJNUM);

        if (j >= 0)
        {
            if (key == 108)                                              /* l */
            {
                if (sat[satOrder[j]].statusFl == SELECT)
                {
                    if (numSatsAuto > 1)
                    {
                        sat[satOrder[j]].statusFl = FALSE;
                        numSatsAuto--;

                        m = getSatListPtr(AUTOTRK);
                        sat[satOrder[m]].statusFl = SELECT;
                    }
                }

                else
                {
                    if (sat[satOrder[j]].statusFl == MARK)
                    {
                        sat[satOrder[j]].statusFl = FALSE;
                        numSatsAuto--;
                    }

                    else
                    {
                        sat[satOrder[j]].statusFl = MARK;
                        numSatsAuto++;
                    }
                }

                pointerFlag   = TRUE;
                didSwitchFlag = TRUE;
            }

            if (key == 111)                                              /* o */
            {
                sat[satOrder[i]].statusFl = FALSE;
                sat[satOrder[j]].statusFl = SELECT;

                if (numSats > numSatLines)
                    pageFlag = TRUE;
                else
                    pointerFlag = TRUE;

                didSwitchFlag = TRUE;
            }

            disableTrack(0);

            clearLine(1,n);
            printf(" %ld: %s",
                sat[satOrder[j]].satIdNum,sat[satOrder[j]].satellite);
            fflush(stdout);
        }

        else
        {
            clearLine(1,n);

            if (!strlen(satNameStr))
                printf(" no change");
            else
                printf(" not found, no change");

            fflush(stdout);
            j = i;
        }

        objectNumFlag = TRUE;
    }

    if (key == 76)                                                       /* L */
    {
        numSatsAuto = 0;

        for (m = 0; m < numSats; m++)
        {
            if (sat[m].statusFl == FALSE)
                sat[m].statusFl = MARK;
            else
                if (sat[m].statusFl == MARK)
                    sat[m].statusFl = FALSE;

            if (sat[m].statusFl)
                numSatsAuto++;
        }

        disableTrack(0);
        pointerFlag = TRUE;
    }

    if ((key == 97 || key == 102 || key == 110 || key == 111)   /* a, f, n, o */
         && numSats > numSatLines)
    {
        k = numSatLines / 2;

        if (j > k && j < numSats - k)
            satLinePtr = j - k;

        if (j <= k)
            satLinePtr = 0;

        if (j >= numSats - k)
            satLinePtr = numSats - numSatLines;
    }

    if (pageFlag)
        updateSatPage();

    if (pointerFlag)
        updateSatPointer();

    if (didSwitchFlag)
    {
        orbitTime  = ZERO;                      /* calculate new ground track */

        freqOffset = ZERO;
        freqStep   = FREQSTEP;
        freqPtr    = 0;
        numFreqs   = sat[getSatListPtr(ORDER)].numFr;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkSwitchPage: switches page in multisat display (j,J,k,K)               */
/*                                                                            */
/******************************************************************************/

void checkSwitchPage()

{
    pageFlag = FALSE;

    if (key == 106 && numSats > numSatLines)                             /* j */
    {
        satLinePtr += numSatLines;

        if (satLinePtr > numSats - numSatLines)
            satLinePtr = numSats - numSatLines;

        pageFlag = TRUE;
    }

    if (key == 74 && numSats > numSatLines)                              /* J */
    {
        satLinePtr = numSats - numSatLines;
        pageFlag   = TRUE;
    }

    if (key == 107 && numSats > numSatLines)                             /* k */
    {
        satLinePtr -= numSatLines;

        if (satLinePtr < 0)
            satLinePtr = 0;

        pageFlag = TRUE;
    }

    if (key == 75 && numSats > numSatLines)                              /* K */
    {
        satLinePtr = 0;
        pageFlag   = TRUE;
    }

    if (pageFlag)
        updateSatPage();

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkSwitchFrequency: switches downlink and uplink frequencies             */
/*                                                                            */
/******************************************************************************/

void checkSwitchFrequency()

{
    if (key == 99)                                                       /* c */
    {
        freqOffset = ZERO;                                            /* [Hz] */
        freqStep   = FREQSTEP;                                        /* [Hz] */

        if (freqStep > 999.0)
            sprintf(str,"reset offset, step = %.0f kHz",freqStep/1000.0);
        else
            sprintf(str,"reset offset, step = %.0f Hz",freqStep);

        dispMessage(str);
    }

    if (key == 88 || key == 120)                                      /* X, x */
    {
        if (numFreqs)
        {
            if (key == 120)
                freqPtr++;

            else
            {
                if (freqPtr == 0)
                    freqPtr = numFreqs - 1;
                else
                    freqPtr--;
            }

            freqPtr %= numFreqs;

            strcpy(downlinkMode,freqs[freqPtr].downlinkMode);
            strcpy(uplinkMode,freqs[freqPtr].uplinkMode);
            downlinkFreq     = freqs[freqPtr].downlink;
            uplinkFreq       = freqs[freqPtr].uplink;
            xponderBandwidth = freqs[freqPtr].bandwidth;
            xponderSign      = freqs[freqPtr].sign;
            freqOffset       = ZERO;
            freqStep         = FREQSTEP;
    
            sprintf(str,"select frequency #%d of %d",freqPtr+1,numFreqs);
            dispMessage(str);
        }
    }

    if (key == 60)                                                       /* < */
    {
        if (freqStep < 1001.0)
            freqStep *= 10.0;                                         /* [Hz] */

        if (freqStep > 999.0)
            sprintf(str,"step = %.0f kHz",freqStep/1000.0);
        else
            sprintf(str,"step = %.0f Hz",freqStep);

        dispMessage(str);
    }

    if (key == 62)                                                       /* > */
    {
        if (freqStep > 99.0)
            freqStep /= 10.0;                                         /* [Hz] */

        if (freqStep > 999.0)
            sprintf(str,"step = %.0f kHz",freqStep/1000.0);
        else
            sprintf(str,"step = %.0f Hz",freqStep);

        dispMessage(str);
    }

    if (key == 43 || key == 61)                                       /* +, = */
    {
        if (fabs(freqOffset + freqStep) <= xponderBandwidth / 2.0 + ONEPPM)
            freqOffset += freqStep;

        sprintf(str,"offset = %+6.2f kHz",freqOffset/1000.0);
        dispMessage(str);
    }

    if (key == 45 || key == 95)                                       /* -, _ */
    {
        if (fabs(freqOffset - freqStep) <= xponderBandwidth / 2.0 + ONEPPM)
            freqOffset -= freqStep;

        sprintf(str,"offset = %+6.2f kHz",freqOffset/1000.0);
        dispMessage(str);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkGraphics: turns graphics window on or off                             */
/*                                                                            */
/******************************************************************************/

void checkGraphics()

{
    if (key == 103 || graphicsFlag)                                   /* g, G */
    {
        graphicsFlag = FALSE;

        if (trueXtermFlag)
        {
            if (!graphicsOpenFlag)               /* start up graphics window  */
            {
                /* check if environment variable DISPLAY is defined */

                if (!strlen(strpDisp))
                {
                    sprintf(str,"Environment variable DISPLAY undefined.");
                    dispMessage(str);
                }

                else
                {
                    sprintf(graphicsTitle,"%s %s   %s",
                        sattrName,sattrVersion,graphHeader);

                    graphicsOpenFlag = StartGraphics();

                    if (graphicsOpenFlag == 99)
                    {
                        dispMessage("Graphics option not available.");
                        graphicsOpenFlag = FALSE;
                    }
                }
            }

            else
            {
                dispMessage("Graphics window is open already.");
            }
        }

        else
        {
            sprintf(str,"Cannot open graphics window on '%s'.",strpTerm);
            dispMessage(str);
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkAddGroundStation: checks if another ground station has to be added    */
/*                                                                            */
/******************************************************************************/

void checkAddGroundStation()

{
    double tmpLtd, tmpLng, tmpDist;
    int    i, n, foundIt, maidenFlag;
    int    error = 0;
    char   tmpStr[40], tmpSiteName[40], tmpSiteNameID[40], tmpMaidenHead[10];
    char   tmpDir[10], tmpCity[40];

    if (key == 101 && graphicsOpenFlag)                                  /* e */
    {
        if (numGroundStations < MAXSTATIONS)
        {
            n = (xTermFlag) ? numLinesX : numLines;
            n = (xTermFlag && numSats < n - 9) ? numSats + 9 : n;

            clearLine(1,n);
            clearLine(1,n-1);

            printf(" Enter site name or locator: ");
            mGets(str);
            upperCase(str);
            strcpy(tmpMaidenHead,str);

            if (!strlen(str))
            {
                clearLine(1,n);
                printf(" no station added");
                fflush(stdout);
                return;
            }

            maidenFlag = FALSE;
            foundIt    = FALSE;
            i = 0;

            do
            {
                strcpy(tmpStr,city[i].cty);
                upperCase(tmpStr);

                if (!strncmp(str,tmpStr,(unsigned int) strlen(str)))
                {
                    foundIt = TRUE;

                    strcpy(tmpSiteName,city[i].cty);

                    tmpLtd = city[i].lat;
                    tmpLng = city[i].lng;

                    getMaidenHead(tmpLtd,tmpLng,tmpMaidenHead);
                }

                i++;
            }
            while (i < numCities && !foundIt);

            if (foundIt)
            {
                strcpy(tmpSiteNameID,tmpSiteName);
                getSiteNameID(tmpSiteNameID);

                if (strlen(tmpSiteNameID))
                    sprintf(str,"Added: %s",tmpSiteName);
                else
                    sprintf(str,"Added: %s (%s)",tmpSiteName,tmpMaidenHead);

                clearLine(1,n);
                printf(" %s",str);
                fflush(stdout);
            }

            else
            {
                error = getPosFromMaidenHead(tmpMaidenHead,&tmpLtd,&tmpLng);

                if (error)
                {
                    sprintf(str,"no matching ground station or locator error");
                    clearLine(1,n);
                    printf(" %s",str);
                    fflush(stdout);
                }

                else
                {
                    strcpy(tmpSiteName,str);
                    maidenFlag = TRUE;
                }
            }

            if (foundIt || (!foundIt && !error))
            {
                if ((foundIt && !strlen(tmpSiteNameID)) || !foundIt)
                {
                    strcpy(tmpSiteNameID,tmpMaidenHead);

                    getGroundTrack(tmpLtd,tmpLng,
                        &dummyd,&dummyd,&tmpDist,tmpDir,tmpCity);

                    if (maidenFlag)
                    {
                        sprintf(str,"%s is %.1f km %s of %s",
                            tmpSiteNameID,tmpDist,tmpDir,tmpCity);
                        clearLine(1,n);
                        printf(" %s",str);
                        fflush(stdout);
                    }
                }

                groundStation[numGroundStations].ltd = tmpLtd;
                groundStation[numGroundStations].lng = tmpLng;
                strcpy(groundStation[numGroundStations].gndStnID,
                       tmpSiteNameID);
                numGroundStations++;
                lastSatGraphHeight = ZERO;
            }
        }

        else
            dispMessage("Too many ground stations.");
    }

    if (key == 69 && numGroundStations > 1)                              /* E */
    {
        dispMessage("Deleted secondary ground stations.");
        numGroundStations = 1;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkHelp: displays help string                                            */
/*                                                                            */
/******************************************************************************/

void checkHelp()

{
    if (key == 104)                                                /* h, H, ? */
    {
        dispMessage(helpStr);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkExit: checks if program should be exited                              */
/*                                                                            */
/******************************************************************************/

void checkExit()

{
    if (mainChoice == SINGLESAT || mainChoice == MULTISAT)
        mainChoice = RESTART;

    switch (keyBoard) 
    {
        case QUIT:                               /* quit sattrack on Q or q   */
            killProgram();

        case SINGLESAT:                          /* single sat disp on S or s */
            mainChoice = SINGLESAT;
            break;

        case MULTISAT:                           /* multisat disp on M or m   */
            mainChoice = (firstMultiSat) ? RESTART : MULTISAT;
            break;

        default:
            break;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkKeyboard: checks keyboard entry when live display is running;         */
/*                it returns zero if no character was entered, and the        */
/*                ASCII code otherwise                                        */
/*                                                                            */
/******************************************************************************/

int checkKeyboard(entryFlag)

int entryFlag;

{
    static int structValidFlag = FALSE;

#ifdef HPUX
    static struct termio  original, modified;
#else
    static struct termios original, modified;
#endif

    int retValue;

    if (!structValidFlag)
    {

#ifdef HPUX
        ioctl(STDIN_FILENO,TCGETA,&original);
        memcpy((char *)&modified,(char *)&original,sizeof(struct termio));
#endif

#ifdef FREEBSD
        ioctl(STDIN_FILENO,TIOCGETA,&original);
        memcpy((char *)&modified,(char *)&original,sizeof(struct termios));
#endif

#if (!defined(HPUX) && !defined(FREEBSD))
        ioctl(STDIN_FILENO,TCGETS,&original);
        memcpy((char *)&modified,(char *)&original,sizeof(struct termios));
#endif

        modified.c_lflag  = 0x00000000;       /* clear all flags              */

     /* modified.c_lflag |= 0x00000002; */    /* set ICANON                   */
     /* modified.c_lflag |= 0x00000008; */    /* set ECHO                     */
     /* modified.c_lflag |= 0x00000010; */    /* set ECHOE                    */
     /* modified.c_lflag |= 0x00000200; */    /* set ECHOCTL                  */
     /* modified.c_lflag |= 0x00008000; */    /* set IEXTEN                   */

        modified.c_cc[VMIN]   = 0;            /* minimum number of characters */
        modified.c_cc[VTIME]  = 0;            /* do not wait                  */
        modified.c_cc[VERASE] = 0177;         /* enable DEL                   */

        structValidFlag = TRUE;
    }

#ifdef HPUX
    ioctl(STDIN_FILENO,TCSETA,&modified);
#endif

#ifdef FREEBSD
    ioctl(STDIN_FILENO,TIOCSETA,&modified);
#endif

#if (!defined(HPUX) && !defined(FREEBSD))
    ioctl(STDIN_FILENO,TCSETS,&modified);
#endif

    retValue = fgetc(stdin);

    if (feof(stdin))
    {
       retValue = -1;
       clearerr(stdin);
    }

#ifdef HPUX
    ioctl(STDIN_FILENO,TCSETA,&original);
#endif

#ifdef FREEBSD
    ioctl(STDIN_FILENO,TIOCSETA,&original);
#endif

#if (!defined(HPUX) && !defined(FREEBSD))
    ioctl(STDIN_FILENO,TCSETS,&original);
#endif

    if (retValue != -1 && !entryFlag)
    {
        if (debugFlag)
        {
            sprintf(str,"key = %d",retValue);
            dispMessage(str);
        }

        gotoXY(lastX,lastY);
        fflush(stdout);
    }

    return(retValue);
}

/******************************************************************************/
/*                                                                            */
/* initMain: initialize main program                                          */
/*                                                                            */
/******************************************************************************/

void initMain()

{
    timeZones = sizeof (timeZoneHour) / sizeof (double);

    lastJulianDateSun  = ZERO;
    lastJulianDateNute = ZERO;
    lastJulianDatePrec = ZERO;
    orbitTime          = ZERO;

    dispVersion();
    getRealTime();
    getSiderealTime(curTime);
    readCities();
    calcCityVectors();
    getTleSets();

    mainChoice         = START;
    startFlag          = TRUE;
    objectNumFlag      = FALSE;
    trackingEnableFlag = FALSE;
    didMultDispFlag    = FALSE;
    firstGraphics      = TRUE;
    graphicsOpenFlag   = FALSE;
    newGndTrkFlag      = FALSE;

    antennaFile        = NULL;
    radioFileA         = NULL;
    radioFileB         = NULL;

    freqOffset         = ZERO;
    freqStep           = FREQSTEP;
    freqPtr            = 0;

#ifdef VISIBPASSES
    visibPassesFlag    = TRUE;
#else
    visibPassesFlag    = FALSE;
#endif

    return;
}

/******************************************************************************/
/*                                                                            */
/* initRestart: inititialize main program at restart                          */
/*                                                                            */
/******************************************************************************/

void initRestart()

{
    if (mainChoice == RESTART)
        dispVersion();

    if (satCrashFlag && liveDispFlag != MULTISAT)
    {
        doBeep(); nl(); reverseBlink();

        if (satCrashFlag)
            printf(" Satellite '%s' crashed already \n",satName);

        normal();
    }

    mainChoice         = RESTART;
    dispUpdateFlag     = TRUE;
    firstMultiSat      = TRUE;
    firstMultiSatCalc  = TRUE;
    satCrashFlag       = FALSE;
    preLaunchFlag      = FALSE;
    preOrbitFlag       = FALSE;
    predictionFlag     = FALSE;
    writeFlag          = FALSE;
    finishPass         = FALSE;
    newRiseFlag        = FALSE;
    passDispFlag       = TRUE;
    countdownFlag      = FALSE;
    checkCountdown     = TRUE;
    sunTransitFlag     = FALSE;
    trackObject        = SAT;
    switchSatFlag      = FALSE;
    key                = TRUE;
    orbitTime          = ZERO;
    lastSatGraphHeight = ZERO;
    satLinePtr         = 0;
    numSatsAuto        = 1;

    return;
}

/******************************************************************************/
/*                                                                            */
/* initTerminal: initializes terminal flag and sets display update intervals  */
/*                                                                            */
/******************************************************************************/

void initTerminal()

{
    if (batchModeFlag)
        return;

    xTermFlag     = (!strcmp(strpTerm,termTypeX)) ? TRUE : FALSE;
    trueXtermFlag = (!strcmp(strpTerm,DEFXTERM))  ? TRUE : FALSE;
    gndTrkFlag    = (xTermFlag) ? TRUE : FALSE;

    singleSatUpdateInt = (xTermFlag) ? (long) FASTUPDATEINT : 
                                       (long) MEDUPDATEINT;

    multiSatUpdateInt  = (xTermFlag) ? (long) FASTUPDATEINT : 
                                       (long) SLOWUPDATEINT;

    if (xTermFlag && verboseFlag && startFlag)
    {
        printf("%dx%d window required\n",XDISPCOLUMNS,numLinesX);
        startFlag = FALSE;
    }

    setbuf(stdout,NULL);                            /* turn off output buffer */

    return;
}

/******************************************************************************/
/*                                                                            */
/* showMainMenu: presents main menu options                                   */
/*                                                                            */
/******************************************************************************/

void showMainMenu()

{
    if (batchModeFlag)
    {
        predictionFlag = TRUE;
        writeFlag      = TRUE;
        mainChoice     = START;
    }

    else
    {
        predictionFlag = FALSE;
        writeFlag      = FALSE;
        mainChoice     = START;
    }

    while (mainChoice == START)
    {
        if (batchModeFlag)
            strcpy(dispStr,PX);

        else
        {
            if (quickStartFlag)
                *dispStr = '\0';

            else
            {
                reverse(); printf("D"); normal();
                printf("isplay ");
                reverse(); printf("P"); normal();
                printf("rediction ");
                reverse(); printf("R"); normal();
                printf("estart ");
                reverse(); printf("Q"); normal();
                printf("uit   <D> ? ");

                mGets(dispStr);
                upperCase(dispStr);
            }
        }

        if (!strlen(dispStr))       mainChoice = DISPLAY;          /* default */
        if (!strncmp(dispStr,DX,1)) mainChoice = DISPLAY;
        if (!strncmp(dispStr,PX,1)) mainChoice = PREDICT;
        if (!strncmp(dispStr,TX,1)) mainChoice = TEST;
        if (!strncmp(dispStr,RX,1)) mainChoice = RESTART;
        if (!strncmp(dispStr,QX,1)) mainChoice = QUIT;

        if (mainChoice == DISPLAY)
        {
            if (quickStartFlag)
                sprintf(dispStr,"%s",defDispType);

            else
            {
                reverse(); printf("S"); normal();
                printf("ingle or ");
                reverse(); printf("M"); normal();
                printf("ultiple satellite(s)   <%s> ? ",defDispType);
                mGets(dispStr);
                upperCase(dispStr);
            }

            if (!strncmp(dispStr,RX,1))
            {
                mainChoice = RESTART;
                break;
            }

            if (!strncmp(dispStr,QX,1))
                killProgram();

            if (!strlen(dispStr))
                sprintf(dispStr,"%s",defDispType);

            mainChoice = (!strncmp(dispStr,MX,1)) ? MULTISAT : SINGLESAT;
        }

        if (mainChoice == PREDICT)
        {
            transitFlag = FALSE;

            if (batchModeFlag)
            {
                if (!strcmp(batchPredType,LONGPR) || 
                    !strcmp(batchPredType,TRANSIT))
                    strcpy(dispStr,LX);
                else
                    strcpy(dispStr,SX);

                transitFlag = (!strcmp(batchPredType,TRANSIT)) ? TRUE : FALSE;
            }

            else
            {
                reverse(); printf("S"); normal();
                printf("hort or ");
                reverse(); printf("L"); normal();
                printf("ong prediction format   <S> ? ");
                mGets(dispStr);
                upperCase(dispStr);

                if (!strncmp(dispStr,RX,1))
                {
                    mainChoice = RESTART;
                    break;
                }

                if (!strncmp(dispStr,QX,1))
                    killProgram();
            }

            shortPredFlag = (!strlen(dispStr) || 
                             !strncmp(dispStr,SX,1)) ? TRUE : FALSE;

            if (!shortPredFlag)
            {
                if (batchModeFlag)
                    sunTransitFlag = FALSE;

                else
                {

#ifdef SUNTRANSITS
                    printf("Calculate solar transits          <N> ? ");
                    mGets(dispStr);
#else
                    *dispStr = '\0';
#endif

                    upperCase(dispStr);

                    if (!strncmp(dispStr,RX,1))
                    {
                        mainChoice = RESTART;
                        break;
                    }

                    if (!strncmp(dispStr,QX,1))
                        killProgram();

                    sunTransitFlag = (!strlen(dispStr) || 
                                      !strncmp(dispStr,NO,1)) ? FALSE : TRUE;
                }

                if (sunTransitFlag)
                {
                    printf("Enter approach limit [arcmin]  <%4.0f> : ",
                        SUNPROX);
                    mGets(dispStr);

                    sunProxLimit = (!strlen(dispStr)) ? SUNPROX : atof(dispStr);
                    sunProxLimit *= CAMR;
                }

                if (transitFlag)
                {
                    sunProxLimit   = SUNPROX * CAMR;
                    sunTransitFlag = TRUE;
                }
            }

            if (batchModeFlag)
            {
                predictionFlag = TRUE;
                writeFlag      = TRUE;
            }

            else
            {
                printf("Output on ");
                reverse(); printf("V"); normal();
                printf("ideo terminal or ");
                reverse(); printf("F"); normal();
                printf("ile  <V> ? ");
                mGets(dispStr);
                upperCase(dispStr);

                if (!strncmp(dispStr,RX,1))
                {
                    mainChoice = RESTART;
                    break;
                }

                if (!strncmp(dispStr,QX,1))
                    killProgram();

                predictionFlag = (!strlen(dispStr) || 
                                  !strncmp(dispStr,VX,1)) ? TRUE : FALSE;
                writeFlag      = (!strncmp(dispStr,FX,1)) ? TRUE : FALSE;
            }
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSatListPtr: finds pointer to the currently active satellite             */
/*                                                                            */
/******************************************************************************/

int getSatListPtr(ptrType)

int ptrType;

{
    long ll;
    int  i, j, k;

    i = 0;

    if (ptrType == LIST)
    {
        foundSatFlag = TRUE;

        while (sat[i].statusFl != SELECT && i < numSats)
            i++;
    }

    if (ptrType == ORDER)
    {
        foundSatFlag = TRUE;

        while (sat[satOrder[i]].statusFl != SELECT && i < numSats)
            i++;
    }

    if (ptrType == ELEV)
    {
        foundSatFlag = TRUE;

        while (sat[satOrder[i]].satEle > ZERO && i < numSats)
            i++;
    }

    if (ptrType == NAME)
    {
        foundSatFlag = FALSE;
        upperCase(satNameStr);
        k = strlen(satNameStr);
        j = 0;

        while (k > 0 && j < numSats)
        {
            strcpy(str,sat[satOrder[j]].satellite);
            upperCase(str);

            if (!strncmp(str,satNameStr, (unsigned int) k))
            {
                foundSatFlag = TRUE;
                i = j;
                break;
            }

            j++;
        }
    }

    if (ptrType == OBJNUM)
    {
        foundSatFlag = FALSE;
        ll = atol(satNameStr);
        k  = strlen(satNameStr);
        j  = 0;

        while (k > 0 && j < numSats)
        {
            if (ll == sat[satOrder[j]].satIdNum)
            {
                foundSatFlag = TRUE;
                i = j;
                break;
            }

            j++;
        }
    }

    if (ptrType == AUTOTRK)
    {
        foundSatFlag = FALSE;
        j = 0;

        while (j < numSats)
        {
            if (sat[satOrder[j]].statusFl == MARK || 
                sat[satOrder[j]].statusFl == SELECT)
            {
                foundSatFlag = TRUE;
                i = j;
                break;
            }

            j++;
        }
    }

    i = (foundSatFlag) ? i : -1;
    return(i);
}

/******************************************************************************/
/*                                                                            */
/* initSingleSatTrackLoop: initializes tracking loop parameters for a single  */
/*                         satellite                                          */
/*                                                                            */
/******************************************************************************/

void initSingleSatTrackLoop()

{
    int ns;

    liveDispFlag  = SINGLESAT;
    trackObject   = SAT;
    prevDispEle   = -TWOPI;
    prevSatLat    = -TWOPI;
    lastYearSec   = -1L;
    oldCityNum    = -1;

    trackInt = (trackingFlag) ? (long) TRACKINT : singleSatUpdateInt;

    if (firstMultiSat)
    {
        nextRiseTime = ZERO;
        nextSetTime  = ZERO;
        riseAzimuth  = ZERO;
        maxAzimuth   = ZERO;
        setAzimuth   = ZERO;
        maxRange     = ZERO;
        newRiseFlag  = FALSE;
        passDispFlag = FALSE;
    }

    else
    {
        ns = getSatListPtr(LIST);
        loadSatData(ns);
        newRiseFlag  = TRUE;
        passDispFlag = TRUE;
    }

    checkPropModelType();

    sprintf(helpStr,"Control: %s",singleSatHelp);

    return;
}

/******************************************************************************/
/*                                                                            */
/* doSingleSatTrackLoop: performs tracking loop with detailed live display    */
/*                       for a single satellite                               */
/*                                                                            */
/******************************************************************************/

void doSingleSatTrackLoop()

{
    if (reinitSingleSat)
    {
        initSingleSatTrackLoop();
        initSingleSatLiveDisp();
        reinitSingleSat = FALSE;
    }

    do
    {
        getRealTime();

        if (curYearSec >= lastYearSec + trackInt && curYearSec % trackInt == 0L)
        {
            calcGroundTrack();
            doLiveCalcs();
            trackingControl();

            lastYearSec    = curYearSec;
            dispUpdateFlag = TRUE;
        }

        if (curYearSec % singleSatUpdateInt == 0L && dispUpdateFlag)
        {
            if (graphicsOpenFlag)
                CreateGraphicsTimeOut();

            updateSingleSatLiveDisp(curTime,nextRiseTime,nextSetTime,
                riseAzimuth,maxAzimuth,setAzimuth,maxElevation,maxRange);

            dispUpdateFlag = FALSE;
        }

        if (graphicsOpenFlag)
            UpdateGraphics(FALSE);

        doSleep();
        checkToggleTrack();
        checkSwitchFrequency();
        checkGraphics();
        checkHelp();
    }
    while (keyBoard == VOID && !satCrashFlag);

    return;
}

/******************************************************************************/
/*                                                                            */
/* initMultiSatTrackLoop: initializes tracking loop parameters for multiple   */
/*                        satellites                                          */
/*                                                                            */
/******************************************************************************/

void initMultiSatTrackLoop()

{
    int i;

    liveDispFlag = MULTISAT;
    trackObject  = SAT;
    lastYearSec  = -1L;

    trackInt = (trackingFlag) ? (long) TRACKINT : multiSatUpdateInt;

    if (firstMultiSat)
    {
        newPassFlag = FALSE;

        for (i = 0; i < numSats; i++)
        {
            sat[i].satLastEle = -PI;
            sat[i].nextRiseT  = ZERO;
            sat[i].nextSetT   = ZERO;
            sat[i].crashFl    = FALSE;
            sat[i].nopassFl   = FALSE;
        }
    }

    sprintf(helpStr,"Control: %s",multiSatHelp);

    return;
}

/******************************************************************************/
/*                                                                            */
/* doMultiSatTrackLoop: performs tracking loop with live display for multiple */
/*                      satellites                                            */
/*                                                                            */
/******************************************************************************/

void doMultiSatTrackLoop()

{
    if (firstMultiSat)
    {
        readSatList();
        saveSatElements();
        initSortSats();
    }

    if (reinitMultiSat)
    {
        initMultiSatTrackLoop();
        initMultiSatLiveDisp();
        reinitMultiSat = FALSE;
    }

    do
    {
        getRealTime();

        if (firstMultiSat || 
           (curYearSec >= lastYearSec + trackInt &&
            curYearSec % trackInt == 0L))
        {
            for (ns = 0; ns < numSats; ns++)
            {
                if (firstMultiSat || 
                    (curYearSec % multiSatUpdateInt == 0L) ||
                    (curYearSec % multiSatUpdateInt != 0L && 
                     sat[ns].statusFl == SELECT && trackingFlag > 0))
                {

                    if (firstMultiSat)
                    {
                        if (firstMultiSatCalc)
                        {
                            gotoXY(18,7);
                            printf("calculating next pass for satellite #");
                            firstMultiSatCalc = FALSE;
                        }

                        gotoXY(55,7);

                        if (xTermFlag)
                            printf("%-3d: %-14s",ns+1,sat[ns].satellite);
                        else
                            printf("%-d",ns+1);

                        if (ns+1 == numSats)
                            printf("  ---  standing by ");

                        fflush(stdout);
                    }

                    loadSatData(ns);
    
                    if (sat[ns].statusFl == SELECT)
                        calcGroundTrack();

                    doLiveCalcs();

                    if (sat[ns].statusFl == SELECT)
                        trackingControl();

                    saveSatData(ns);
                }
            }

            firstMultiSat  = FALSE;
            dispUpdateFlag = TRUE;
            lastYearSec    = curYearSec;
        }

        if (curYearSec % multiSatUpdateInt == 0L && dispUpdateFlag)
        {
            if (graphicsOpenFlag)
                CreateGraphicsTimeOut();

            updateMultiSatLiveDisp(curTime);
            dispUpdateFlag = FALSE;
        }

        if (graphicsOpenFlag)
            UpdateGraphics(FALSE);

        doSleep();
        checkToggleTrack();
        checkSwitchFrequency();
        checkSwitchSat();
        checkSwitchPage();
        checkGraphics();
        checkAddGroundStation();
        checkHelp();
    }
    while (keyBoard == VOID);

    return;
}

/******************************************************************************/
/*                                                                            */
/* doLiveCalcs: performs the calculations for the live display loops          */
/*                                                                            */
/******************************************************************************/

void doLiveCalcs()

{
    if (curTime > nextSetTime && geoSyncFlag != GEOSTAT)
    {
        predTime     = (elsetEpoch > curTime) ? elsetEpoch : curTime;
        predStopTime = predTime + (stopTime - startTime);

        getNextPass();
        newPassFlag = TRUE;
    }

    preLaunchFlag = (launchEpoch > curTime) ? TRUE : FALSE;
    preOrbitFlag  = (elsetEpoch  > curTime) ? TRUE : FALSE;
    initNorad     = TRUE;

    getSiderealTime(curTime);
    getSiteVector();

    getNoradStateVector(curTime);
    getStateVector(curTime);

    getAziElev(SAT);
    getRange();
    getShuttleOrbit();
    getSunVector(1);
    getAziElev(SUN);
    getAziElev(MOON);
    getSubSatPoint(SAT);
    getSubSatPoint(SUN);
    getDoppler();
    getPathLoss();
    getSquintAngle();
    getSunPhaseAngle(); 
    getPhase();
    getMode();

    eclipseCode = satEclipse(satElevation);
    return;
}

/******************************************************************************/
/*                                                                            */
/* saveSatData: saves satellite data in a structure                           */
/*                                                                            */
/******************************************************************************/

void saveSatData(satPtr)

int satPtr;

{
    if (preOrbitFlag)
    {
        if (satTypeFlag == STS && preLaunchFlag)
        {
            satLat       = KSCLAT;
            satLong      = KSCLONG;
            satHeight    = KSCALT;
            satAzimuth   = ZERO;
            satElevation = ZERO;
            stsOrbitNum  = 0L;
        }

        else
        {
            satLat       = ZERO;
            satLong      = ZERO;
            satHeight    = ZERO;
            satAzimuth   = ZERO;
            satElevation = ZERO;
            orbitNum     = 0L;
        }
    }

    if (gndTrkFlag || graphicsOpenFlag)
    {
        getGroundTrack(satLat,satLong,
                       &cityLat,&cityLong,&gndTrkDist,gndTrkDir,gndTrkCity);
    }

    else
    {
        gndTrkDist  = ZERO;
        cityLat     = ZERO;
        cityLong    = ZERO;
        *gndTrkDir  = '\0';
        *gndTrkCity = '\0';
    }

    sat[satPtr].dwnlnkFrq   = downlinkFreq;
    sat[satPtr].uplnkFrq    = uplinkFreq;

    sat[satPtr].satOrbNum   = orbitNum;
    sat[satPtr].stsOrbNum   = stsOrbitNum;

    sat[satPtr].satOrb      = curOrbit;
    sat[satPtr].stsOrb      = stsOrbit;
    sat[satPtr].satOrbFrac  = orbitFract;
    sat[satPtr].stsOrbFrac  = stsOrbitFract;

    sat[satPtr].satAzi      = satAzimuth;
    sat[satPtr].satLastEle  = sat[satPtr].satEle;
    sat[satPtr].satEle      = satElevation;
    sat[satPtr].satRng      = satRange;
    sat[satPtr].satLtd      = satLat;
    sat[satPtr].satLng      = satLong;
    sat[satPtr].satHgt      = satHeight;

    sat[satPtr].satTypeFl   = satTypeFlag;
    sat[satPtr].launchFl    = launchFlag;
    sat[satPtr].prelaunchFl = preLaunchFlag;
    sat[satPtr].preorbitFl  = preOrbitFlag;
    sat[satPtr].geosyncFl   = geoSyncFlag;
    sat[satPtr].crashFl     = satCrashFlag;

    sat[satPtr].eclCode     = eclipseCode;

    sat[satPtr].ctyLtd      = cityLat;
    sat[satPtr].ctyLng      = cityLong;
    sat[satPtr].ctyDist     = gndTrkDist;

    strcpy(sat[satPtr].ctyDir,gndTrkDir);
    strcpy(sat[satPtr].ctyStr,gndTrkCity);

    sat[satPtr].semiMajAxis = semiMajorAxis;
    sat[satPtr].raanPr      = raanPrec;
    sat[satPtr].periPr      = perigeePrec;

    if (newPassFlag)
    {
        sat[satPtr].nextRiseT = nextRiseTime;
        sat[satPtr].nextSetT  = nextSetTime;
        sat[satPtr].riseAzi   = riseAzimuth;
        sat[satPtr].maxAzi    = maxAzimuth;
        sat[satPtr].setAzi    = setAzimuth;
        sat[satPtr].maxEle    = maxElevation;
        sat[satPtr].maxRng    = maxRange;
        sat[satPtr].eclRise   = eclipseRise;
        sat[satPtr].eclMax    = eclipseMax;
        sat[satPtr].eclSet    = eclipseSet;
        sat[satPtr].nopassFl  = noPassFlag;
    }

    if (curTime > sat[satPtr].nextRiseT && 
        fabs(sat[satPtr].nextSetT - curTime) < HALFMIN &&
        sat[satPtr].satEle < ZERO)
        sat[satPtr].satEle = ONEPPM;

    if (sat[satPtr].nopassFl && newPassFlag)
    {
        sat[satPtr].nextRiseT = curTime + maxDays;
        sat[satPtr].nextSetT  = curTime + maxDays;
    }

    newPassFlag = FALSE;

    if (sat[satPtr].geosyncFl == GEOSTAT && sat[satPtr].satEle <= ZERO)
        sat[satPtr].nextRiseT = ONEMEG + ONEPPM;

    if (sat[satPtr].crashFl)
        sat[satPtr].nextRiseT = ONEMEG + ONEPPM + TWOPPM;

    if (sat[satPtr].geosyncFl == GEOSTAT && sat[satPtr].satEle > ZERO)
        sat[satPtr].nextSetT  = TWOMEG;

    return;
}

/******************************************************************************/
/*                                                                            */
/* loadSatData: loads satellite data from a structure into global variables   */
/*                                                                            */
/******************************************************************************/

void loadSatData(satPtr)

int satPtr;

{
    int i;

    strcpy(satName,sat[satPtr].satellite);
    strcpy(satAlias,sat[satPtr].satelliteAlias);
    strcpy(propModel,sat[satPtr].model);

    satNum           = sat[satPtr].satIdNum;

    epochDay         = sat[satPtr].epDay;
    elsetEpoch       = sat[satPtr].elSetEp;
    launchEpoch      = sat[satPtr].launchEp;

    inclination      = sat[satPtr].epInc;
    epochRaan        = sat[satPtr].epRaan;
    eccentricity     = sat[satPtr].epEcc;
    epochArgPerigee  = sat[satPtr].epArgPer;
    epochMeanAnomaly = sat[satPtr].epMeanAnom;
    epochMeanMotion  = sat[satPtr].epMeanMot;
    decayRate        = sat[satPtr].epDecRate;
    decayRateDot     = sat[satPtr].epDecRateDot;
    bStarCoeff       = sat[satPtr].epBstar;
    epochOrbitNum    = sat[satPtr].epOrbNum;
    ephemerisType    = sat[satPtr].ephType;
    propModelType    = sat[satPtr].modelType;

    attitudeFlag     = sat[satPtr].attitudeFl;
    perigeePhase     = sat[satPtr].perigPh;
    maxPhase         = sat[satPtr].maxPh;
    numModes         = sat[satPtr].numMd;

    refOrbit         = sat[satPtr].refOrb;
    semiMajorAxis    = sat[satPtr].semiMajAxis;
    raanPrec         = sat[satPtr].raanPr;
    perigeePrec      = sat[satPtr].periPr;

    cosInclination   = sat[satPtr].cosInc;
    sinInclination   = sat[satPtr].sinInc;

    satLat           = sat[satPtr].satLtd;
    satLong          = sat[satPtr].satLng;
    satHeight        = sat[satPtr].satHgt;

    satAzimuth       = sat[satPtr].satAzi;
    satElevation     = sat[satPtr].satEle;
    satRange         = sat[satPtr].satRng;

    nextRiseTime     = sat[satPtr].nextRiseT;
    nextSetTime      = sat[satPtr].nextSetT;
    riseAzimuth      = sat[satPtr].riseAzi;
    maxAzimuth       = sat[satPtr].maxAzi;
    setAzimuth       = sat[satPtr].setAzi;
    maxElevation     = sat[satPtr].maxEle;
    maxRange         = sat[satPtr].maxRng;
    eclipseRise      = sat[satPtr].eclRise;
    eclipseMax       = sat[satPtr].eclMax;
    eclipseSet       = sat[satPtr].eclSet;

    satTypeFlag      = sat[satPtr].satTypeFl;
    launchFlag       = sat[satPtr].launchFl;
    preLaunchFlag    = sat[satPtr].prelaunchFl;
    preOrbitFlag     = sat[satPtr].preorbitFl;
    geoSyncFlag      = sat[satPtr].geosyncFl;
    satCrashFlag     = sat[satPtr].crashFl;

    eclipseCode      = sat[satPtr].eclCode;

    if (attitudeFlag)
    {
        attLat       = sat[satPtr].attLtd;
        attLong      = sat[satPtr].attLng;
        satAttGl[0]  = sat[satPtr].attX;
        satAttGl[1]  = sat[satPtr].attY;
        satAttGl[2]  = sat[satPtr].attZ;
    }

    if (sat[satPtr].statusFl == SELECT)
    {
        if (numModes)
        {
            for (i = 0; i < numModes; i++)
            {
                strcpy(modes[i].modeStr,sat[satPtr].satModes[i].modeStr);
                modes[i].minPhase = sat[satPtr].satModes[i].minPhase; 
                modes[i].maxPhase = sat[satPtr].satModes[i].maxPhase;;
            }
        }

        numFreqs = sat[satPtr].numFr;

        if (numFreqs)
        {
            for (i = 0; i < numFreqs; i++)
            {
                strcpy(freqs[i].downlinkMode,
                       sat[satPtr].satFreqs[i].downlinkMode);
                strcpy(freqs[i].uplinkMode,
                       sat[satPtr].satFreqs[i].uplinkMode);
                freqs[i].downlink  = sat[satPtr].satFreqs[i].downlink; 
                freqs[i].uplink    = sat[satPtr].satFreqs[i].uplink;
                freqs[i].bandwidth = sat[satPtr].satFreqs[i].bandwidth;
                freqs[i].sign      = sat[satPtr].satFreqs[i].sign;
            }

            strcpy(downlinkMode,freqs[freqPtr].downlinkMode);
            strcpy(uplinkMode,freqs[freqPtr].uplinkMode);
            downlinkFreq     = freqs[freqPtr].downlink;
            uplinkFreq       = freqs[freqPtr].uplink;
            xponderBandwidth = freqs[freqPtr].bandwidth;
            xponderSign      = freqs[freqPtr].sign;
        }

        else
        {
            *downlinkMode    = '\0';
            *uplinkMode      = '\0';
            downlinkFreq     = DOWNLINKFREQ * CMHZHZ;
            uplinkFreq       = UPLINKFREQ * CMHZHZ;
            xponderBandwidth = ZERO;
            xponderSign      = ZERO;
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* main program sattrack                                                      */
/*                                                                            */
/******************************************************************************/

void main(argc,argv)

int  argc;
char *argv[];

{
    signal(SIGINT,killProgram);
    getEnvironment();
    checkArguments(argc,argv);
    //checkLicense();	// jwc, no need to check.
    initMain();

    while (TRUE)                                         /* main program loop */
    {
        if (mainChoice == START || mainChoice == PREDICT || 
            mainChoice == TEST  || mainChoice == RESTART)
        {
            initRestart();
            getRealTime();
            getDefaultData();
            initTerminal();
        }

        while (mainChoice == RESTART)
        {
            checkTimeZone();
            getSiteParams();
            getSatParams();
            getSatModes(satName);
            getSatPrecession();
            showMainMenu();
        }

        switch (mainChoice)
        {
            case SINGLESAT:
                reinitSingleSat = TRUE;
                doSingleSatTrackLoop();
                break;

            case MULTISAT:
                reinitMultiSat = TRUE;
                doMultiSatTrackLoop();
                break;

            case PREDICT:
                doPrediction();
                break;

            case TEST:
                doNoradTest();
                break;

            case RESTART:
                break;

            case QUIT:
                killProgram();

            default:
                break;
        }

        checkExit();
    }
}

/******************************************************************************/
/*                                                                            */
/* End of program sattrack.c                                                  */
/*                                                                            */
/******************************************************************************/
