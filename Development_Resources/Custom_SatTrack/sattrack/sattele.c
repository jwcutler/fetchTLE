/******************************************************************************/
/*                                                                            */
/*  Title       : sattele.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 14Feb94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Interface routines for controlling the ISI telescopes.      */
/*                The main body of these routines will only be executed if    */
/*                'ISI' is defined in 'sattrack.h'. This also requires a      */
/*                corresponding change in the Makefile to allow the remote    */
/*                procedure call interface routines to be linked.             */
/*                                                                            */
/*  If 'ISI' is undefined in 'sattrack.h', those lines of the following       */
/*  functions that call certain other functions specific for the ISI          */
/*  telescopes are commented out. This way the code can be compiled           */
/*  elsewhere.                                                                */
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
#include <rpc/rpc.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

#ifdef ISI
#include "/usr1/isisoft/src/VxSource/include/globalserver.h"
#endif

/******************************************************************************/
/*                                                                            */
/* global fields                                                              */
/*                                                                            */
/******************************************************************************/

char *cpu[] = { "vme0", "vme1", "vme2" };              /* real-time computers */

/******************************************************************************/
/*                                                                            */
/* external functions                                                         */
/*                                                                            */
/******************************************************************************/

#ifdef ISI
extern int remotevar(), trackControl();
#endif

/******************************************************************************/
/*                                                                            */
/* initISI: initializes ISI telescope tracking control parameters if the      */
/*          tracking is not running already                                   */
/*                                                                            */
/******************************************************************************/

int initISI()

{
#ifdef ISI
    long clearFlag, solsysFlag, delayLine, trackFlag, trackingVME;
    long trackingCheck;
    int  i, didInit;
    char objName[80];
#endif

    int error;

    error = FALSE;

#ifdef ISI

    trackingCheck = 0L;                    /* check if tracking is ON already */
                                           /* and if 'xobserve' has altered   */
                                           /* the value of 'solsysfl'         */

    didInit = FALSE;

    for (i = 0; i <= 2; i++)                      /* talk vme0, vme1 and vme2 */
    {
        error += remotevar(cpu[i], "tracking", LONG | READ, &trackingVME);
        trackingCheck += trackingVME;

        if (trackingCheck != 0L)
        {
            error += remotevar(cpu[i], "solsysfl", LONG | READ, &solsysFlag);

            if (solsysFlag == SATCODE)
            {
                trackingCheck = 0L;
                didInit = TRUE;
            }
        }
    }

    if (trackingCheck == 0L && !error)
        trackingEnableFlag = TRUE;
    else
    {
        if (verboseFlag)
            printf("\ninitISI: error = %d  solsysFlag = %ld  tCheck = %ld\n",
                error,solsysFlag,trackingCheck);

        return(-1);
    }

    switch(trackObject)
    {
        case SUN:                                   /* avoid tracking the Sun */
            if (verboseFlag)
                printf("\ninitISI: tried to track the Sun\n");
            return(-1);
            break;

        case MOON:
            strcpy(objName,"Moon");
            break;

        case SAT:
            strcpy(objName,satTrackName);
            break;

        default:
            break;
    }

    solsysFlag  = SATCODE;
    clearFlag   = 1L;
    trackingVME = 1L;
    delayLine   = 1L;
    trackFlag   = 1L;
    error       = FALSE;

    error += remotevar(cpu[0], "srcname", STRING | WRITE, objName);

    if (!didInit)
        error += remotevar(cpu[0],"dlyblockflag", LONG | WRITE, &delayLine);

    for (i = 0; i <= 2; i++)                      /* talk vme0, vme1 and vme2 */
    {
        if (!didInit)
            error += remotevar(cpu[i], "solsysfl", LONG | WRITE, &solsysFlag);

        error += remotevar(cpu[i], "clearflag", LONG | WRITE, &clearFlag);
        error += remotevar(cpu[i], "tracking", LONG | WRITE, &trackingVME);

        if (!didInit)
            error += trackControl(cpu[i],trackFlag);

        if (verboseFlag && error)
            printf("\ninitISI oho: error = %d (vme%d)\n",error,i);
    }

#endif

    if (verboseFlag && error)
        printf("\ninitISI end: error = %d\n",error);

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* stopISI: stops tracking with the ISI telescopes                            */
/*                                                                            */
/******************************************************************************/

int stopISI()

{
#ifdef ISI
    long solsysFlag, trackFlag, trackingVME;
    int  i;
    char objName[80];
#endif

    int error;

    error = FALSE;

#ifdef ISI

    if (!trackingEnableFlag || trackingFlag == ERROR)
    {
        if (verboseFlag)
            printf("\nstopISI: trackingEnableFlage = %d  trackingFlag = %d\n",
                trackingEnableFlag,trackingFlag);

        return(-1);
    }

    trackingEnableFlag = FALSE;

    error += remotevar(cpu[0], "solsysfl", LONG | READ, &solsysFlag);

    if (solsysFlag != SATCODE || error)
    {
        if (verboseFlag)
            printf("\nstopISI: error = %d  solsysFlag = %ld\n",
                error,solsysFlag);

        return(-1);
    }

    *objName = '\0';

    solsysFlag  = 0L;
    trackingVME = 0L;
    trackFlag   = 0L;

    for (i = 0; i <= 2; i++)                   /* talk to vme0, vme1 and vme2 */
    {
        error += remotevar(cpu[i], "srcname", STRING | WRITE, objName);
        error += remotevar(cpu[i], "solsysfl", LONG | WRITE, &solsysFlag);
        error += remotevar(cpu[i], "tracking", LONG | WRITE, &trackingVME);
        error += trackControl(cpu[i],trackFlag);
    }

#endif

    if (verboseFlag && error)
        printf("\nstopISI end: error = %d\n",error);

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* controlISI: sends real-time tracking parameters to the ISI telescopes via  */
/*             remote procedure calls                                         */
/*                                                                            */
/*             local geodetic coordinate system for the ISI:                  */
/*             left-handed                                                    */
/*             +X = north,  +Y = east,  +Z = up                               */
/*             therefore the x coordinate needs to be inverted                */
/*                                                                            */
/******************************************************************************/

int controlISI()

{
#ifdef ISI
    double localVecX, localVecY, localVecZ, azimuth, elevation;
    long   solsysFlag;
    int    i, index;
    char   varName[40];
#endif

    int error;

    error = FALSE;

#ifdef ISI

    if (!trackingEnableFlag || trackingFlag == ERROR)
    {
        if (verboseFlag)
            printf("\ncontrolISI: trackingEnableFlag = %d  trackingFlag = %d\n",
                trackingEnableFlag,trackingFlag);

        return(-1);
    }

    error += remotevar(cpu[0], "solsysfl", LONG | READ, &solsysFlag);

    if (solsysFlag != SATCODE || error)
    {
        if (verboseFlag)
            printf("\ncontrolISI: error = %d  solsysFlag = %ld\n",
                error,solsysFlag);

        return(-1);
    }

    switch(trackObject)
    {
        case SUN:                                   /* avoid tracking the Sun */
            if (verboseFlag)
                printf("\ncontrolISI: tried to track the Sun\n");
            return(-1);
            break;

        case MOON:
            localVecX = -localVecMoonGl[0];
            localVecY =  localVecMoonGl[1];
            localVecZ =  localVecMoonGl[2];
            azimuth   =  moonAzimuth;
            elevation =  moonElevation;
            break;

        case SAT:
            localVecX = -localVecSatGl[0];
            localVecY =  localVecSatGl[1];
            localVecZ =  localVecSatGl[2];
            azimuth   =  satAzimuth;
            elevation =  satElevation;
            break;

        default:
            break;
    }

    index = (int) (curYearSec % 20L);           /* ring buffer has 20 entries */

    for (i = 0; i <= 2; i++)                   /* talk to vme0, vme1 and vme2 */
    {
        sprintf(varName,"timeindexSat[%d]",index);
        error += remotevar(cpu[i], varName, LONG | WRITE, &curYearSec);

        if (i == 0)                                     /* vme0 only          */
        {
            sprintf(varName,"azimuthSat[%d]",index);
            error += remotevar(cpu[i], varName, DOUBLE | WRITE, &azimuth);

            sprintf(varName,"altitudeSat[%d]",index);
            error += remotevar(cpu[i], varName, DOUBLE | WRITE, &elevation);
        }

        if (i > 0)                                      /* vme1 and vme2 only */
        {
            sprintf(varName,"scrxSat[%d]",index);
            error += remotevar(cpu[i], varName, DOUBLE | WRITE, &localVecX);

            sprintf(varName,"scrySat[%d]",index);
            error += remotevar(cpu[i], varName, DOUBLE | WRITE, &localVecY);

            sprintf(varName,"scrzSat[%d]",index);
            error += remotevar(cpu[i], varName, DOUBLE | WRITE, &localVecZ);
        }
    }

#endif

    if (verboseFlag && error)
        printf("\ncontrolISI end: error = %d\n",error);

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* End of function block sattele.c                                            */
/*                                                                            */
/******************************************************************************/
