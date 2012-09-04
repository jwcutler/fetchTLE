/******************************************************************************/
/*                                                                            */
/*  Title       : satprec.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 28Oct93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Precession routines for the Earth's axis of rotation.       */
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

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* getPrecMatrix: calculates the precession matrix for the mean equinox       */
/*                1950.0 using the rigorous formulae given in the             */
/*                "The Astronomical Almanac", 1983, page B18                  */
/*                and the matrix elements given in                            */
/*                "Explanatory Supplement to the Ephemeris", 1974, page 31    */
/*                                                                            */
/*                time used for calculation : Julian date referred to UTC     */
/*                                                                            */
/******************************************************************************/

void getPrecMatrix()

{
    double tu, tusq, tucb, zeta, zett, thet;
    double coszeta, coszett, costhet, sinzeta, sinzett, sinthet, c, d;
    int    i, j;

    if (fabs(julianDate - lastJulianDatePrec) < PRECUPDATEINT)
        return;

    lastJulianDatePrec = julianDate;

    tu   = (julianDate - JULDAT1950) / TROPCENT;
    tusq = tu*tu;
    tucb = tu*tusq;

    zeta = (0.6402633*tu + 0.0000839*tusq + 0.0000050*tucb)*CDR;
    zett = (0.6402633*tu + 0.0003036*tusq + 0.0000050*tucb)*CDR;
    thet = (0.5567376*tu - 0.0001183*tusq - 0.0000117*tucb)*CDR;

    coszeta = cos(zeta);
    coszett = cos(zett);
    costhet = cos(thet);
    sinzeta = sin(zeta);
    sinzett = sin(zett);
    sinthet = sin(thet);

    c = coszeta*costhet;
    d = sinzeta*costhet;

    /*  regular precession matrix */

    precMatrix[0][0] =  c*coszett - sinzeta*sinzett;
    precMatrix[0][1] = -d*coszett - coszeta*sinzett;
    precMatrix[0][2] = -sinthet*coszett;
    precMatrix[1][0] =  c*sinzett + sinzeta*coszett;
    precMatrix[1][1] = -d*sinzett + coszeta*coszett;
    precMatrix[1][2] = -sinthet*sinzett;
    precMatrix[2][0] =  coszeta*sinthet;
    precMatrix[2][1] = -sinzeta*sinthet;
    precMatrix[2][2] =  costhet;

    /* use unity matrix for now; this seems to be the right one */

    precMatrix[0][0] =  1.0;
    precMatrix[0][1] =  0.0;
    precMatrix[0][2] =  0.0;
    precMatrix[1][0] =  0.0;
    precMatrix[1][1] =  1.0;
    precMatrix[1][2] =  0.0;
    precMatrix[2][0] =  0.0;
    precMatrix[2][1] =  0.0;
    precMatrix[2][2] =  1.0;

    /* transposed precession matrix, probably not needed */

    for (i = 0; i <= 2; i++)
    {
        for (j = 0; j <= 2; j++)
        {
            precMatrixTr[i][j] = precMatrix[j][i];
        }
    }

/*
    printf("\nprecession matrices :\n");

    for (i = 0; i <= 2; i++)
        printf("%+2.10f      %+2.10f      %+2.10f\n",
                precMatrix[i][0],precMatrix[i][1],precMatrix[i][2]);
    printf("\n");

    for (i = 0; i <= 2; i++)
        printf("%+2.10f      %+2.10f      %+2.10f\n",
                precMatrixTr[i][0],precMatrixTr[i][1],precMatrixTr[i][2]);
    printf("\n");
*/

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satprec.c                                            */
/*                                                                            */
/******************************************************************************/
