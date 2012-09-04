/******************************************************************************/
/*                                                                            */
/*  Title       : satctrl.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 23Jan94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines for controlling satellite communications hardware  */
/*                (antenna interface, radio).                                 */
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
#include <string.h>
#include <math.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* control flags for the ISI telescope control system                         */
/*                                                                            */
/******************************************************************************/

#ifdef ISI
#undef  ANTENNATYPE
#define ANTENNATYPE   ISITELESCOPE
#undef  RADIOTYPE
#define RADIOTYPE     NONE
#endif

/******************************************************************************/
/*                                                                            */
/* trackAntenna: controls movement of the satellite antenna                   */
/*                                                                            */
/* THIS FUNCTION NEEDS TO BE ADAPTED TO THE PARTICULAR ANTENNA CONTROLLER     */
/* USED. SPECIFY THE ANTENNA CONTROLLER TYPE IN 'sattrack.h'.                 */
/*                                                                            */
/******************************************************************************/

void trackAntenna()

{
    double sendEle, sendEleRate, downFreq, upFreq;
    int    error;
    char   sendDownMode[20], sendUpMode[20];

    /* the next few lines are required here also, because some ground station */
    /* equipment like the TrakBox require the antenna and radio information   */
    /* to be sent out in one single command string                            */

    if (trackObject == SAT)
    {
        downFreq = (downlinkFreq + freqOffset + downlinkDopp) * CHZMHZ;
        upFreq   = (uplinkFreq + freqOffset*xponderSign + uplinkDopp) * CHZMHZ;

        sprintf(sendDownMode,"%s",downlinkMode);
        sprintf(sendUpMode,"%s",uplinkMode);
    }

    else
    {
        downFreq = (downlinkFreq + freqOffset) * CHZMHZ;
        upFreq   = (uplinkFreq + freqOffset*xponderSign) * CHZMHZ;

        sprintf(sendDownMode,"NONE");
        sprintf(sendUpMode,"NONE");
    }

    trackAziRate = trackAzimuth   - lastTrackAzi;
    trackEleRate = trackElevation - lastTrackEle;

    /* the next two lines are needed to make tracking smooth */
    /* for pass across north                                 */

    if (fabs(trackAzimuth - lastTrackAzi) > PI)
        trackAziRate += (trackAzimuth < lastTrackAzi) ? TWOPI : -TWOPI;

    /* the next four lines are needed to limit the tracking speed */
    /* of the antenna in azimuth and elevation                    */

    if (fabs(trackAziRate) > MAXAZIRATE)
        trackAziRate = (trackAziRate > 0.0) ? MAXAZIRATE : -MAXAZIRATE;

    if (fabs(trackEleRate) > MAXELERATE)
        trackEleRate = (trackEleRate > 0.0) ? MAXELERATE : -MAXELERATE;

    lastTrackAzi = trackAzimuth;
    lastTrackEle = trackElevation;

    /* the next two lines are needed to make tracking smooth near */
    /* the horizon in the presence of atmospheric refraction      */

    sendEle      = (trackElevation > HALFDEG) ? trackElevation : ZERO;
    sendEleRate  = (trackElevation > HALFDEG) ? trackEleRate   : ZERO;

    if (!trackCtrlFlag)
        return;

    switch(ANTENNATYPE) 
    {
        case NONE:
            break;

        case GENERIC:
            if (antennaFile)
            {
                fprintf(antennaFile,
                    "satAntenna: %8ld %8.3f %7.3f %7.3f %7.3f\n",
                    curYearSec,trackAzimuth*CRD,sendEle*CRD,
                    trackAziRate*CRD,sendEleRate*CRD);
                fflush(antennaFile);
            }

            break;

        case KANSASCITY:
            if (antennaFile)
            {
                /* THE NEXT LINE NEEDS TO BE CONFIGURED BY THE USER */

                fprintf(antennaFile,"%8.3f %7.3f %7.3f %7.3f\n",
                    trackAzimuth*CRD,sendEle*CRD,
                    trackAziRate*CRD,sendEleRate*CRD);
                fflush(antennaFile);
            }

            break;

        /************************************************************/
        /* JR1EDE, 04Jan95                                          */
        /*                                                          */
        /* JA6FTL TrakBox "HostMode"                                */
        /*        controls both radio and antenna by a serial line  */
        /*        and sends the mode and frequency infos to it's    */
        /*        attached radio by another serial line.            */
        /*        The baud rate is user configurable up to 9600.    */
        /*                                                          */
        /*        Newline: NL -> CR or NL -> NL                     */
        /*                                                          */
        /************************************************************/

        case TRAKBOX:
            if (antennaFile)
            {
                fprintf(antennaFile,"AZ%03.0f EL%02.0f ",
                    trackAzimuth*CRD,sendEle*CRD);

                /* send downlink only if mode is specified correctly */

                if (strlen(sendDownMode) && strncmp("NONE",sendDownMode,4))
                {
                    fprintf(antennaFile,"MD%s ",downlinkMode);
                    fprintf(antennaFile,"FD%011.0f ",downFreq*1e8);
                }

                /* send uplink only if mode is specified correctly */

                if (strlen(sendUpMode) && strncmp("NONE",sendUpMode,4))
                {
                    fprintf(antennaFile,"MU%s ",uplinkMode);
                    fprintf(antennaFile,"FU%011.0f",upFreq*1e8);
                }

                fprintf(antennaFile,"\n");
                fflush(antennaFile);
            }

            break;

        case ISITELESCOPE:
            error = controlISI();

            if (error)
            {
                trackingFlag  = ERROR;
                trackDispFlag = TRUE;
            }

            break;

        default:
            break;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* trackRadio: control satellite radio(s) (frequency, Doppler shift, etc.)    */
/*                                                                            */
/* THIS FUNCTION NEEDS TO BE ADAPTED TO THE PARTICULAR RADIO USED.            */
/* SPECIFY THE RADIO TYPE IN 'sattrack.h'.                                    */
/*                                                                            */
/******************************************************************************/

void trackRadio()

{
    double downFreq, upFreq;
    char   sendDownMode[20], sendUpMode[20];

    if (trackObject == SAT)
    {
        downFreq = (downlinkFreq + freqOffset + downlinkDopp) * CHZMHZ;
        upFreq   = (uplinkFreq + freqOffset*xponderSign + uplinkDopp) * CHZMHZ;

        sprintf(sendDownMode,"%s",downlinkMode);
        sprintf(sendUpMode,"%s",uplinkMode);
    }

    else
    {
        downFreq = (downlinkFreq + freqOffset) * CHZMHZ;
        upFreq   = (uplinkFreq + freqOffset*xponderSign) * CHZMHZ;

        sprintf(sendDownMode,"NONE");
        sprintf(sendUpMode,"NONE");
    }

    if (!trackCtrlFlag)
        return;

    switch(RADIOTYPE)
    {
        case NONE:
            break;

        case GENERIC:
            if (radioFileA)
            {
                fprintf(radioFileA,"satRadioA: %8ld  %s  %.6f  %s  %.6f\n",
                    curYearSec,downlinkMode,downFreq,uplinkMode,upFreq);
                fflush(radioFileA);
            }

            break;

        case FT736:
            if (radioFileA)
            {
                /* THE NEXT LINE NEEDS TO BE CONFIGURED BY THE USER */

                fprintf(radioFileA,"%.6f %.6f\n",downFreq,upFreq);
                fflush(radioFileA);
            }

            break;

        case IC970:
            if (radioFileA)
            {
                /* THE NEXT LINE NEEDS TO BE CONFIGURED BY THE USER */

                fprintf(radioFileA,"%.6f %.6f\n",downFreq,upFreq);
                fflush(radioFileA);
            }

            break;

        /*************************************/
        /* JR1EDE, 04Jan95                   */
        /* TS-790 serial control information */
        /*************************************/
        
        case TS790:
            if (radioFileA)
            {
                /* send downlink only if mode is specified correctly */

                if (strlen(sendDownMode) && strncmp("NONE",sendDownMode,4))
                {
                    fprintf(radioFileA,"MD%s ",downlinkMode);
                    fprintf(radioFileA,"FD%011.0f ",downFreq*1e8);
                }

                /* send uplink only if mode is specified correctly */

                if (strlen(sendUpMode) && strncmp("NONE",sendUpMode,4))
                {
                    fprintf(radioFileA,"MU%s ",uplinkMode);
                    fprintf(radioFileA,"FU%011.0f",upFreq*1e8);
                }

                fprintf(radioFileA,"\n");
                fflush(radioFileA);
            }

            break;

        case DUAL:
            if (radioFileA) /* downlink radio */
            {
                /* THE NEXT LINE NEEDS TO BE CONFIGURED BY THE USER */

                fprintf(radioFileA,"satRadioA: %8ld %s %.6f\n",
                    curYearSec,downlinkMode,downFreq);
                fflush(radioFileA);
            }

            if (radioFileB) /* uplink radio */
            {
                /* THE NEXT LINE NEEDS TO BE CONFIGURED BY THE USER */

                fprintf(radioFileB,"satRadioB: %8ld %s %.6f\n",
                    curYearSec,uplinkMode,upFreq);
                fflush(radioFileB);
            }

            break;

        default:
            break;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* trackingControl: controls communications hardware                          */
/*                                                                            */
/*               'lastTrackYearSec' is used to avoid duplicate messages when  */
/*               switching between SINGLE and MULTISAT live displays          */
/*                                                                            */
/*               'trackCtrlFlag' is used to inhibit the first message after   */
/*               changing the object to avoid big azimuth and elevation rates */
/*                                                                            */
/******************************************************************************/

void trackingControl()

{
    double trackLimit;

    switch(trackObject)
    {
        case SUN:
            trackAzimuth   = sunAzimuth;
            trackElevation = sunElevation;
            break;

        case MOON:
            trackAzimuth   = moonAzimuth;
            trackElevation = moonElevation;
            break;

        case SAT:
            trackAzimuth   = satAzimuth;
            trackElevation = satElevation;
            break;

        default:
            break;
    }

    trackLimit = (ANTENNATYPE  == ISITELESCOPE) ?         -PI : TRACKLIMIT1;
    trackLimit = (trackingFlag == AUTOTRK)      ? TRACKLIMIT2 : trackLimit;

    if (!preOrbitFlag && trackingFlag > 0 && trackElevation > trackLimit)
    {
        if (curYearSec != lastTrackYearSec)
        {
            trackAntenna();
            trackRadio();

            lastTrackYearSec = curYearSec; 
            trackCtrlFlag    = TRUE;
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* initSatIO: checks if I/O devices are accessible (owned by user)            */
/*                                                                            */
/******************************************************************************/

int initSatIO()

{
    int  error;
    char msgStr[80];

    if (!antennaFile && ANTENNATYPE != NONE)
    {
        sprintf(msgStr,"Trying to open '%s' ...",antennaIO);
        dispMessage(msgStr);

        antennaFile = fopen(antennaIO, "a");
    }

    if (!radioFileA && RADIOTYPE != NONE)
    {
        sprintf(msgStr,"Trying to open '%s' ...",radioIOA);
        dispMessage(msgStr);

        radioFileA = fopen(radioIOA, "a");
    }

    if (!radioFileB && RADIOTYPE == DUAL)
    {
        sprintf(msgStr,"Trying to open '%s' ...",radioIOB);
        dispMessage(msgStr);

        radioFileB = fopen(radioIOB, "a");
    }

    error = (!antennaFile || 
             (!radioFileA && ANTENNATYPE != TRAKBOX) || 
             (!radioFileB && RADIOTYPE == DUAL)) ? TRUE : FALSE;

    if (!error)
    {
        *msgStr = '\0';
        dispMessage(msgStr);
    }

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* closeSatIO: closes I/O devices                                             */
/*                                                                            */
/******************************************************************************/

int closeSatIO()

{
    int error;

    if (antennaFile)
    {
        fclose(antennaFile);
        antennaFile = NULL;
    }

    if (radioFileA)
    {
        fclose(radioFileA);
        radioFileA = NULL;
    }

    if (radioFileB)
    {
        fclose(radioFileB);
        radioFileB = NULL;
    }

    error = FALSE;
    return(error);
}

/******************************************************************************/
/*                                                                            */
/* End of function block satctrl.c                                            */
/*                                                                            */
/******************************************************************************/
