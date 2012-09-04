/******************************************************************************/
/*                                                                            */
/*  Title       : satdisp.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 24Feb92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This function block performs the live display part of the   */
/*                satellite tracking program SatTrack.                        */
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

#include "satglobalsx.h"          /* external global variables                */
#include "sattrack.h"             /* definition of various constants          */

#ifdef HPTERM
#include "hpterm.h"               /* definitions of hpterm macros             */
#else
#include "vt100.h"                /* definitions of VT100 macros              */
#endif

/******************************************************************************/
/*                                                                            */
/*  static variables                                                          */
/*                                                                            */
/******************************************************************************/

static int launchMsgA, launchMsgB, launchMsgC, launchMsgD;
static int lastYearDay, reverseFlag;
static int firstMultiSatDisp, poleFlag;

static char localDate[12], localTime[12];

/******************************************************************************/
/*                                                                            */
/* updateSingleSatLiveDisp: updates the tracking live display                 */
/*                                                                            */
/******************************************************************************/

void updateSingleSatLiveDisp(curTime,nextRiseTime,nextSetTime,
            riseAzimuth,maxAzimuth,setAzimuth,maxElevation,maxRange)

double curTime, nextRiseTime, nextSetTime;
double riseAzimuth, maxAzimuth, setAzimuth, maxElevation, maxRange;

{
    double fabsMET, timeArg, cUnit;
    double dispAzimuth   = ZERO;
    double dispElevation = ZERO;
    long   tDayNum;
    int    tDay, tMonth, tYear, tYearDay, tHour, tMin, tSec;
    int    i, lastHour, lastMin, lastSec;
    char   str[80];

    if (preOrbitFlag)                       /* do not display negative orbits */
    {
        for (i = 0; i <= 2; i++)
        {
            satPosGl[i] = ZERO;
            satVelGl[i] = ZERO;
        }

        orbitNum      = FALSE;
        orbitFract    = ZERO;
        stsOrbitNum   = FALSE;
        stsOrbitFract = ZERO;
        downlinkDopp  = ZERO;
        uplinkDopp    = ZERO;
        downlinkLoss  = ZERO;
        uplinkLoss    = ZERO;
        squintAngle   = ZERO;
        sunPhaseAngle = ZERO;
        satPhase      = ZERO;
        satAzimuth    = ZERO;
        satElevation  = -ONEPPM / 2.0;
        satRange      = ZERO;
        rangeRate     = ZERO;
        satVelocity   = ZERO;

        if (satTypeFlag == STS && preLaunchFlag)
        {
            satLat     = KSCLAT;
            satLong    = KSCLONG;
            satHeight  = KSCALT;
            prevSatLat = KSCLAT - ONEPPM;
        }

        else
        {
            satLat     = ZERO;
            satLong    = ZERO;
            satHeight  = ZERO;
            prevSatLat = -ONEPPM;
        }
    }

    if (trackObject)
    {
        satRange      = ZERO;
        rangeRate     = ZERO;
    }

    cUnit = (satTypeFlag == STS) ? CKMNM : 1.0;

    if (satElevation > ZERO && !elevationFlag)
    {
        gotoXY(nX+headerCol,nY+1);
        doBeep();
        reverse();
        printf("%s",header);
        normal();
        elevationFlag = TRUE;
    }

    if (satElevation <= ZERO && elevationFlag)
    {
        gotoXY(nX+headerCol,nY+1);
        doBeep();
        underline();
        printf("%s",header);
        normal();
        elevationFlag = FALSE;
    }

    timeArg = curTime + timeZone;
    tDayNum = (long) timeArg;
    convertTime(timeArg,&dummyi,&tHour,&tMin,&tSec);
    getDate(tDayNum,&tYear,&tMonth,&tDay,&tYearDay);

    sprintf(localDate,"%02d%3s%02d",tDay,monthName(tMonth),tYear%100);
    sprintf(localTime,"%02d:%02d:%02d",tHour,tMin,tSec);
    sprintf(xTimeStr,"%7s  %8s %s",localDate,localTime,timeZoneStr);

    if (tYearDay != lastYearDay)
    {
        gotoXY(nX+38,nY+3);
        printf("%3s  %7s",dayName(tDayNum%7),localDate);
        gotoXY(nX+38,nY+4);
        printf("%-3d %8s",tYearDay,localTime);
        lastYearDay = tYearDay;
    }

    else
    {
        gotoXY(nX+42,nY+4);
        printf("%8s",localTime);
    }

    if (debugFlag)
    {
        gotoXY(nX+42,nY+5);
        printf("%8ld",curYearSec);

        gotoXY(nX+42,nY+6);
        convertTime(lasTime*CRREV,&dummyi,&lastHour,&lastMin,&lastSec);
        printf("%02d:%02d:%02d",lastHour,lastMin,lastSec);
    }

    if (trackDispFlag)
    {
        gotoXY(nX+46,nY+8);

        if (trackingFlag)
        {
            reverse();

            if (trackingFlag == ON)
                printf("  ON");

            if (trackingFlag == AUTOTRK)
                printf("AUTO");

            if (trackingFlag == ERROR)
                printf(" ERR");

            normal();
        }

        else
        {
            printf(" OFF");
        }

        trackDispFlag = FALSE;
    }

    gotoXY(nX+18,nY+5);
    printf("%7.3f",curInclination*CRD);

    if (satTypeFlag == STS)
    {
        gotoXY(nX+14,nY+6);
        printf("%5ld",stsOrbitNum);
        gotoXY(nX+20,nY+6);
        printf("%5.1f",stsOrbitFract);
    }

    else
    {
        gotoXY(nX+14,nY+6);
        printf("%5ld",orbitNum);
        gotoXY(nX+20,nY+6);
        printf("%5.1f",orbitFract);
    }

    gotoXY(nX+28,nY+6);

    if (orbitNum > 0 || stsOrbitNum > 0)
    {
        if (eclipseCode == VISIBLE) reverse();
        printf("%s",visibCode[eclipseCode]);
        if (eclipseCode == VISIBLE) normal();
    }

    else
        printf("%s",visibCode[BLANK]);

    if (newTrackObjectFlag)
    {
        gotoXY(nX+1,nY+7);
        if (trackObject == SUN) reverse();
        printf("Sun ");
        if (trackObject == SUN) normal();
    }

    gotoXY(nX+14,nY+7);
    printf("%5.1f %5.1f",sunAzimuth*CRD,sunElevation*CRD);

    if (newTrackObjectFlag)
    {
        gotoXY(nX+1,nY+8);
        if (trackObject == MOON) reverse();
        printf("Moon");
        if (trackObject == MOON) normal();
    }

    gotoXY(nX+14,nY+8);
    printf("%5.1f %5.1f",moonAzimuth*CRD,moonElevation*CRD);

    /* the next lines are useful for solar eclipses and testing purposes */
/*
    gotoXY(nX+13,nY+9);
    printf("%6.1f %5.1f",sunLong*CRD,sunLat*CRD);

    gotoXY(nX+14,nY+9);
    printf("%7.3f %7.3f",(sunAzimuth - moonAzimuth)*CRD,
                         (sunElevation - moonElevation)*CRD);
*/

    /* the next lines are needed for checking the positions of Sun and Moon */
/*
    gotoXY(nX+0,nY+9);
    printf("Sun   RA=%10.6f Dec=%10.6f R=%.3f",sunRA*CRH,sunDec*CRD,sunDist);
    gotoXY(nX+0,nY+9);
    printf("Moon  RA=%10.6f Dec=%10.6f R=%.3f",moonRA*CRH,moonDec*CRD,moonDist);
*/

    /* the next lines are needed for checking the orbit calculations */
/*
    gotoXY(nX+0,nY+9);
    printf("Orbit: %.2f km x %.2f km",perigeeHeight,apogeeHeight);
*/

    /* the next lines are needed for checking the orbit number calculations */
/*
    if (debugFlag)
    {
        if(xTermFlag)
        {
            gotoXY(nX+0,nY+27);
            printf("dH:%9.3f   MM0:%9.6f   MM:%9.6f",
                satRadius-perigeeHeight-EARTHRADIUS,
                epochMeanMotion,curMeanMotion);

            gotoXY(nX+0,nY+29);
            printf("stsOrbit   :%9.3f",stsOrbit);
            printf("  =  curOrbit:%9.3f  +  curArgPerigee:%9.3f",
                    curOrbit,curArgPerigee*CRREV);

            gotoXY(nX+0,nY+30);
            printf("curArgNodeX:%9.3f",curArgNodeX*CRREV);
        }

        else
        {
            gotoXY(nX+0,nY+9);
            printf("dH: %9.3f  ",satRadius-perigeeHeight-EARTHRADIUS);
            printf("stsOrbit :%9.3f = curOrbit:%9.3f + curArgPerigee:%5.3f",
                    curOrbit+curArgPerigee*CRREV,curOrbit,curArgPerigee*CRREV);
        }
    }
*/

    gotoXY(nX+61,nY+3);
    printf("%-4s",downlinkMode);
    gotoXY(nX+61,nY+4);
    printf("%-4s",uplinkMode);

    if (satElevation > ZERO) reverse();
    gotoXY(nX+67,nY+3);
    printf("%10.4f",(downlinkFreq + freqOffset + downlinkDopp)*CHZMHZ);
    gotoXY(nX+67,nY+4);
    printf("%10.4f",(uplinkFreq + freqOffset*xponderSign + uplinkDopp)*CHZMHZ);

    gotoXY(nX+67,nY+5);
    printf("%8.2f00",downlinkLoss);
    gotoXY(nX+67,nY+6);
    printf("%8.2f00",uplinkLoss);
    gotoXY(nX+67,nY+7);

    if (attitudeFlag)
        printf("%9.3f0",squintAngle*CRD);
    else
        printf("%9.3f0",sunPhaseAngle*CRD);

    gotoXY(nX+67,nY+8);
    printf("%8.2f00",satPhase);

    if (!numModes && satElevation > ZERO) normal();
    gotoXY(nX+78,nY+8);
    printf("%-2s",modeString);
    if (numModes && satElevation > ZERO) normal();

    if (trackObject == SAT)
    {
        dispAzimuth   = satAzimuth;
        dispElevation = satElevation;
    }

    if (trackObject == SUN)
    {
        dispAzimuth   = sunAzimuth;
        dispElevation = sunElevation;
    }

    if (trackObject == MOON)
    {
        dispAzimuth   = moonAzimuth;
        dispElevation = moonElevation;
    }

    gotoXY(nX+20,nY+10);
    if (dispElevation > ZERO) reverse();
    printf("%10.3f",dispAzimuth*CRD);
    gotoXY(nX+20,nY+11);
    printf("%10.3f",dispElevation*CRD);
    gotoXY(nX+35,nY+11);

    if (newTrackObjectFlag)
    {
        printf("_");
    }

    else
    {
        if (fabs(dispElevation) < ONEPPM && fabs(dispAzimuth) < ONEPPM)
            printf("/");

        else
        {
            if (prevDispEle > -PI - ONEPPM)
            {
                (dispElevation > prevDispEle) ? printf("/") : printf("\\");
            }
        }
    }

    prevDispEle = dispElevation;

    if (dispElevation > ZERO && trackObject) normal();
    gotoXY(nX+20,nY+12);
    printf("%10.3f",satRange*cUnit);
    gotoXY(nX+20,nY+13);
    printf("%10.3f",rangeRate*cUnit);
    if (dispElevation > ZERO && !trackObject) normal();

    newTrackObjectFlag = FALSE;

    gotoXY(nX+65,nY+10);
    printf("%6.3f",fabs(satLat*CRD));
    gotoXY(nX+76,nY+10);

    (satLat > ZERO) ? printf("N") : printf("S");
    
    if (prevSatLat > -PI - ONEPPM)
    {
        (satLat > prevSatLat) ? printf(" /") : printf(" \\");
    }

    prevSatLat = satLat;
    gotoXY(nX+64,nY+11);
    printf("%7.3f",fabs(satLong*CRD));
    gotoXY(nX+75,nY+11);

    (satLong > ZERO) ? printf(" W") : printf(" E");
    
    gotoXY(nX+61,nY+12);
    if (satHeight < ZERO) reverseBlink();
    printf("%10.3f",satHeight*cUnit);
    if (satHeight < ZERO) normal();
    gotoXY(nX+62,nY+13);
    printf("%9.3f",satVelocity*cUnit);

/*
    gotoXY(nX+18,nY+14);
    printf("%+10.3f",sitePosGl[0]*cUnit);
    gotoXY(nX+42,nY+14);
    printf("%+10.3f",sitePosGl[1]*cUnit);
    gotoXY(nX+66,nY+14);
    printf("%+10.3f",sitePosGl[2]*cUnit);
*/

    gotoXY(nX+18,nY+15);
    printf("%+10.3f",satPosGl[0]*cUnit);
    gotoXY(nX+42,nY+15);
    printf("%+10.3f",satPosGl[1]*cUnit);
    gotoXY(nX+66,nY+15);
    printf("%+10.3f",satPosGl[2]*cUnit);

/*
    gotoXY(nX+18,nY+16);
    printf("%+10.3f",satPosS[0]*cUnit);
    gotoXY(nX+42,nY+16);
    printf("%+10.3f",satPosS[1]*cUnit);
    gotoXY(nX+66,nY+16);
    printf("%+10.3f",satPosS[2]*cUnit);
*/

    gotoXY(nX+18,nY+16);
    printf("%+10.3f",satVelGl[0]*cUnit);
    gotoXY(nX+42,nY+16);
    printf("%+10.3f",satVelGl[1]*cUnit);
    gotoXY(nX+66,nY+16);
    printf("%+10.3f",satVelGl[2]*cUnit);

/*
    gotoXY(nX+18,nY+16);
    printf("%+10.3f",satVelS[0]*cUnit);
    gotoXY(nX+42,nY+16);
    printf("%+10.3f",satVelS[1]*cUnit);
    gotoXY(nX+66,nY+16);
    printf("%+10.3f",satVelS[2]*cUnit);
*/

/*
    gotoXY(nX+6,nY+27);
    printf("Satellite:");
    gotoXY(nX+18,nY+27);
    printf("%+12.9f",localVecSatGl[0]);
    gotoXY(nX+33,nY+27);
    printf("%+12.9f",localVecSatGl[1]);
    gotoXY(nX+48,nY+27);
    printf("%+12.9f",localVecSatGl[2]);

    gotoXY(nX+6,nY+28);
    printf("Sun:");
    gotoXY(nX+18,nY+28);
    printf("%+12.9f",localVecSunGl[0]);
    gotoXY(nX+33,nY+28);
    printf("%+12.9f",localVecSunGl[1]);
    gotoXY(nX+48,nY+28);
    printf("%+12.9f",localVecSunGl[2]);

    gotoXY(nX+6,nY+29);
    printf("Moon:");
    gotoXY(nX+18,nY+29);
    printf("%+12.9f",localVecMoonGl[0]);
    gotoXY(nX+33,nY+29);
    printf("%+12.9f",localVecMoonGl[1]);
    gotoXY(nX+48,nY+29);
    printf("%+12.9f",localVecMoonGl[2]);
*/

    if (geoSyncFlag != GEOSTAT)
    {
        if (newRiseFlag)
        {
            gotoXY(nX+6,nY+18);

            if (satElevation <= ZERO)
            {
                reverse();
                printf("Next AOS  :");
                normal();
            }

            else
                printf("Last AOS  :");

            timeArg = nextRiseTime + timeZone;
            tDayNum = (long) timeArg;
            convertTime(timeArg,&dummyi,&tHour,&tMin,&tSec);
            getDate(tDayNum,&dummyi,&dummyi,&dummyi,&tYearDay);
            gotoXY(nX+18,nY+18);
            printf("%3d/%02d:%02d:%02d",tYearDay,tHour,tMin,tSec);
        }

        convertTime(ZERO,&tYearDay,&tHour,&tMin,&tSec);

        if (satElevation <= ZERO && nextRiseTime - curTime >= ZERO)
            convertTime(nextRiseTime-curTime,&tYearDay,&tHour,&tMin,&tSec);

        if (satElevation > ZERO && nextSetTime - curTime >= ZERO)
            convertTime(nextSetTime-curTime,&tYearDay,&tHour,&tMin,&tSec);

        if (tYearDay == 0 && tHour == 0 && tMin < 1)
        {
            countdownFlag = TRUE;
            doBeep();
        }

        else
            countdownFlag = FALSE;

        if (countdownFlag && checkCountdown)
        {
            gotoXY(nX+6,nY+21);
            reverseBlink();
            printf("Countdown :");
            normal();
            checkCountdown = FALSE;
        }

        if (!countdownFlag && !checkCountdown)
        {
            gotoXY(nX+6,nY+21);
            reverse();
            printf("Countdown :");
            normal();
            checkCountdown = TRUE;
        }

        gotoXY(nX+18,nY+21);
        if (countdownFlag || satElevation > ZERO) reverse();
        printf("%3d/%02d:%02d:%02d",tYearDay,tHour,tMin,tSec);
        if (countdownFlag || satElevation > ZERO) normal();

        if (satElevation > ZERO && passDispFlag)
        {
            gotoXY(nX+6,nY+18);
            printf("Last AOS  :");
            gotoXY(nX+6,nY+20);
            reverse();
            printf("Next LOS  :");
            normal();
            passDispFlag = FALSE;
        }

        if (newRiseFlag)
        {
            convertTime(nextSetTime-nextRiseTime,&tYearDay,&tHour,&tMin,&tSec);
            gotoXY(nX+18,nY+19);
            printf("%3d/%02d:%02d:%02d",tYearDay,tHour,tMin,tSec);

            if (satElevation <= ZERO)
            {
                gotoXY(nX+6,nY+20);
                printf("Next LOS  :");
            }

            timeArg = nextSetTime + timeZone;
            tDayNum = (long) timeArg;
            convertTime(timeArg,&dummyi,&tHour,&tMin,&tSec);
            getDate(tDayNum,&dummyi,&dummyi,&dummyi,&tYearDay);
            gotoXY(nX+18,nY+20);
            printf("%3d/%02d:%02d:%02d",tYearDay,tHour,tMin,tSec);

            gotoXY(nX+64,nY+18);
            printf("%5.1f00",riseAzimuth*CRD);
            gotoXY(nX+77,nY+18);
            printf("%s",visibCode[eclipseRise]);

            gotoXY(nX+64,nY+19);
            printf("%5.1f00",maxAzimuth*CRD);
            gotoXY(nX+77,nY+19);
            printf("%s",visibCode[eclipseMax]);

            gotoXY(nX+64,nY+20);
            printf("%5.1f00",setAzimuth*CRD);
            gotoXY(nX+77,nY+20);
            printf("%s",visibCode[eclipseSet]);

            gotoXY(nX+61,nY+21);
            if (maxElevation > MAXELELIMIT) reverse();
            printf("%8.1f00",maxElevation*CRD);
            gotoXY(nX+61,nY+22);
            printf("%8.1f00",maxRange*cUnit);
            if (maxElevation > MAXELELIMIT) normal();

            newRiseFlag  = FALSE;
            passDispFlag = TRUE;
        }
    }

    if (launchFlag)
    {
        getMetStr(preLaunchFlag,curTime,launchEpoch,str);

        gotoXY(nX+17,nY+22);
        printf("%s",str);
        sprintf(xMetStr,"MET: %s",str);

        fabsMET = fabs(curTime - launchEpoch);

        if (fabsMET <= NINEMIN && fabsMET > ONEMIN && preLaunchFlag && 
            !launchMsgA)
        {
            gotoXY(nX+6,nY+23);
            doBeep();
            reverse();
            printf("Final Launch Countdown  ");
            normal();
            launchMsgA = TRUE;
        }

        if (fabsMET <= ONEMIN && preLaunchFlag)
        {
            doBeep();

            if (!launchMsgB)
            {
                gotoXY(nX+6,nY+23);
                reverseBlink();
                printf("Final Launch Countdown  ");
                normal();
                launchMsgB = TRUE;
            }
        }

        if (!preLaunchFlag && preOrbitFlag && !launchMsgC)
        {
            gotoXY(nX+6,nY+23);
            doBeep();
            reverse();
            printf("Ascending to Orbit      ");
            normal();
            launchMsgC = TRUE;
        }

        if (!preLaunchFlag && !preOrbitFlag && launchMsgC && !launchMsgD)
        {
            gotoXY(nX+1,nY+23);
            doBeep();
            clearCurs();
            launchMsgD = TRUE;
        }
    }

    else
        *xMetStr = '\0';

    getMaidenHead(satLat,satLong,maidenHead);

    getGroundTrack(satLat,satLong,
                   &cityLat,&cityLong,&gndTrkDist,gndTrkDir,gndTrkCity);

    *xGndTrkStr = '\0';
    poleFlag = FALSE;

    if (satTypeFlag == STS && launchFlag)
    {
        if (preLaunchFlag)
        {
            newCityNum =  -2;
            gndTrkDist = 0.0;
            sprintf(gndTrkDir,"N");
            sprintf(gndTrkCity,"Launch Complex 39, KSC, FL, USA");
        }

        if (!preLaunchFlag && preOrbitFlag)
        {
            newCityNum =  -3;
            gndTrkDist = 0.0;
            *maidenHead = '\0';
            sprintf(gndTrkDir,"NE");
            sprintf(gndTrkCity,"the Launch Pad --- Ascending to Orbit");
        }
    }

    if (satTypeFlag != STS && launchFlag && preOrbitFlag)
    {
        newCityNum =  -2;
        gndTrkDist = 0.0;
        *maidenHead = '\0';
        sprintf(gndTrkDir,"N");
        sprintf(gndTrkCity,"the Launch Pad");
    }

    if (!launchFlag && preOrbitFlag)
    {
        clearLine(nX+15,nY+24);
    }

    else
    {
        gotoXY(nX+15,nY+24);
        printf("%6s",maidenHead);

        gotoXY(nX+24,nY+24);
        printf("%7.1f ",gndTrkDist*cUnit);
        printf("%2s %3s of ",(satTypeFlag == STS) ? "nm" : "km",gndTrkDir);

        poleFlag = (!strcmp(gndTrkCity,"North Pole") || 
                    !strcmp(gndTrkCity,"South Pole")) ? TRUE : FALSE;

        if (oldCityNum != newCityNum)
        {
            if (poleFlag)                              /* North or South Pole */
                printf("the %-33s",gndTrkCity);
            else
                printf("%-37s",gndTrkCity);
        }
    }

    sprintf(xGndTrkStr,"%6s    %6.1f %2s %3s of %s%s",maidenHead,
            gndTrkDist*cUnit,(satTypeFlag == STS) ? "nm" : "km",
            gndTrkDir,(poleFlag) ? "the " : "",gndTrkCity);

    oldCityNum = newCityNum;

    lastX = nX + 80;
    lastY = nY + 24;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* getMetStr: gets MET (mission elapsed time) character string                */
/*                                                                            */
/******************************************************************************/

void getMetStr(plFlag,cTime,lTime,mStr)

double cTime, lTime;
int    plFlag;
char   *mStr;

{
    double MET;
    int    METday, METhour, METmin, METsec;

    MET = cTime - lTime;

    if (plFlag)
        MET *= -1.0;

    convertTime(MET,&METday,&METhour,&METmin,&METsec);

    if (plFlag)
    {
        if (METday)
            sprintf(mStr,"%4d/%02d:%02d:%02d",
                 -METday,METhour,METmin,METsec);
        else
            sprintf(mStr,"  -%d/%02d:%02d:%02d",
                -METday,METhour,METmin,METsec);
    }

    else
        sprintf(mStr,"%4d/%02d:%02d:%02d",METday,METhour,METmin,METsec);

    return;
}

/******************************************************************************/
/*                                                                            */
/* updateMultiSatLiveDisp: updates the tracking live display for multiple     */
/*                         satellites                                         */
/*                                                                            */
/******************************************************************************/

void updateMultiSatLiveDisp(curTime)

double curTime;

{
    double timeArg, nxtRiseT, nxtSetT, elev, cUnit;
    long   tDayNum;
    int    i, j, k, n, slen, tYear, tMonth, tDay, tYearDay, tHour, tMin, tSec;
    char   str[80];

    n = (gndTrkFlag) ? XDISPCOLUMNS - DISPCOLUMNS : 0;

    if (trackDispFlag)
    {
        if (xTermFlag)
            gotoXY(69,3);
        else
            gotoXY(65,1);

        if (trackingFlag)
        {
            reverse();

            if (trackingFlag == ON)
                printf("  ON");

            if (trackingFlag == AUTOTRK)
                printf("AUTO");

            if (trackingFlag == ERROR)
                printf(" ERR");

            normal();
        }

        else
        {
            if (!xTermFlag)
            {
                underline();
                printf("    ");
                normal();
            }

            else
                printf(" OFF");
        }

        trackDispFlag = FALSE;
    }

    timeArg = curTime + timeZone;
    tDayNum = (long) timeArg;
    convertTime(timeArg,&dummyi,&tHour,&tMin,&tSec);
    getDate(tDayNum,&tYear,&tMonth,&tDay,&tYearDay);

    sprintf(localDate,"%02d%3s%02d",tDay,monthName(tMonth),tYear%100);
    sprintf(localTime,"%02d:%02d:%02d",tHour,tMin,tSec);
    sprintf(xTimeStr,"%7s  %8s %s",localDate,localTime,timeZoneStr);

    if (tYearDay != lastYearDay)
    {
        gotoXY(46+n,3);
        printf("%3s  %7s",dayName(tDayNum%7),localDate);
        gotoXY(67+n,3);
        printf("%3d  %8s",tYearDay,localTime);
        lastYearDay = tYearDay;
    }

    else
    {
        gotoXY(72+n,3);
        printf("%8s",localTime);
    }

    if (numSats > numSatLines)
    {
        gotoXY(3,6);
        printf("%3d-%3d (%3d)",satLinePtr+1,satLinePtr+numSatLines,numSats);
    }

    sortSats();

    for (i = 0; i < numSatLines; i++)
    {
        j = satOrder[satLinePtr+i];

        elev     = sat[j].satEle;
        nxtRiseT = sat[j].nextRiseT;
        nxtSetT  = sat[j].nextSetT;

        gotoXY(1,i+7);

        if (sat[j].statusFl == SELECT)
        {
            reverse();
            printf(">");
            normal();
            reverseFlag = FALSE;
        }

        if (sat[j].statusFl == MARK)
            printf("-");

        if (sat[j].statusFl == FALSE)
            bl();

        if (sat[j].geosyncFl == GEOSTAT)
        {
            if (elev > ZERO)
            {
                reverse();
                reverseFlag = TRUE;
            }
        }

        else
        {
            if (!sat[j].preorbitFl)
            {
                if (elev <= ZERO && nxtRiseT-curTime < ONEMIN) 
                {
                    if (sat[j].statusFl == SELECT)
                        doBeep();

                    reverseBlink();
                    reverseFlag  = TRUE;
                    justAcquFlag = TRUE;
                }

                if (elev > ZERO && fabs(nxtSetT-curTime) < ONEMIN) 
                {
                    if (sat[j].statusFl == SELECT)
                        doBeep();

                    reverseBlink();
                    reverseFlag = TRUE;
                }

                if (elev > ZERO && fabs(nxtSetT-curTime) >= ONEMIN) 
                {
                    reverse();
                    reverseFlag = TRUE;
                }

                if (elev > TRACKLIMIT1 && nxtRiseT-curTime > TENMIN)
                {
                    reverse();
                    reverseFlag  = TRUE;
                    justLostFlag = TRUE;
                }
            }
        }

        if (objectNumFlag)
            sprintf(str,"%ld",sat[j].satIdNum);
        else
            sprintf(str,"%s",sat[j].satellite);

        printf("%-s",str);

        slen = strlen(str);

        if (slen < MAXSATNAMELEN)
        {
            bl();

            for (k = 0; k < MAXSATNAMELEN - slen - 1; k++)
                printf(".");
        }

        if (debugFlag)
        {
            gotoXY(11,i+7);
            if (sat[j].satTypeFl == STS)
                printf("%5.1f",sat[j].stsOrbFrac);
            else
                printf("%5.1f",sat[j].satOrbFrac);
        }

        if (justAcquFlag || justLostFlag)
        {
             normal();
             reverseFlag  = FALSE;
             justAcquFlag = FALSE;
             justLostFlag = FALSE;
        }

        if (sat[j].geosyncFl == GEOSTAT || sat[j].crashFl || sat[j].nopassFl)
        {
            if (sat[j].geosyncFl == GEOSTAT)
                printf("  Geostationary satellite                     ");

            if (sat[j].crashFl)
            {
                printf("  Satellite has crashed already");
                clearCurs();
            }

            if (sat[j].geosyncFl != GEOSTAT && !sat[j].crashFl && 
                sat[j].nopassFl)
            {
                printf("  No pass for %6.3f days                     ",
                    nxtRiseT - curTime);
            }
        }

        else
        {
            convertTime(ZERO,&tYearDay,&tHour,&tMin,&tSec);

            if (elev <= ZERO && nxtRiseT-curTime >= ZERO)
                convertTime(nxtRiseT-curTime,&tYearDay,&tHour,&tMin,&tSec);

            if (elev > ZERO && nxtSetT-curTime >= ZERO)
                convertTime(nxtSetT-curTime,&tYearDay,&tHour,&tMin,&tSec);

            printf("%3d/%02d:%02d:%02d  ",tYearDay,tHour,tMin,tSec);

            if (elev <= ZERO)
            {
                tDayNum = (long) (nxtRiseT + timeZone);
                convertTime(nxtRiseT+timeZone,&dummyi,&tHour,&tMin,&tSec);
            }

            else
            {
                tDayNum = (long) (nxtSetT + timeZone);
                convertTime(nxtSetT+timeZone,&dummyi,&tHour,&tMin,&tSec);
            }

            getDate(tDayNum,&tYear,&tMonth,&tDay,&tYearDay);
            printf("%3d/%02d:%02d:%02d ",tYearDay,tHour,tMin,tSec);

            convertTime(nxtSetT-nxtRiseT,&tDay,&tHour,&tMin,&tSec);
            printf("%3d:%02d:%02d  ",tDay*24+tHour,tMin,tSec);

            printf("%2.0f",sat[j].maxEle*CRD);

            (sat[j].maxEle > MAXELELIMIT) ? printf("* ") : printf("  ");

            printf("%s%s%s ",visibCode[sat[j].eclRise],
                             visibCode[sat[j].eclMax],
                             visibCode[sat[j].eclSet]);
        }

        *xMetStr = '\0';

        if (sat[j].launchFl)
        {
            getMetStr(sat[j].prelaunchFl,curTime,sat[j].launchEp,str);
            sprintf(xMetStr,"MET: %s",str);
        }

        strcpy(sat[j].xMetStr,xMetStr);

        if (sat[j].launchFl && sat[j].preorbitFl)
        {
            reverse();

            if (sat[j].prelaunchFl)
                printf("  not yet launched");
            else
                printf("ascending to orbit");

            normal();
            reverseFlag = FALSE;
            clearCurs();
        }

        if (!sat[j].preorbitFl && !sat[j].crashFl)
        {
            printf("%s  ",visibCode[sat[j].eclCode]);

            printf("%3.0f %3.0f ",sat[j].satAzi*CRD,sat[j].satEle*CRD);

            if (firstMultiSatDisp)
                printf("_");

            else
            {
                if (sat[j].satLastEle < sat[j].satEle)
                    printf("/");
                else
                    printf("\\");
            }

            printf("%6.0f",sat[j].satRng);
        }

        if (!sat[j].crashFl && gndTrkFlag)
        {
            if (!strncmp(sat[j].model,TLEMEANMODEL,4))
                printf(" M ");

            if (!strncmp(sat[j].model,LOWEARTHMODEL,4))
                printf(" G ");

            if (!strncmp(sat[j].model,DEEPSPACEMODEL,4))
                printf(" D ");
        }

        if (!sat[j].preorbitFl && !sat[j].crashFl && gndTrkFlag)
        {
            printf("%4.0f km %3s %-37s",
                sat[j].ctyDist,sat[j].ctyDir,sat[j].ctyStr);
        }

        if (reverseFlag)
        {
            normal();
            reverseFlag = FALSE;
        }
    }

    for (j = 0; j < numSats; j++)
    {
        if (sat[j].crashFl && sat[j].statusFl == SELECT)
            *xGndTrkStr = '\0';

        if (!sat[j].crashFl && sat[j].statusFl == SELECT)
        {
            poleFlag = (!strcmp(sat[j].ctyStr,"North Pole") || 
                        !strcmp(sat[j].ctyStr,"South Pole")) ? TRUE : FALSE;

            getMaidenHead(sat[j].satLtd,sat[j].satLng,maidenHead);

            cUnit = (sat[j].satTypeFl == STS) ? CKMNM : 1.0;

            sprintf(xGndTrkStr,"%6s    %6.1f %2s %3s of %s%s",maidenHead,
                sat[j].ctyDist*cUnit,(sat[j].satTypeFl == STS) ? "nm" : "km",
                sat[j].ctyDir,(poleFlag) ? "the " : "",sat[j].ctyStr);
        }
    }

    firstMultiSatDisp = FALSE;

    lastX = DISPCOLUMNS + n;
    lastY = numSatLines + 6;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* updateSatPointer: updates satellite pointer (cursor)                       */
/*                                                                            */
/******************************************************************************/

void updateSatPointer()

{
    int i, j, n;

    n = (gndTrkFlag) ? XDISPCOLUMNS - DISPCOLUMNS : 0;

    if (trackDispFlag)
    {
        if (xTermFlag)
            gotoXY(69,3);
        else
            gotoXY(65,1);

        if (trackingFlag)
        {
            reverse();

            if (trackingFlag == ON)
                printf("  ON");

            if (trackingFlag == AUTOTRK)
                printf("AUTO");

            if (trackingFlag == ERROR)
                printf(" ERR");

            normal();
        }

        else
        {
            if (!xTermFlag)
            {
                underline();
                printf("    ");
                normal();
            }

            else
                printf(" OFF");
        }

        trackDispFlag = FALSE;
    }

    for (i = 0; i < numSatLines; i++)
    {
        j = satOrder[satLinePtr+i];

        gotoXY(1,i+7);

        if (sat[j].statusFl == SELECT)
        {
            reverse();
            printf(">");
            normal();
            reverseFlag = FALSE;
        }

        if (sat[j].statusFl == MARK)
            printf("-");

        if (sat[j].statusFl == FALSE)
            bl();
    }

    lastX = DISPCOLUMNS + n;
    lastY = numSatLines + 6;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* updateSatPage: updates satellite page in multisat display                  */
/*                                                                            */
/******************************************************************************/

void updateSatPage()

{
    int  i, j, n;
    char str[80];

    n = (gndTrkFlag) ? XDISPCOLUMNS - DISPCOLUMNS : 0;

    if (numSats > numSatLines)
    {
        gotoXY(3,6);
        printf("%3d-%3d (%3d)",satLinePtr+1,satLinePtr+numSatLines,numSats);
    }

    for (i = 0; i < numSatLines; i++)
    {
        clearLine(1,i+7);
        j = satOrder[satLinePtr+i];

        if (objectNumFlag)
            sprintf(str,"%ld",sat[j].satIdNum);
        else
            sprintf(str,"%s",sat[j].satellite);

        printf(" %-s",str);
    }

    updateSatPointer();

    lastX = DISPCOLUMNS + n;
    lastY = numSatLines + 6;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* initSingleSatLiveDisp: initializes live display (text strings)             */
/*                                                                            */
/******************************************************************************/

void initSingleSatLiveDisp()

{
    char stsUnit[6], str[80];

    launchMsgA         = FALSE;
    launchMsgB         = FALSE;
    launchMsgC         = FALSE;
    launchMsgD         = FALSE;
    trackDispFlag      = TRUE;
    elevationFlag      = FALSE;
    newTrackObjectFlag = TRUE;
    quickStartFlag     = FALSE;
    lastYearDay        = -1;

    gotoXY(1,1);
    clearScreen();
    dispTitle();

    gotoXY(nX+1,nY+3);
    printf("Ground Stn : %15s",siteName);
    advCurs(3);
    printf("Date: ___  _______");
    advCurs(3);
    printf("Downlnk ____: _____.____ MHz");

    gotoXY(nX+1,nY+4);

    if (strlen(satAlias))
    {
        if (satTypeFlag == STS)
            printf("Shuttle    : %15s",satAlias);
        else
            printf("Satellite  : %15s",satAlias);
    }

    else
        printf("Satellite  : %15s",satName);

    advCurs(3);
    printf("%-4s: ___ __:__:__",timeZoneStr);
    advCurs(3);
    printf("Uplink  ____: _____.____ MHz");

    gotoXY(nX+1,nY+5);
    printf("Inclination:     ___.___ deg");

    if (debugFlag)
    {
        advCurs(3);
        printf("UTC :     ________");
        advCurs(3);
    }

    else
        advCurs(24);

    printf("Downlnk Loss:  ____.____ dB");

    gotoXY(nX+1,nY+6);
    printf("Orbit      : _____ ___._ %% _");

    if (debugFlag)
    {
        advCurs(3);
        printf("LAST:     __:__:__");
        advCurs(3);
    }

    else
    {
        advCurs(3);
        printf("Object  :   %6ld",satNum);
        advCurs(3);
    }

    printf("Uplink  Loss:  ____.____ dB");

    gotoXY(nX+1,nY+7);
    printf("Sun   Az/El: ___._ -__._ deg");
    advCurs(3);
    printf("Model   :%9s",propModel);
    advCurs(3);

    if (attitudeFlag)
        printf("Squint Angle:  ____.____ deg");
    else
        printf("Sun Angle   :  ____.____ deg");

    gotoXY(nX+1,nY+8);
    printf("Moon  Az/El: ___._ -__._ deg");
    advCurs(3);
    printf("Tracking:     ____");
    advCurs(3);
    printf("Phase (%4.0f):  ____.____ __",maxPhase);

    gotoXY(nX+6,nY+10);
    printf("Azimuth   :      ___.___ deg");
    advCurs(12);
    printf("Latitude     :     __.___ deg _ _");

    gotoXY(nX+6,nY+11);
    printf("Elevation :      -__.___ deg _");
    advCurs(10);
    printf("Longitude    :    ___.___ deg _");

    sprintf(stsUnit,(satTypeFlag == STS) ? "nm" : "km");

    gotoXY(nX+6,nY+12);
    printf("Range     :   ______.___ %2s",stsUnit);
    advCurs(13);
    printf("Height       : ______.___ %2s",stsUnit);

    gotoXY(nX+6,nY+13);
    printf("Range Rate:    -____.___ %2s/s",stsUnit);
    advCurs(11);
    printf("Velocity     :  _____.___ %2s/s",stsUnit);

/*
    gotoXY(nX+1,nY+14);
    printf("Site  Vector  X: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Y: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Z: -_____.___ %2s",stsUnit);
*/

    gotoXY(nX+1,nY+15);
    printf("State Vector  X: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Y: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Z: -_____.___ %2s",stsUnit);

/*
    gotoXY(nX+1,nY+16);
    printf("Norad Model   X: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Y: -_____.___ %2s",stsUnit);
    advCurs(8);
    printf("Z: -_____.___ %2s",stsUnit);
*/

    gotoXY(nX+1,nY+16);
    printf("             VX: -_____.___ %2s/s",stsUnit);
    advCurs(5);
    printf("VY: -_____.___ %2s/s",stsUnit);
    advCurs(5);
    printf("VZ: -_____.___ %2s/s",stsUnit);

    if (geoSyncFlag == GEOSTAT)
    {
        gotoXY(nX+12,nY+19);
        printf("Geostationary Satellite --- No Calculation of AOS and LOS");
    }

    else
    {
        gotoXY(nX+6,nY+18);
        printf("Next AOS  : ___/__:__:__ %-4s",timeZoneStr);
        advCurs(11);
        printf("AOS Azimuth  :    ___.___ deg  _");

        gotoXY(nX+6,nY+19);
        printf("Duration  : ___/__:__:__");
        advCurs(16);
        printf("MEL Azimuth  :    ___.___ deg  _");

        gotoXY(nX+6,nY+20);
        printf("Next LOS  : ___/__:__:__ %-4s",timeZoneStr);
        advCurs(11);
        printf("LOS Azimuth  :    ___.___ deg  _");

        gotoXY(nX+6,nY+21);
        reverse();
        printf("Countdown :");
        normal();
        printf(" ___/__:__:__");
        advCurs(16);
        printf("Max Elevation:     __.___ deg");
    }

    if (launchFlag)
    {
        gotoXY(nX+6,nY+22);
        printf("MET       : ___/__:__:__");
    }

    if (geoSyncFlag != GEOSTAT)
    {
        gotoXY(nX+46,nY+22);
        printf("MEL Range    : ______.___ %s",stsUnit);
    }

    gotoXY(nX+1,nY+24);
    sprintf(str,"_____________________________________");
    printf("Ground Track: ______   _____._ %2s ___ of %s",stsUnit,str);

    lastX = nX + 80;
    lastY = nY + 24;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* dispTitle: displays title for the single satellite live display            */
/*                                                                            */
/******************************************************************************/

void dispTitle()

{
    char satDispName[20], str[80], satStr[20];

    if (xTermFlag && didMultDispFlag)
    {
        nX = (numLinesX >= 30) ? 26 : 0;
        nY = (numLinesX >= 30) ?  2 : 0;
    }

    if (!xTermFlag || !didMultDispFlag)
    {
        nX = 0;
        nY = 0;
    }

    sprintf(satStr,"%s %s",sattrName,sattrVersion);
    sprintf(satDispName,"%s",satName);
    upperCase(satDispName);
    sprintf(header," %s %s ",satDispName,dispHeader);
    headerCol = 41 - (int) strlen(header) / 2;

    if (nX && nY)
    {
        gotoXY(nX,nY);
        underline();
        printf("%82c",' ');

        gotoXY(nX,nY+25);
        printf("%82c",' ');
        normal();

        sprintf(str,"%46s"," ");
        gotoXY(nX,nY+1);
    }

    else
    {
        sprintf(str,"%44s"," ");
        gotoXY(1,1);
    }

    underline();
    printf("%-14s %s %20s",satStr,str,callSign);

    gotoXY(nX+headerCol,nY+1);
    printf("%s",header);
    normal();

    return;
}

/******************************************************************************/
/*                                                                            */
/* initMultiSatLiveDisp: initializes multiple satellite live display          */
/*                                                                            */
/******************************************************************************/

void initMultiSatLiveDisp()

{
    int  i, j, n;
    char str[150], satStr[150];

    lastYearDay       = TRUE;
    trackDispFlag     = TRUE;
    reverseFlag       = FALSE;
    justLostFlag      = FALSE;
    didMultDispFlag   = TRUE;
    firstMultiSatDisp = TRUE;
    quickStartFlag    = FALSE;

    numSatLines = (xTermFlag) ? numLinesX - 6 : numLines - 6;

    if (numSats < numSatLines)
        numSatLines = numSats;

    n = (gndTrkFlag) ? XDISPCOLUMNS - DISPCOLUMNS : 0;

    gotoXY(1,1);
    clearScreen();
    underline();
    sprintf(satStr,"%8s %-6s",sattrName,sattrVersion);

    for (i = 0; i < 45 + n; i++)
        strcat(satStr," ");

    sprintf(satStr,"%s%20s",satStr,callSign);
    printf("%s",satStr);

    sprintf(header,"GROUP %s MULTISAT %s",satGroup,dispHeader);
    headerCol = (int) (DISPCOLUMNS + n + 1 - (int) strlen(header)) / 2;
    gotoXY(headerCol,1);
    printf("%s",header);
    normal();

    gotoXY(2,3);

    if (gndTrkFlag)
        printf("Ground Station: %-s",fullSiteName);
    else
        printf("Ground Station: %-s",siteName);

    if (xTermFlag)
    {
        gotoXY(59,3);
        printf("Tracking: ____");
    }

    gotoXY(40+n,3);
    printf("Date: ___  _______   %4s: ___  __:__:__",timeZoneStr);

    gotoXY(2,5);
    underline();
    printf("   Satellite  ");
    normal();
    bl();
    underline();
    printf(" Countdown ");
    normal();
    bl(); bl();
    underline();
    printf("Next AOS/LOS");
    normal();
    bl(); bl();
    underline();
    printf("Duration");
    normal();
    bl();
    underline();
    printf("MEL");
    normal();
    bl(); bl();
    underline();
    printf("Visib");
    normal();
    bl(); bl();
    underline();
    printf("Azi");
    normal();
    bl();
    underline();
    printf("Ele");
    normal();
    bl(); bl();
    underline();
    printf(" Range");
    normal();

    if (gndTrkFlag)
    {
        bl(); 
        underline();
        printf("M");
        normal();
        bl();
        underline();
        printf("              Ground Track Location              ");
        normal();
    }

    if (numSats > numSatLines)
    {
        gotoXY(3,6);
        printf("%3d-%3d (%3d)",satLinePtr+1,satLinePtr+numSatLines,numSats);
    }

    for (i = 0; i < numSatLines; i++)
    {
        gotoXY(2,i+7);
        j = satOrder[satLinePtr+i];

        if (objectNumFlag)
            sprintf(str,"%ld",sat[j].satIdNum);
        else
            sprintf(str,"%s",sat[j].satellite);

        printf("%-s",str);
    }

    updateSatPointer();

    lastX = DISPCOLUMNS + n;
    lastY = numSatLines + 6;

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* initSortSats: initializes sortSats()                                       */
/*                                                                            */
/******************************************************************************/

void initSortSats()

{
    int i;

    for (i = 0; i < numSats; i++)
        satOrder[i] = i;

    return;
}

/******************************************************************************/
/*                                                                            */
/* sortSats: orders satellites that are below the horizon by rise time and    */
/*           those above the horizon by reverse set time                      */
/*                                                                            */
/******************************************************************************/

void sortSats()

{
    int i, j, tmpIndex;

    initSortSats();

    /* sort all satellites by rise time */

    for (i = 0; i < numSats; i++)
    {
        for (j = 0; j < numSats - 1; j++)
        {
            if (sat[satOrder[j]].nextRiseT >= sat[satOrder[j+1]].nextRiseT)
            {
                tmpIndex      = satOrder[j];
                satOrder[j]   = satOrder[j+1];
                satOrder[j+1] = tmpIndex;
            } 
        }
    }

    /* resort all satellites that are above the horizon in reverse order */

    for (i = 0; i < numSats; i++)
    {
        for (j = 0; j < numSats - 1; j++)
        {
            if (sat[satOrder[j]].satEle   > ZERO && 
                sat[satOrder[j+1]].satEle > ZERO && 
                sat[satOrder[j]].nextSetT <= sat[satOrder[j+1]].nextSetT)
            {
                tmpIndex      = satOrder[j];
                satOrder[j]   = satOrder[j+1];
                satOrder[j+1] = tmpIndex;
            }
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* dispMessage: displays message string                                       */
/*                                                                            */
/******************************************************************************/

void dispMessage(msgStr)

char *msgStr;

{
    int  l, lCol, lRow;
    char dispStr[80];

    sprintf(dispStr,"%s ",msgStr);

    if (liveDispFlag == SINGLESAT)
    {
        if (xTermFlag && nX && nY)
        {
            if (numLinesX > 28)
                lCol = (XDISPCOLUMNS - 80) / 2 + 1;
            else
                lCol = (XDISPCOLUMNS - 80) / 2 - (int) strlen(dispStr);

            lRow = 29;
            clearLine(lCol,lRow);
            gotoXY(lCol,lRow);
        }

        else
        {
            oldCityNum = -1;
            clearLine(24,24);
            gotoXY(80 - (int) strlen(dispStr),24);
        }

        printf("%s",dispStr);
    }

    if (liveDispFlag == MULTISAT)
    {
        l = (xTermFlag) ? numLinesX : numLines;
        l = (xTermFlag && numSats < l - 9) ? numSats + 9 : l;

        clearLine(1,l);
        clearLine(1,l-1);

        printf(" %s",dispStr);
    }

    gotoXY(lastX,lastY);
    fflush(stdout);
    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function satdisp.c                                                  */
/*                                                                            */
/******************************************************************************/
