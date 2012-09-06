/******************************************************************************/
/*                                                                            */
/*  Title       : satargs.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 21Nov94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines for command line parsing.                          */
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

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

#ifdef HPTERM
#include "hpterm.h"
#else
#include "vt100.h"
#endif

/******************************************************************************/
/*                                                                            */
/* checkArguments: checks if command line arguments are passed to main        */
/*                                                                            */
/******************************************************************************/

void checkArguments(argC,argV)

int  argC;
char *argV[];

{
    int  i, error, argCount;
    char defaultsFileStr[80];

    batchModeFlag    = FALSE;
    debugFlag        = FALSE;
    defaultsFileFlag = FALSE;
    graphicsFlag     = FALSE;
    modelFlag        = FALSE;
    quickStartFlag   = FALSE;
    timeZoneFlag     = FALSE;
    verboseFlag      = FALSE;
    error            = FALSE;

    if (argC >= 2)
    {
        argCount = 1;

        for (i = 1; i < argC; i++)
        {
            upperCase(argV[i]);

            if (!strncmp(argV[i],BATCH,2) && (int) strlen(argV[i]) == 2)
            {
                batchModeFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],DEBUG,2) && (int) strlen(argV[i]) == 2)
            {
                debugFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],DEFFILE,2) && (int) strlen(argV[i]) <= 4)
            {
                defaultsFileFlag = TRUE;
                strcpy(defaultsFileStr,argV[i]);
                argCount++;
            }

            if (!strncmp(argV[i],GRAPH,2) && (int) strlen(argV[i]) == 2)
            {
                graphicsFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],MODEL,2) && (int) strlen(argV[i]) == 2)
            {
                modelFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],QUICK,2) && (int) strlen(argV[i]) == 2)
            {
                quickStartFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],TIMEZONE,2) && (int) strlen(argV[i]) == 2)
            {
                timeZoneFlag = TRUE;
                argCount++;
            }

            if (!strncmp(argV[i],VERBOSE,2) && (int) strlen(argV[i]) == 2)
            {
                verboseFlag = TRUE;
                argCount++;
            }
        }

        if (argC != argCount && !batchModeFlag)
        {
            nl();
            printf("%s %s\n",sattrName,sattrVersion);
            doBeep();
            printf("Usage: sattrack [-b] [-d] [-fn] [-g] [-m] [-q] [-t] [-v]");
            printf("\n\n");
            exit(-1);
        }
    } 

    if (batchModeFlag)
    {
        debugFlag        = FALSE;
        defaultsFileFlag = FALSE;
        graphicsFlag     = FALSE;
        modelFlag        = FALSE;
        quickStartFlag   = FALSE;
        timeZoneFlag     = FALSE;
        verboseFlag      = FALSE;
        
        // Verify the the number of arguments for batch mode
        if (argC != 16)
        {
            nl();
            printf("%s %s Batch Mode",sattrName,sattrVersion);
            nl(); doBeep(); nl(); reverseBlink();
            printf(" Number of command line arguments is wrong \n");
            normal(); nl();

            error = TRUE;
        }

        if (error)
        {
            printf("Usage:    sattrack -b ");
            underline();
            printf("time-zone");
            normal();
            bl();
            underline();
            printf("station-name");
            normal();
            bl();
            underline();
            printf("station-lat");
            normal();
            bl();
            underline();
            printf("station-lon");
            normal();
            bl();
            underline();
            printf("sat-name");
            normal();
            nl();
            printf("          ");
            underline();
            printf("sat-tle-l1");
            normal();
            bl();
            underline();
            printf("sat-tle-l2");
            normal();
            bl();
            underline();
            printf("prediction-type");
            normal();
            bl();
            underline();
            printf("start-date");
            normal();
            bl();
            underline();
            printf("start-time");
            normal();
            nl();
            printf("          ");
            underline();
            printf("time-step [s]");
            normal();
            bl();
            underline();
            printf("duration [d]");
            normal();
            bl();
            underline();
            printf("min-elev [deg]");
            normal();
            bl();
            printf("[no]hardcopy");
            nl(); nl();
            
            exit(-1);
        }

        else
        {
            strcpy(batchTimeZone,argV[2]);
            strcpy(batchSiteName,argV[3]);
            strcpy(batchSiteLat,argV[4]);
            strcpy(batchSiteLong,argV[5]);
            strcpy(batchSatName,argV[6]);
            strcpy(batchTleL1,argV[7]);
            strcpy(batchTleL2,argV[8]);
            strcpy(batchPredType,argV[9]);
            strcpy(batchStartDate,argV[10]);
            strcpy(batchStartTime,argV[11]);
            strcpy(batchStepTime,argV[12]);
            strcpy(batchDuration,argV[13]);
            strcpy(batchMinElev,argV[14]);
            strcpy(batchHardcopy,argV[15]);
            //printf("%s-%s\n", batchMinElev, batchDuration);

            replaceBlanks(batchSiteName);
            replaceBlanks(batchSatName);
        }

    }

    if (defaultsFileFlag)
    {
        clipString(defaultsFileStr,2);
        defaultsFileNum = atoi(defaultsFileStr);

        if (defaultsFileNum < 0 || defaultsFileNum > 99 || 
            (int) strlen(defaultsFileStr) > 2 || 
            (defaultsFileNum < 10 && (int) strlen(defaultsFileStr) > 1))
        {
            nl();
            printf("%s %s\n",sattrName,sattrVersion);
            doBeep();
            printf("\nUsage: sattrack [-fn]  (n = 0,...,99)\n\n");
            exit(-1);
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satargs.c                                            */
/*                                                                            */
/******************************************************************************/
