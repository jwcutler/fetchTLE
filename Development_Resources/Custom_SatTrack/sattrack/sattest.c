/******************************************************************************/
/*                                                                            */
/*  Title       : sattest.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 17Feb94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines for testing the sattrack code.                     */
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

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* doNoradTest: performs precision test of state vectors for various models   */
/*                                                                            */
/*              for reference see:                                            */
/*              "Spacetrack Report No. 3, Models of Propagation for NORAD     */
/*              Element Sets", F. R. Hoots and R. L. Roehrich, December 1980  */
/*                                                                            */
/******************************************************************************/

void doNoradTest()

{
    double curT, dT;
    int    i;

    for (i = 0; i <= 2; i++)
    {
        satPosGl[i] = 0.0;
        satVelGl[i] = 0.0;
    }

    printElements();
    checkPropModelType();

    printf("\nState vectors for NORAD precision test ");
    printf("with the '%s' model:\n\n",propModel);
    printf("Start time: %15.8f\n\n",epochDay);

    printf("   dT           X, Xdot              Y, Ydot              ");
    printf("Z, Zdot\n");
    printf(" [min]         [km, km/s]           [km, km/s]           ");
    printf("[km, km/s]\n\n");

    initNorad = TRUE;

    for (i = 0; i < 5; i++)
    {
        dT   = 0.25 * (double) i;
        curT = epochDay + dT;

        if (propModelType == LOWEARTH)
            getNoradStateVector(curT);
        else
            getStateVector(curT);              /* for now call TLE Mean Model */

        printf(" %4.0f   %19.9f  %19.9f  %19.9f\n",dT*1440.0,
                 satPosGl[0],satPosGl[1],satPosGl[2]);

        printf("        %19.9f  %19.9f  %19.9f\n\n",
                 satVelGl[0],satVelGl[1],satVelGl[2]);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block sattest.c                                            */
/*                                                                            */
/******************************************************************************/
