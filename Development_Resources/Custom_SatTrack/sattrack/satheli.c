/******************************************************************************/
/*                                                                            */
/*  Title       : satheli.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 17Oct93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines that calculate the positions of the Sun and the    */
/*                Moon. If 'moonFlag' or 'sunTransitFlag' are set, the update */
/*                timer is without effect.                                    */
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
/* getSunVector: calculates the apparent geocentric position of the Sun       */
/*               and the Moon using harmonic polynomials                      */
/*                                                                            */
/*               for reference see:                                           */
/*               T. C. Van Flandern and K. F. Pulkkinen,                      */
/*               Ap.J. (Suppl.) 41, 391-411 (1979)                            */
/*                                                                            */
/******************************************************************************/

void getSunVector(moonFlag)

int moonFlag;

{
    double cheby[20], a[31];
    double meanAnomMoon, meanLongMoon, longAscNodeMoon, argLatMoon;
    double meanElongMoon, meanLongSun, meanAnomSun;
    double meanAnomVenus, meanAnomMars, meanAnomJupiter;
    double tu, Tu, rP, U, V, W, raa, deca;
    int    i;

    if (!moonFlag && !sunTransitFlag && 
        fabs(julianDate - lastJulianDateSun) < SUNUPDATEINT)
        return;

    lastJulianDateSun = julianDate;

    tu = julianDate - JULDAT2000;                     /* t since 2000.0 [d]   */
    Tu = tu / JULCENT + 1.0;                          /* t since 1900.0 [jcy] */

    meanLongMoon    = reduce(0.606434 + 0.03660110129 * tu, -ONE, ONE);
    meanAnomMoon    = reduce(0.374897 + 0.03629164709 * tu, -ONE, ONE);
    argLatMoon      = reduce(0.259091 + 0.03674819520 * tu, -ONE, ONE);
    meanElongMoon   = reduce(0.827362 + 0.03386319198 * tu, -ONE, ONE);
    longAscNodeMoon = reduce(0.347343 - 0.00014709391 * tu, -ONE, ONE);
    meanLongSun     = reduce(0.779072 + 0.00273790931 * tu, -ONE, ONE);
    meanAnomSun     = reduce(0.993126 + 0.00273777850 * tu, -ONE, ONE);
    meanAnomVenus   = reduce(0.140023 + 0.00445036173 * tu, -ONE, ONE);
    meanAnomMars    = reduce(0.053856 + 0.00145561327 * tu, -ONE, ONE);
    meanAnomJupiter = reduce(0.056531 + 0.00023080893 * tu, -ONE, ONE);

    cheby[1]  = TWOPI * reduce(meanLongMoon,ZERO,ONE);
    cheby[2]  = TWOPI * reduce(meanAnomMoon,ZERO,ONE);
    cheby[3]  = TWOPI * reduce(argLatMoon,ZERO,ONE);
    cheby[4]  = TWOPI * reduce(meanElongMoon,ZERO,ONE) ;
    cheby[5]  = TWOPI * reduce(longAscNodeMoon,ZERO,ONE);
    cheby[7]  = TWOPI * reduce(meanLongSun,ZERO,ONE);
    cheby[8]  = TWOPI * reduce(meanAnomSun,ZERO,ONE);
    cheby[13] = TWOPI * reduce(meanAnomVenus,ZERO,ONE);
    cheby[16] = TWOPI * reduce(meanAnomMars,ZERO,ONE);
    cheby[19] = TWOPI * reduce(meanAnomJupiter,ZERO,ONE);

    /* Sun */

    rP = 1.00014 - 0.01675 * cos(cheby[8]) - 0.00014 * cos(2.0 * cheby[8]);

    U =  1.00000 - 
         0.03349 * cos(cheby[8]) - 
         0.00014 * cos(2.0 * cheby[8]) + 
         0.00008 * Tu * cos(cheby[8]) - 
         0.00003 * sin(cheby[8] - cheby[19]);

    V =  0.39785 * sin(cheby[7]) - 
         0.01000 * sin(cheby[7] - cheby[8]) +
         0.00333 * sin(cheby[7] + cheby[8]) - 
         0.00021 * Tu * sin(cheby[7]) +
         0.00004 * sin(cheby[7] + 2.0 * cheby[8]) - 
         0.00004 * cos(cheby[7]) -
         0.00004 * sin(cheby[5] - cheby[7]) + 
         0.00003 * Tu * sin(cheby[7] - cheby[8]);

    W = -0.04129 * sin(2.0 * cheby[7]) + 
         0.03211 * sin(cheby[8]) +
         0.00104 * sin(2.0 * cheby[7] - cheby[8]) -
         0.00035 * sin(2.0 * cheby[7] + cheby[8]) - 
         0.00010 -
         0.00008 * Tu * sin(cheby[8]) - 
         0.00008 * sin(cheby[5]) +
         0.00007 * sin(2.0 * cheby[8]) + 
         0.00005 * Tu * sin(2.0 * cheby[7]) +
         0.00003 * sin(cheby[1] - cheby[7]) -
         0.00002 * cos(cheby[8] - cheby[19]) +
         0.00002 * sin(4.0 * cheby[8] - 8.0 * cheby[16] + 3.0 * cheby[19]) -
         0.00002 * sin(cheby[8] - cheby[13]) -
         0.00002 * cos(2.0 * cheby[8] - 2.0 * cheby[13]);

    raa    = W / sqrt(U - V*V);
    sunRA  = cheby[7] + atan(raa  / sqrt(1.0 - raa*raa));
    sunRA  = reduce(sunRA,ZERO,TWOPI);

    deca   = V / sqrt(U);
    sunDec = atan(deca / sqrt(1.0 - deca*deca));
    sunDec = reduce(sunDec,-PI,PI);

    rP    *= EARTHSMA;

    sunPosGl[0] = rP * cos(sunRA) * cos(sunDec);
    sunPosGl[1] = rP * sin(sunRA) * cos(sunDec);
    sunPosGl[2] = rP * sin(sunDec);

    sunDist     = absol(sunPosGl);

    if (!moonFlag)
        return;

    /* Moon */

    rP    =  60.36298 
            - 3.27746 * cos(cheby[2]) 
            - 0.57994 * cos(cheby[2] - 2.0 * cheby[4]) 
            - 0.46357 * cos(2.0 * cheby[4]) 
            - 0.08904 * cos(2.0 * cheby[2]) 
            + 0.03865 * cos(2.0 * cheby[2] - 2.0 * cheby[4]) 
            - 0.03237 * cos(2.0 * cheby[4] - cheby[8]) 
            - 0.02688 * cos(cheby[2] + 2.0 * cheby[4]) 
            - 0.02358 * cos(cheby[2] - 2.0 * cheby[4] + cheby[8]) 
            - 0.02030 * cos(cheby[2] - cheby[8]) 
            + 0.01719 * cos(cheby[4]) 
            + 0.01671 * cos(cheby[2] + cheby[8]) 
            + 0.01247 * cos(cheby[2] - 2.0 * cheby[3]) 
            + 0.00704 * cos(cheby[8]) 
            + 0.00529 * cos(cheby[4] + cheby[8]) 
            - 0.00524 * cos(cheby[2] - 4.0 * cheby[4]) 
            + 0.00398 * cos(cheby[2] - 2.0 * cheby[4] - cheby[8]) 
            - 0.00366 * cos(3.0 * cheby[2])
            - 0.00295 * cos(2.0 * cheby[2] - 4.0 * cheby[4]) 
            - 0.00263 * cos(cheby[4] + cheby[8]) 
            + 0.00249 * cos(3.0 * cheby[2] - 2.0 * cheby[4]) 
            - 0.00221 * cos(cheby[2] + 2.0 * cheby[4] - cheby[8]) 
            + 0.00185 * cos(2.0 * cheby[3] - 2.0 * cheby[4]) 
            - 0.00161 * cos(2.0 * cheby[4] - 2.0 * cheby[8]) 
            + 0.00147 * cos(cheby[2] + 2.0 * cheby[3] - 2.0 * cheby[4]) 
            - 0.00142 * cos(4.0 * cheby[4]) 
            + 0.00139 * cos(2.0 * cheby[2] - 2.0 * cheby[4] + cheby[8]) 
            - 0.00118 * cos(cheby[2] - 4.0 * cheby[4] + cheby[8])
            - 0.00116 * cos(2.0 * cheby[2] + 2.0 * cheby[4]) 
            - 0.00110 * cos(2.0 * cheby[2] - cheby[8]);

    a[1]  =  0.39558 * sin(cheby[3] + cheby[5]);
    a[2]  =  0.08200 * sin(cheby[3]);
    a[3]  =  0.03257 * sin(cheby[2] - cheby[3] - cheby[5]);
    a[4]  =  0.01092 * sin(cheby[2] + cheby[3] + cheby[5]);
    a[5]  =  0.00666 * sin(cheby[2] - cheby[3]);
    a[6]  = -0.00644 * sin(cheby[2] + cheby[3] - 2.0 * cheby[4] + cheby[5]);
    a[7]  = -0.00331 * sin(cheby[3] - 2.0 * cheby[4] + cheby[5]);
    a[8]  = -0.00304 * sin(cheby[3] - 2.0 * cheby[4]);
    a[9]  = -0.00240 * sin(cheby[2] - cheby[3] - 2.0 * cheby[4] - cheby[5]);
    a[10] =  0.00226 * sin(cheby[2] + cheby[3]);
    a[11] = -0.00108 * sin(cheby[2] + cheby[3] - 2.0 * cheby[4]);
    a[12] = -0.00079 * sin(cheby[3] - cheby[5]);
    a[13] =  0.00078 * sin(cheby[3] + 2.0 * cheby[4] + cheby[5]);
    a[14] =  0.00066 * sin(cheby[3] + cheby[5] - cheby[8]);
    a[15] = -0.00062 * sin(cheby[3] + cheby[5] + cheby[8]);
    a[16] = -0.00050 * sin(cheby[2] - cheby[3] - 2.0 * cheby[4]);
    a[17] =  0.00045 * sin(2.0 * cheby[2] + cheby[3] + cheby[5]);
    a[18] = -0.00031 * sin(2.0 * cheby[2] + cheby[3] - 
                           2.0 * cheby[4] + cheby[5]);
    a[19] = -0.00027 * sin(cheby[2] + cheby[3] - 2.0 * cheby[4] + 
                           cheby[5] + cheby[8]);
    a[20] = -0.00024 * sin(cheby[3] - 2.0 * cheby[4] + cheby[5] + cheby[8])
            -0.00021 * Tu * sin(cheby[3] + cheby[5]);

    V = 0.0;

    for (i = 1; i <= 20; i++)
        V += a[i];

    U = 1.0 - 0.10828 * cos(cheby[2])
            - 0.01880 * cos(cheby[2] - 2.0 * cheby[4])
            - 0.01479 * cos(2.0 * cheby[4])
            + 0.00181 * cos(2.0 * cheby[2] - 2.0 * cheby[4])
            - 0.00147 * cos(2.0 * cheby[2])
            - 0.00105 * cos(2.0 * cheby[4] - cheby[8])
            - 0.00075 * cos(cheby[2] - 2.0 * cheby[4] + cheby[8])
            - 0.00067 * cos(cheby[2] - cheby[8])
            + 0.00057 * cos(cheby[4])
            + 0.00055 * cos(cheby[2] + cheby[8])
            - 0.00046 * cos(cheby[2] + 2.0 * cheby[4])
            + 0.00041 * cos(cheby[2] - 2.0 * cheby[3])
            + 0.00024 * cos(cheby[8]);

    a[1]  =  0.10478 * sin(cheby[2]);
    a[2]  = -0.04105 * sin(2.0 * cheby[3] + 2.0 * cheby[5]);
    a[3]  = -0.02130 * sin(cheby[2] - 2.0 * cheby[4]);
    a[4]  = -0.01779 * sin(2.0 * cheby[3] + cheby[5]);
    a[5]  =  0.01774 * sin(cheby[5]);
    a[6]  =  0.00987 * sin(2.0 * cheby[4]);
    a[7]  = -0.00338 * sin(cheby[2] - 2.0 * cheby[3] - 2.0 * cheby[5]);
    a[8]  = -0.00309 * sin(cheby[8]);
    a[9]  = -0.00190 * sin(2.0 * cheby[3]);
    a[10] = -0.00144 * sin(cheby[2] + cheby[5]);
    a[11] = -0.00144 * sin(cheby[2] - 2.0 * cheby[3] - cheby[5]);
    a[12] = -0.00113 * sin(cheby[2] + 2.0 * cheby[3] + 2.0 * cheby[5]);
    a[13] = -0.00094 * sin(cheby[2] - 2.0 * cheby[4] + cheby[8]);
    a[14] = -0.00092 * sin(2.0 * cheby[2] - 2.0 * cheby[4]);
    a[15] =  0.00071 * sin(2.0 * cheby[4] - cheby[8]);
    a[16] =  0.00070 * sin(2.0 * cheby[2]);
    a[17] =  0.00067 * sin(cheby[2]+2.0 * cheby[3] -
                           2.0 * cheby[4]+2.0 * cheby[5]);
    a[18] =  0.00066 * sin(2.0 * cheby[3] - 2.0 * cheby[4] + cheby[5]);
    a[19] = -0.00066 * sin(2.0 * cheby[4] + cheby[5]);
    a[20] =  0.00061 * sin(cheby[2] - cheby[8]);
    a[21] = -0.00058 * sin(cheby[4]);
    a[22] = -0.00049 * sin(cheby[2] + 2.0 * cheby[3] + cheby[5]);
    a[23] = -0.00049 * sin(cheby[2] - cheby[5]);
    a[24] = -0.00042 * sin(cheby[2] + cheby[8]);
    a[25] =  0.00034 * sin(2.0 * cheby[3] - 2.0 * cheby[4] + 2.0 * cheby[5]);
    a[26] = -0.00026 * sin(2.0 * cheby[3] - 2.0 * cheby[4]);
    a[27] =  0.00025 * sin(cheby[2] - 2.0 * (cheby[3] + cheby[4] + cheby[5]));
    a[28] =  0.00024 * sin(cheby[2] - 2.0 * cheby[3]);
    a[29] =  0.00023 * sin(cheby[2] + 2.0 * cheby[3] -
                                      2.0 * cheby[4] + cheby[5]);
    a[30] =  0.00023 * sin(cheby[2] - 2.0 * cheby[4] - cheby[5]);

    W = 0.0;

    for (i = 1; i <= 30; i++)
        W += a[i];

    raa     = W / sqrt(U - V*V);
    moonRA  = cheby[1] + atan(raa  / sqrt(1.0 - raa*raa));
    moonRA  = reduce(moonRA,ZERO,TWOPI);

    deca    = V / sqrt(U);
    moonDec = atan(deca / sqrt(1.0 - deca*deca));
    moonDec = reduce(moonDec,-PI,PI);

    rP     *= EARTHRADIUS;

    moonPosGl[0] = rP * cos(moonRA) * cos(moonDec);
    moonPosGl[1] = rP * sin(moonRA) * cos(moonDec);
    moonPosGl[2] = rP * sin(moonDec);

    moonDist     = absol(moonPosGl);

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satheli.c                                            */
/*                                                                            */
/******************************************************************************/
