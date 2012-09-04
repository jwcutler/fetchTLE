/******************************************************************************/
/*                                                                            */
/*  Title       : satnute.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 17Aug94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This function block calculates the nutation effects.        */
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
/* getNutation: calculates nutation in longitude and obliquity, and the       */
/*              equation of the equinoxes                                     */
/*                                                                            */
/******************************************************************************/

void getNutation()

{
    double tu, tusq, tucb, eps, trueEps, l, lp, f, d, o;

/******************************************************************************/
/*                                                                            */
/*  check if nutation quantities need to be updated                           */
/*                                                                            */
/******************************************************************************/

    if (fabs(julianDate - lastJulianDateNute) < NUTEUPDATEINT)
        return;

    lastJulianDateNute = julianDate;

    tu   = (julianDate - JULDAT2000) / JULCENT;
    tusq = tu*tu;
    tucb = tu*tusq;

/******************************************************************************/
/*                                                                            */
/*  now calculate the mean obliquity of the ecliptic (eps), based on the      */
/*  epoch J2000.0                                                             */
/*                                                                            */
/*  for reference see the "Astronomical Almanac", 1986, pages B24 - B31       */
/*                                                                            */
/*  the equation for calculation of eps has been taken from the               */
/*  "Astronomical Almanac", 1984, page S21 (in the Supplement section)        */
/*                                                                            */
/******************************************************************************/

    eps = (84381.448 - 46.815*tu - 0.00059*tusq + 0.001813*tucb) * CASR;

/******************************************************************************/
/*                                                                            */
/*  now compute the nutation in longitude (totPsi) and in obliquity (totEps)  */
/*  thus giving the true obliquity of the ecliptic (trueEps) and from that    */
/*  the equation of the equinoxes, also based on the epoch J2000.0            */
/*                                                                            */
/*  calculation of the nutation parameters totPsi and totEps is performed     */
/*  by using the "1980 IAU Theory of Nutation"                                */
/*  (see "Astronomical Almanac", 1984, pages S21-S26)                         */
/*                                                                            */
/*  fundamental arguments of the Sun and the Moon are : l, lp, f, d, o        */
/*  (time-dependent arguments)                                                */
/*  (from the "Astronomical Almanac", 1984, page S26)                         */
/*  the function reduce() reduces their values into the interval (0, 2pi)     */
/*                                                                            */
/*  the nutation in longitude (totPsi) and in obliquity (totEps) are updated  */
/*  when the update time interval is elapsed                                  */
/*                                                                            */
/******************************************************************************/

    l   = ( 485866.733 + 1717915922.633*tu +31.310*tusq + 0.064*tucb) * CASR;
    l   = reduce(l,ZERO,TWOPI);

    lp  = (1287099.804 +  129596581.224*tu - 0.577*tusq - 0.012*tucb) * CASR;
    lp  = reduce(lp,ZERO,TWOPI);

    f   = ( 335778.877 + 1739527263.137*tu -13.257*tusq + 0.011*tucb) * CASR;
    f   = reduce(f,ZERO,TWOPI);

    d   = (1072261.307 + 1602961601.328*tu - 6.891*tusq + 0.019*tucb) * CASR;
    d   = reduce(d,ZERO,TWOPI);

    o   = ( 450160.280 -    6962890.539*tu + 7.455*tusq + 0.008*tucb) * CASR;
    o   = reduce(o,ZERO,TWOPI);

    getNutationSeries(tu,l,lp,f,d,o);

    trueEps    = eps + totEps;                                       /* [rad] */
    equEquinox = totPsi * cos(trueEps);                              /* [rad] */

    return; 
}

/******************************************************************************/
/*                                                                            */
/* getNutationSeries: calculates sine and cosine series of nutation           */
/*                                                                            */
/******************************************************************************/

void getNutationSeries(tu,l,lp,f,d,o)

double tu, l, lp, f, d, o;

{
    double b[106], sinb[106], c[106], cosc[106], bc[106];

    int n;

/******************************************************************************/
/*                                                                            */
/* coefficients and arguments of the sine and cosine terms for the Fourier    */
/* series representation (106 terms) of the nutation in longitude (sine)      */
/* and of the nutation in obliquity (cosine), respectively                    */
/* (all known long and short-term variations are included)                    */
/*                                                                            */
/*                                                                            */
/* totEps    = b(0)*cos(B(0)) + b(1)*cos(B(1)) + ... + b(105)*cos(B(105))     */
/*                                                                            */
/* with b(n) = aat(n,0) + aat(n,1)*tu  for n = 0,...,14   (time-dependent)    */
/* and  B(n) = at(n,0)*l + at(n,1)*lp + at(n,2)*f + at(n,3)*d + at(n,4)*o     */
/*                                                                            */
/* with b(n) = aa(n,0)                 for n = 15,...,105 (time-independent)  */
/* and  B(n) = a(n,0)*l + a(n,1)*lp + a(n,2)*f + a(n,3)*d + a(n,4)*o          */
/*                                                                            */
/*                                                                            */
/* totPsi    = c(0)*sin(C(0)) + c(1)*sin(C(1)) + ... + c(105)*sin(C(105))     */
/*                                                                            */
/* with c(n) = aat(n,1) + aat(n,2)*tu  for n = 0,...,14   (time-dependent)    */
/* and  c(n) = aa(n,1)                 for n = 15,...,105 (time-independent)  */
/*                                                                            */
/* and  C(n) = B(n)                                                           */
/*                                                                            */
/*                                                                            */
/* first :  15 terms with time-dependent arguments                            */
/*          (either nutation in longitude or in obliquity is time-dependent)  */
/*                                                                            */
/*          the data field at[][] contains the code for the linear combina-   */
/*          tions of the arguments of Sun and Moon (l, lp, f, d, o)           */
/*                                                                            */
/******************************************************************************/

    static double at[15][5] = {
         {  0,  0,  0,  0,  1},
         {  0,  0,  0,  0,  2},
         {  0,  0,  2, -2,  2},
         {  0,  1,  0,  0,  0},
         {  0,  1,  2, -2,  2},
         {  0, -1,  2, -2,  2},
         {  0,  0,  2, -2,  1},
         {  0,  2,  0,  0,  0},
         {  0,  2,  2, -2,  2},
         {  0,  0,  2,  0,  2},
         {  1,  0,  0,  0,  0},
         {  0,  0,  2,  0,  1},
         {  1,  0,  2,  0,  2},
         {  1,  0,  0,  0,  1},
         { -1,  0,  0,  0,  1}
    };

    static double aat[15][4] = {
         { -171996,  -174.2,   92025,     8.9},
         {    2062,     0.2,    -895,     0.5},
         {  -13187,    -1.6,    5736,    -3.1},
         {    1426,    -3.4,      54,    -0.1},
         {    -517,     1.2,     224,    -0.6},
         {     217,    -0.5,     -95,     0.3},
         {     129,     0.1,     -70,     0.0},
         {      17,    -0.1,       0,     0.0},
         {     -16,     0.1,       7,     0.0},
         {   -2274,    -0.2,     977,    -0.5},
         {     712,     0.1,      -7,     0.0},
         {    -386,    -0.4,     200,     0.0},
         {    -301,     0.0,     129,    -0.1},
         {      63,     0.1,     -33,     0.0},
         {     -58,    -0.1,      32,     0.0}
    };

/******************************************************************************/
/*                                                                            */
/* second : 91 terms with non-time dependent arguments                        */
/*                                                                            */
/******************************************************************************/

    static double a[91][5] = {
         { -2,  0,  2,  0,  1},
         {  2,  0, -2,  0,  0},
         { -2,  0,  2,  0,  2},
         {  1, -1,  0, -1,  0},
         {  0, -2,  2, -2,  1},
         {  2,  0, -2,  0,  1},
         {  2,  0,  0, -2,  0},
         {  0,  0,  2, -2,  0},
         {  0,  1,  0,  0,  1},
         {  0, -1,  0,  0,  1},
         { -2,  0,  0,  2,  1},
         {  0, -1,  2, -2,  1},
         {  2,  0,  0, -2,  1},
         {  0,  1,  2, -2,  1},
         {  1,  0,  0, -1,  0},
         {  2,  1,  0, -2,  0},
         {  0,  0, -2,  2,  1},
         {  0,  1, -2,  2,  0},
         {  0,  1,  0,  0,  2},
         { -1,  0,  0,  1,  1},
         {  0,  1,  2, -2,  0},
         {  1,  0,  0, -2,  0},
         { -1,  0,  2,  0,  2},
         {  0,  0,  0,  2,  0},
         { -1,  0,  2,  2,  2},
         {  1,  0,  2,  0,  1},
         {  0,  0,  2,  2,  2},
         {  2,  0,  0,  0,  0},
         {  1,  0,  2, -2,  2},
         {  2,  0,  2,  0,  2},
         {  0,  0,  2,  0,  0},
         { -1,  0,  2,  0,  1},
         { -1,  0,  0,  2,  1},
         {  1,  0,  0, -2,  1},
         { -1,  0,  2,  2,  1},
         {  1,  1,  0, -2,  0},
         {  0,  1,  2,  0,  2},
         {  0, -1,  2,  0,  2},
         {  1,  0,  2,  2,  2},
         {  1,  0,  0,  2,  0},
         {  2,  0,  2, -2,  2},
         {  0,  0,  0,  2,  1},
         {  0,  0,  2,  2,  1},
         {  1,  0,  2, -2,  1},
         {  0,  0,  0, -2,  1},
         {  1, -1,  0,  0,  0},
         {  2,  0,  2,  0,  1},
         {  0,  1,  0, -2,  0},
         {  1,  0, -2,  0,  0},
         {  0,  0,  0,  1,  0},
         {  1,  1,  0,  0,  0},
         {  1,  0,  2,  0,  0},
         {  1, -1,  2,  0,  2},
         { -1, -1,  2,  2,  2},
         { -2,  0,  0,  0,  1},
         {  3,  0,  2,  0,  2},
         {  0, -1,  2,  2,  2},
         {  1,  1,  2,  0,  2},
         { -1,  0,  2, -2,  1},
         {  2,  0,  0,  0,  1},
         {  1,  0,  0,  0,  2},
         {  3,  0,  0,  0,  0},
         {  0,  0,  2,  1,  2},
         { -1,  0,  0,  0,  2},
         {  1,  0,  0, -4,  0},
         { -2,  0,  2,  2,  2},
         { -1,  0,  2,  4,  2},
         {  2,  0,  0, -4,  0},
         {  1,  1,  2, -2,  2},
         {  1,  0,  2,  2,  1},
         { -2,  0,  2,  4,  2},
         { -1,  0,  4,  0,  2},
         {  1, -1,  0, -2,  0},
         {  2,  0,  2, -2,  1},
         {  2,  0,  2,  2,  2},
         {  1,  0,  0,  2,  1},
         {  0,  0,  4, -2,  2},
         {  3,  0,  2, -2,  2},
         {  1,  0,  2, -2,  0},
         {  0,  1,  2,  0,  1},
         { -1, -1,  0,  2,  1},
         {  0,  0, -2,  0,  1},
         {  0,  0,  2, -1,  2},
         {  0,  1,  0,  2,  0},
         {  1,  0, -2, -2,  0},
         {  0, -1,  2,  0,  1},
         {  1,  1,  0, -2,  1},
         {  1,  0, -2,  2,  0},
         {  2,  0,  0,  2,  0},
         {  0,  0,  2,  4,  2},
         {  0,  1,  0,  1,  0}
    };

    static double aa[91][2] = {
         {   46,   -24},
         {   11,     0},
         {   -3,     1},
         {   -3,     0},
         {   -2,     1},
         {    1,     0},
         {   48,     1},
         {  -22,     0},
         {  -15,     9},
         {  -12,     6},
         {   -6,     3},
         {   -5,     3},
         {    4,    -2},
         {    4,    -2},
         {   -4,     0},
         {    1,     0},
         {    1,     0},
         {   -1,     0},
         {    1,     0},
         {    1,     0},
         {   -1,     0},
         { -158,    -1},
         {  123,   -53},
         {   63,    -2},
         {  -59,    26},
         {  -51,    27},
         {  -38,    16},
         {   29,    -1},
         {   29,   -12},
         {  -31,    13},
         {   26,    -1},
         {   21,   -10},
         {   16,    -8},
         {  -13,     7},
         {  -10,     5},
         {   -7,     0},
         {    7,    -3},
         {   -7,     3},
         {   -8,     3},
         {    6,     0},
         {    6,    -3},
         {   -6,     3},
         {   -7,     3},
         {    6,    -3},
         {   -5,     3},
         {    5,     0},
         {   -5,     3},
         {   -4,     0},
         {    4,     0},
         {   -4,     0},
         {   -3,     0},
         {    3,     0},
         {   -3,     1},
         {   -3,     1},
         {   -2,     1},
         {   -3,     1},
         {   -3,     1},
         {    2,    -1},
         {   -2,     1},
         {    2,    -1},
         {   -2,     1},
         {    2,     0},
         {    2,    -1},
         {    1,    -1},
         {   -1,     0},
         {    1,    -1},
         {   -2,     1},
         {   -1,     0},
         {    1,    -1},
         {   -1,     1},
         {   -1,     1},
         {    1,     0},
         {    1,     0},
         {    1,    -1},
         {   -1,     0},
         {   -1,     0},
         {    1,     0},
         {    1,     0},
         {   -1,     0},
         {    1,     0},
         {    1,     0},
         {   -1,     0},
         {   -1,     0},
         {   -1,     0},
         {   -1,     0},
         {   -1,     0},
         {   -1,     0},
         {   -1,     0},
         {    1,     0},
         {   -1,     0},
         {    1,     0}
    };

/******************************************************************************/
/*                                                                            */
/* calculate the linear combinations of the arguments of Sun and Moon         */
/*                                                                            */
/* B(n) = C(n) = bc(n) = ....                                                 */
/*                                                                            */
/******************************************************************************/

    for (n = 0; n <= 14; n++)
    {
        bc[n] = at[n][0]*l + at[n][1]*lp + at[n][2]*f + at[n][3]*d + at[n][4]*o;
        bc[n] = reduce(bc[n],ZERO,TWOPI);
    }

    for (n = 15; n <= 105; n++)
    {
        bc[n] = a[n-15][0]*l + a[n-15][1]*lp + a[n-15][2]*f
                             + a[n-15][3]*d  + a[n-15][4]*o;
        bc[n] = reduce(bc[n],ZERO,TWOPI);
    }

/******************************************************************************/
/*                                                                            */
/* calculate the nutation in longitude (totPsi) (sine series) and             */
/* the nutation in obliquity (totEps) (cosine series)                         */
/*                                                                            */
/******************************************************************************/

    for (n = 0; n <= 14; n++)
    {
        b[n]    = aat[n][0] + aat[n][1]*tu;
        c[n]    = aat[n][2] + aat[n][3]*tu;
    }

    for (n = 15; n <= 105; n++)
    {
        b[n]    = aa[n-15][0];
        c[n]    = aa[n-15][1];
    }

    for (n = 0; n <= 105; n++)
    {
        sinb[n] = sin (bc[n]);
        cosc[n] = cos (bc[n]);
    }

    totPsi = 0.0;
    totEps = 0.0;

    for (n = 0; n <= 105; n++)
    {
        totPsi += b[n]*sinb[n];
        totEps += c[n]*cosc[n];
    }

    totPsi *= CASR / 10000.0;                                        /* [rad] */
    totEps *= CASR / 10000.0;                                        /* [rad] */

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satnute.c                                            */
/*                                                                            */
/******************************************************************************/
