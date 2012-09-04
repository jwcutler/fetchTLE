/******************************************************************************/
/*                                                                            */
/*  Title       : satrefr.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 01Jan92                                                     */
/*  Last change : 01Jan95                                                     */
/*                                                                            */
/*  Synopsis    : This function calculates the correction to elevation due    */
/*                to atmospheric refraction for the wavelength region between */
/*                200 and 20000 nm.                                           */
/*                                                                            */
/*                                                                            */
/*  Input parameters :                                                        */
/*                                                                            */
/*   1.  standard atmospheric pressure      [mmHg]   :  atmPress              */
/*   2.  standard ambient temperature       [deg C]  :  ambTemp               */
/*   3.  relative humidity                  [%]      :  relHumid              */
/*   4.  wavelength (in vacuum)             [micron] :  waveLength            */
/*   5.  reference wavelength (in vacuum)   [micron] :  refWaveLength         */
/*   5.  Earth's effective radius           [km]     :  effEarthRad           */
/*   6.  scale height of the atmosphere     [km]     :  scaleHeight           */
/*   7.  altitude of the observer           [km]     :  obsAlt                */
/*   8.  elevation of the satellite         [rad]    :  satEle                */
/*                                                                            */
/*                                                                            */
/*  Output parameters :                                                       */
/*                                                                            */
/*   1.  index of refraction                [---]    :  refIndex              */
/*   2.  correction to elevation            [rad]    :  refrEle               */
/*   3.  differential elevation correction  [rad]    :  diffEle               */
/*   4.  air mass                           [---]    :  airMass               */
/*                                                                            */
/*                                                                            */
/*  For reference see :   C.W.Allen, "Astrophysical Quantities",              */
/*                        The Athlone Press, London (1976), pages 122, 124    */
/*                                                                            */
/*                        P.N.Tverskoi, "Physics of the Atmosphere, A Course  */
/*                        in Meteorology", E.S.Selezneva (ed.), GIMIZ,        */
/*                        Leningrad (1962)                                    */
/*                        (for conversion of relative humidity into water     */
/*                        vapor pressure)                                     */
/*                                                                            */
/*                                                                            */
/*  Original papers : a)  H.Barrel, J. Opt. Soc. Am. 41, 295-299 (1951)       */
/*                    b)  B.Edlen,  J. Opt. Soc. Am. 43, 339-344 (1953)       */
/*                    c)  C.D.Coleman, W.R.Bozman, W.F.Meggers, "Table of     */
/*                        Wavenumbers", NBS Monograph 3, Washington D.C.      */
/*                        (1960)                                              */
/*                                                                            */
/*  All equations were taken from the original papers :                       */
/*                                                                            */
/*  assumed standard temperature and pressure: +15 deg C, 760 mmHg            */
/*                                                                            */
/*  n1    = (n(15,760)-1)*1.0e-6          from eqn. (2) in b)                 */
/*  wv    = correction for humid air      from eqn. (5) in b)                 */
/*  ntpf1 = (n(t,p,f) -1)*1.0e-6          from eqn. (9) in a)                 */
/*                                                                            */
/*  reference (c) has been used for cross-checking the formulae and the data  */
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
/* getRefraction: calculates the refraction in the atmosphere                 */
/*                                                                            */
/******************************************************************************/

void getRefraction(atmPress,ambTemp,relHumid,waveLength,refWaveLength,
                   effEarthRad,scaleHeight,obsAlt,satEle,
                   prefIndex,prefrEle,pdiffEle,pairMass)

double atmPress, ambTemp, relHumid, waveLength, refWaveLength, effEarthRad; 
double scaleHeight, obsAlt, satEle, *prefIndex, *prefrEle, *pdiffEle, *pairMass;

{
    double ntpf1, ntpf1r, n1, n1r, ntpf, ntpfr, mr, mw, nr, tp, wv, wvr;
    double waterVapor, absTemp, log10E, E;
    double refrEle, diffEle, airMass, hAtm, sinEle, arg;
    double rSin, rSin2, rObs, rObs2, rAtm, rAtm2, a, alpha, sinAlp, beta, betar;

/******************************************************************************/
/*                                                                            */
/* return zero if satellite is below the horizon                              */
/*                                                                            */
/******************************************************************************/

    if (satEle < 0.0)
    {
        *prefIndex = 0.0;
        *prefrEle  = 0.0;
        *pdiffEle  = 0.0;
        *pairMass  = 0.0;

        return;
    }

/******************************************************************************/
/*                                                                            */
/* convert relative humidity [%] into water vapor pressure [mmHg]             */
/*                                                                            */
/* scale factor : 1/133.322 = 1/100/1.33322                                   */
/* (conversion from "per cent" into decimal number and from mbar into mmHg)   */
/*                                                                            */
/* for reference see :   P.N.Tverskoi, "Physics of the Atmosphere, A Course   */
/*                       in Meteorology", E.S.Selezneva (ed.), GIMIZ,         */
/*                       Leningrad (1962)                                     */
/*                                                                            */
/******************************************************************************/

    absTemp    = 273.155 + ambTemp;
    log10E     = 23.5518 - 2937.4 / absTemp - 4.9283 * log10(absTemp); 
    E          = pow(10.0,log10E);
    waterVapor = E * relHumid / 133.322;

/******************************************************************************/
/*                                                                            */
/* rescale length units from km into m                                        */
/*                                                                            */
/******************************************************************************/

    effEarthRad *= CKMM;
    scaleHeight *= CKMM;
    obsAlt      *= CKMM;

/******************************************************************************/
/*                                                                            */
/* first calculate (n-1) for standard temperature (t = +15 deg C) and         */
/* standard pressure (p = 760 mmHg) in dry air for both the wavelength and    */
/* the reference wavelength                                                   */
/*                                                                            */
/* waveLength and refWaveLength are the two wavelengths in vacuum             */
/*                                                                            */
/******************************************************************************/

    mw  = 1.0 / (waveLength * waveLength);
    mr  = 1.0 / (refWaveLength * refWaveLength);

    n1  = 64.328 + 29498.1 / (146.0 - mw) + 255.4 / (41.0 - mw);
    n1r = 64.328 + 29498.1 / (146.0 - mr) + 255.4 / (41.0 - mr);

/******************************************************************************/
/*                                                                            */
/* now calculate the contribution due to water vapor in the air for both      */
/* wavelengths                                                                */
/*                                                                            */
/******************************************************************************/

    tp  = 1.0 + 0.003661 * ambTemp;

    wv  = -(0.0624 - 0.000680 * mw) * waterVapor / tp;
    wvr = -(0.0624 - 0.000680 * mr) * waterVapor / tp;

/******************************************************************************/
/*                                                                            */
/* calculate pressure at the specified altitude                               */
/*                                                                            */
/* then calculate (n-1) for pressure (atmPress) and temperature (ambTemp)     */
/*                                                                            */
/* explanation of the numerical factor :                                      */
/*                                                                            */
/* 1.387188936e-3 = (1 + 15*0.003661)/(760*(1 + 760*0.813e-6))                */
/*                                                                            */
/******************************************************************************/

    nr     = (1.0 + (1.049 - 0.0157 * ambTemp) * 1.0e-6 * atmPress) / tp;
    nr    *= 1.387188936e-3;

    ntpf1  = n1  * atmPress * nr + wv;
    ntpf1r = n1r * atmPress * nr + wvr;

    ntpf   = 1.0 + ntpf1  * 1.0e-6;
    ntpfr  = 1.0 + ntpf1r * 1.0e-6;

/******************************************************************************/
/*                                                                            */
/* calculate the angle of normal incidence into the atmosphere (alpha)        */
/* and then the correction to elevation (refralt) by applying Snell's law:    */
/*                                                                            */
/* n2/n1 = ntpf = sin(alpha)/sin(beta)  and  refrEle = alpha - beta           */
/*                                                                            */
/* if the elevation is still or becomes less than 0.0 deg the correction      */
/* (refrEle) is set to zero                                                   */
/*                                                                            */
/******************************************************************************/

    rObs    = effEarthRad + obsAlt;
    rAtm    = effEarthRad + scaleHeight;
    hAtm    = scaleHeight - obsAlt;

    sinEle  = sin(satEle);
    rSin    = rObs * sinEle;
    rSin2   = rSin * rSin;
    rObs2   = rObs * rObs;
    rAtm2   = rAtm * rAtm;

    a       = -rSin + sqrt (rSin2 + rAtm2 - rObs2);
    airMass = a / hAtm;

    alpha   = acos((a*a + rAtm2 - rObs2) / (2.0*a*rAtm));
    sinAlp  = sin(alpha);

    arg     = sinAlp / ntpf;

    if (arg >  1.0) arg =  1.0;      /* protect against out-of-range problems */
    if (arg < -1.0) arg = -1.0;

    beta    = asin(arg);
    arg     = sinAlp / ntpfr;

    if (arg >  1.0) arg =  1.0;
    if (arg < -1.0) arg = -1.0;

    betar   = asin(arg);

    refrEle = alpha - beta;
    diffEle = beta  - betar;

/******************************************************************************/
/*                                                                            */
/* correction for refraction in the atmosphere is now done with respect to:   */
/*                                                                            */
/* 1. wavelength of observation                                               */
/* 2. ambient temperature                                                     */
/* 3. atmospheric pressure                                                    */
/* 4. water vapor pressure                                                    */
/* 5. altitude (zenith distance)                                              */
/* 6. curvature of the atmosphere                                             */
/*                                                                            */
/******************************************************************************/

/*
    printf("\n");
    printf("wavelength              [micron] : %9.6f\n",waveLength);
    printf("reference wavelength    [micron] : %9.6f\n",refWaveLength);
    printf("atmospheric pressure    [mmHg]   : %9.3f\n",atmPress);
    printf("ambient temperature     [deg C]  : %9.3f\n",ambTemp);
    printf("water vapor pressure    [mmHg]   : %9.3f\n",waterVapor);
    printf("refractive index        (n-1)e+6 : %9.3f\n",ntpf1);
    printf("alpha                   [deg]    : %9.6f\n",alpha*CRD);
    printf("beta                    [deg]    : %9.6f\n",beta*CRD);
    printf("betar                   [deg]    : %9.6f\n",betar*CRD);
    printf("refractive correction   [arcsec] : %9.4f\n",refrEle*CRAS);
    printf("differential correction [arcsec] : %9.4f\n",diffEle*CRAS);
    printf("air mass                [---]    : %9.3f\n",airMass);
*/

    *prefIndex = ntpf; 
    *prefrEle  = refrEle;
    *pdiffEle  = diffEle;
    *pairMass  = airMass;

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satrefr.c                                            */
/*                                                                            */
/******************************************************************************/
