/******************************************************************************/
/*                                                                            */
/*  Title       : satprop.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 20Nov93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : SGP4/SDP4 propagation routines (translated from FORTRAN)    */
/*                for reference see:                                          */
/*                "Spacetrack Report No. 3, Models for Propagation of         */
/*                NORAD Element Sets", F. R. Hoots and R. L. Roehrich,        */
/*                December 1980                                               */
/*                                                                            */
/*  REMARK: The SDP4 model implementation has not been completed in this      */
/*          release of SatTrack. The full implementation of the SDP4 model    */
/*          for deep space objects is expected to be part of a future         */
/*          release. For now, a simplified model is used for deep space       */
/*          objects (the so-called 'TLE Mean model').                         */
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
#include "satglobalspx.h"
#include "sattrack.h"
#include "satprop.h"

/******************************************************************************/
/*                                                                            */
/* checkPropModelType: checks type of propagation model used                  */
/*                                                                            */
/******************************************************************************/

void checkPropModelType()

{
    getSemiMajorAxis();
    checkPerigeeHeight();

    if (deepSpaceFlag)
    {
        sprintf(propModel,"%s",TLEMEANMODEL);                 /* for now only */
        propModelType = TLEMEAN;

/*
        sprintf(propModel,"%s",DEEPSPACEMODEL);
        propModelType = DEEPSPACE;
*/
    }

    else
    {
        sprintf(propModel,"%s",LOWEARTHMODEL);
        propModelType = LOWEARTH;
    }

    /* 'modelFlag' is TRUE if main() is invoked with the '-m' option */
    /* this overwrites the usage of either the SGP4 or SDP4 models   */

    if (modelFlag)  
    {
        sprintf(propModel,"%s",TLEMEANMODEL);
        propModelType = TLEMEAN;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* changeToNoradUnits: changes units of fundamental elements for use with     */
/*                     NORAD model                                            */
/*                                                                            */
/******************************************************************************/

void changeToNoradUnits()

{
    epoch0     = epochDay;                                   /* [d]           */

    ecc0       = eccentricity;                               /* [---]         */
    inc0       = inclination;                                /* [rad]         */
    meanMot0   = epochMeanMotion * TWOPI / MPD;              /* [rad/min]     */
    meanAn0    = epochMeanAnomaly;                           /* [rad]         */
    argPer0    = epochArgPerigee;                            /* [rad]         */
    raan0      = epochRaan;                                  /* [rad]         */

    decRate    = decayRate * TWOPI / MPD2;                   /* [rad/min^2]   */
    decRateDot = decayRateDot * TWOPI / MPD3;                /* [rad/min^3]   */

    bStar      = bStarCoeff / AE;

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSemiMajorAxis: recover original mean motion (meanMotDeep) and semimajor */
/*                   axis (sma0Deep) from input elements                      */
/*                                                                            */
/******************************************************************************/

void getSemiMajorAxis()

{
    changeToNoradUnits();

    cosInc        = cos(inc0);
    sinInc        = sin(inc0);

    theta         = cosInc;
    theta2        = SQR(theta);
    theta4        = SQR(theta2);

    x1mth2        =  1.0 - theta2;
    x3th2m1       =  3.0 * theta2 - 1.0;
    x5th2m1       =  5.0 * theta2 - 1.0;
    x7th2m1       =  7.0 * theta2 - 1.0;
    x7th2m3       =  7.0 * theta2 - 3.0;
    x19th2m4      = 19.0 * theta2 - 4.0;

    eccSQ         = SQR(ecc0);
    eccCB         = ecc0 * eccSQ;

    beta02        = 1.0 - eccSQ;                          /* needed elsewhere */
    beta0         = sqrt(beta02);                         /* needed elsewhere */
    beta04        = SQR(beta02);                          /* needed elsewhere */

    sma1          = pow((KE/meanMot0),TWOTHIRDS);

    deltaFact     = 1.5 * CK2 * x3th2m1 / pow(beta02,THREEHALFS);
    delta1        = deltaFact / SQR(sma1);
    delta12       = SQR(delta1);
    delta13       = delta1 * delta12;

    sma0          = sma1 * (1.0 - ONETHIRD * delta1 - delta12 
                                - 134.0/81.0 * delta13);

    delta0        = deltaFact / SQR(sma0);
    meanMotDeep   = meanMot0 / (1.0 + delta0);
    sma0Deep      = sma0 / (1.0 - delta0);
    semiMajorAxis = sma0Deep * EARTHRADIUS;

    deepSpaceFlag = (TWOPI / meanMotDeep >= 225.0) ? TRUE : FALSE;

    return;
}

/******************************************************************************/
/*                                                                            */
/* getTimeArgs: gets time arguments for the SGP4/SDP4 model calculations      */
/*                                                                            */
/******************************************************************************/

void getTimeArgs(tmArg)

double tmArg;

{
    tFP = (tmArg - epoch0) * MPD;                                    /* [min] */
    tSQ = tFP * tFP;
    tCB = tSQ * tFP;
    tQD = tCB * tFP;
    tQN = tQD * tFP;

    return;
}

/******************************************************************************/
/*                                                                            */
/* checkPerigeeHeight: checks perigee height and makes modifications          */
/*                     accordingly                                            */
/*                                                                            */
/* For a perigee below 156 km, the values of S and Q0MS0T4 are altered.       */
/*                                                                            */
/******************************************************************************/

void checkPerigeeHeight()

{
    double perigeeTest;

    perigeeTest   = (sma0Deep * (1.0 - ecc0)/AE - AE) * EARTHRADIUS;
    perigeeHeight = (sma0Deep * (1.0 - ecc0)    - AE) * EARTHRADIUS;
    apogeeHeight  = (sma0Deep * (1.0 + ecc0)    - AE) * EARTHRADIUS;

    truncFlag     = (perigeeTest   < 220.0) ? TRUE : FALSE;
    satCrashFlag  = (perigeeHeight <   0.0) ? TRUE : FALSE;

    sStar = S;
    qmst4 = Q0MS0T4;

    if (perigeeHeight < 156.0)
    {
        sStar = perigeeHeight - S0;

        if (perigeeHeight <= 98.0)
            sStar = 20.0;
    
        qmst4 = pow(((Q0 - sStar) * AE / EARTHRADIUS),4.0);
        sStar = sStar / EARTHRADIUS + AE;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* calcCommonPerturb: calculates quantities that are common to the SGP4 and   */
/*                    SDP4 models                                             */
/*                                                                            */
/******************************************************************************/

void calcCommonPerturb()

{
    pInvSQ = 1.0 / (SQR(sma0Deep) * beta04);
    xi     = 1.0 / (sma0Deep - sStar);
    eta    = sma0Deep * ecc0 * xi;
    etaSQ  = SQR(eta);
    eccEta = ecc0 * eta;
    psiSQ  = fabs(1.0 - etaSQ);
    coef0  = qmst4 * pow(xi,4.0);
    coef1  = coef0 / pow(psiSQ,3.5);

    c2     = coef1 * meanMotDeep * 
             (sma0Deep * (1.0 + 1.5 * etaSQ + eccEta * (4.0 + etaSQ)) + 
             0.75 * CK2 * xi / psiSQ * x3th2m1 * 
             (8.0 + 3.0 * etaSQ * (8.0 + etaSQ)));

    c1     = bStar * c2;

    c3     = coef0 * xi * A3OCK2 * meanMotDeep * AE * sinInc / ecc0;

    c4     = 2.0 * meanMotDeep * coef1 * sma0Deep * beta02 * 
             ((2.0 * eta * (1.0 + eccEta) + 0.5 * ecc0 + 0.5 * eta * etaSQ) - 
             2.0 * CK2 * xi / (sma0Deep * psiSQ) * 
             (-3.0 * x3th2m1 * (1.0 + 1.5 * etaSQ - 2.0 * eccEta - 
             0.5 * eccEta * etaSQ) + 0.75 * x1mth2 * 
             (2.0 * etaSQ - eccEta - eccEta * etaSQ) * cos(2.0 * argPer0)));

    c5     = 2.0 * coef1 * sma0Deep * beta02 * 
             (1.0 + 2.75 * (etaSQ + eccEta) + eccEta * etaSQ);

    clc1   = 3.0  * CK2 * pInvSQ * meanMotDeep;
    clc2   = clc1 * CK2 * pInvSQ;
    clc3   = 1.25 * CK4 * SQR(pInvSQ) * meanMotDeep;

    c1SQ   = SQR(c1);
    d2     = 4.0 * sma0Deep * xi * c1SQ;
    clc0   = d2 * xi * c1 / 3.0;
    d3     = (17.0 * sma0Deep + sStar) * clc0;
    d4     = 0.5 * clc0 * sma0Deep * xi * 
             (221.0 * sma0Deep + 31.0 * sStar) * c1;

    t2Cof  = 1.5 * c1;
    t3Cof  = d2 + 2.0 * c1SQ;
    t4Cof  = 0.25 * (3.0 * d3 + c1 * (12.0 * d2 + 10.0 * c1SQ));
    t5Cof  = 0.2 * (3.0 * d4 + 12.0 * c1 * d3 + 
             6.0 * SQR(d2) + 15.0 * c1SQ * (2.0 * d2 + c1SQ));

    xLCof  = 0.125 * A3OCK2 * sinInc * (3.0 + 5.0 * theta) / (1.0 + theta);
    ayCof  = 0.250 * A3OCK2 * sinInc;

    delMeanAn0 = pow(1.0 + eta * cos(meanAn0),3.0);

    meanAnDot  = meanMotDeep + 0.5 * clc1 * beta0 * x3th2m1 + 
                 0.0625 * clc2 * beta0 * 
                 (13.0 - 78.0 * theta2 + 137.0 * theta4);

    argPerDot  = 0.5 * clc1 * x5th2m1 + 0.0625 * clc2 * 
                 (7.0 - 114.0 * theta2 + 395.0 * theta4) + 
                 clc3 * (3.0 - 36.0 * theta2 + 49.0 * theta4);

    raanDot    = (-clc1 - 0.5 * clc2 * x19th2m4 - 2.0 * clc3 * x7th2m3) * theta;

    meanAnCof  = -TWOTHIRDS * coef0 * bStar * AE / eccEta;
    argPerCof  = bStar * c3 * cos(argPer0);
    raanCof    = -3.5 * beta02 * clc1 * theta * c1;

    return;
}

/******************************************************************************/
/*                                                                            */
/* updateGravityAndDrag: updates quantities for secular gravity and           */
/*                       atmospheric drag                                     */
/*                                                                            */
/******************************************************************************/

void updateGravityAndDrag()

{
    meanAnDF = meanAn0 + meanAnDot * tFP;
    argPerDF = argPer0 + argPerDot * tFP;
    raanDF   = raan0   + raanDot   * tFP;

    meanAn   = meanAnDF;
    argPer   = argPerDF;
    raan     = raanDF + raanCof * tSQ;

    delA     = 1.0 - c1 * tFP;
    delE     = bStar * c4 * tFP;
    delL     = t2Cof * tSQ;

    if (!truncFlag)
    {
        delArgPer  = argPerCof * tFP;
        delMeanAn  = meanAnCof * (pow(1.0 + eta * cos(meanAnDF),3.0) - 
                                  delMeanAn0);

        delAM      = delArgPer + delMeanAn;
        meanAn    += delAM;
        argPer    -= delAM;

        sinMeanAn0 = sin(meanAn0);
        sinMeanAnP = sin(meanAn);

        delA      -= d2 * tSQ + d3 * tCB + d4 * tQD;
        delE      += bStar * c5 * (sinMeanAnP - sinMeanAn0);
        delL      += t3Cof * tCB + t4Cof * tQD + t5Cof * tQN;
    }

    smallA  = sma0Deep * SQR(delA);
    smallE  = ecc0 - delE;

    xL      = meanAn + argPer + raan + meanMotDeep * delL;
    beta    = sqrt(1.0 - SQR(smallE));
    meanMot = KE / pow(smallA,1.5);

    curRaanX       = raan;                                         /* [rad]   */
    curMotionX     = meanMot * MPD / TWOPI;                        /* [rev/d] */
    meanAnomalyX   = meanAn;                                       /* [rad]   */

    return;
}

/******************************************************************************/
/*                                                                            */
/* updateLongPeridics: updates long periodic quantities                       */
/*                                                                            */
/******************************************************************************/

void updateLongPeriodics()

{
    upd7  = 1.0 / (smallA * SQR(beta));

    axN   = smallE * cos(argPer);

    ayNL  = upd7 * ayCof;
    ayN   = smallE * sin(argPer) + ayNL;

    xLL   = upd7 * xLCof * axN;
    xLT   = xL + xLL;

    curArgPerigeeX = xLT - meanAn - raan;                          /* [rad]   */
    return;
}

/******************************************************************************/
/*                                                                            */
/* updateShortPeridics: gets osculating quantities by adding delta terms      */
/*                                                                            */
/******************************************************************************/

void updateShortPeriodics()

{
    eCosE    = kep4 + kep5;
    eSinE    = kep2 - kep3;

    eL2      = SQR(axN) + SQR(ayN);
    upd1     = 1.0 - eL2;
    pL       = smallA * upd1;
    betaL    = sqrt(upd1);

    smallR   = smallA * (1.0 - eCosE);

    rDot     = KE * sqrt(smallA) / smallR * eSinE;
    rfDot    = KE * sqrt(pL) / smallR;

    upd2     = smallA / smallR;
    upd3     = 1.0 / (1.0 + betaL);

    cosU     = upd2 * (cosEpAP - axN + ayN * eSinE * upd3);
    sinU     = upd2 * (sinEpAP - ayN - axN * eSinE * upd3);

    cos2U    = 2.0 * SQR(cosU) - 1.0;
    sin2U    = 2.0 * sinU * cosU;

    smallU   = atan2(sinU,cosU);
    smallU   = reduce(smallU,ZERO,TWOPI);

    upd4     = CK2 / SQR(pL);
    upd5     = CK2 / pL;
    upd6     = 1.5 * upd4 * theta;

    delR     = 0.5 * upd5 * x1mth2 * cos2U;
    delU     = -0.25 * upd4 * x7th2m1 * sin2U;

    delRaan  = upd6 * sin2U;
    delInc   = upd6 * sinInc * cos2U;

    delRDot  = -meanMot * upd5 * x1mth2 * sin2U;
    delRFDot = meanMot * upd5 * (x1mth2 * cos2U + 1.5 * x3th2m1);

    rk       = smallR * (1.0 - 1.5 * upd4 * betaL * x3th2m1) + delR;
    uk       = smallU + delU;

    raanK    = raan  + delRaan;
    incK     = inc0  + delInc;
    rkDot    = rDot  + delRDot;
    rfkDot   = rfDot + delRFDot;

    curArgNodeX     = uk;
    curInclinationX = incK;

    return;
}

/******************************************************************************/
/*                                                                            */
/* keplerX: solves Kepler's equation                                          */
/*                                                                            */
/******************************************************************************/

void keplerX()

{
    int iterationCount;

    capU    = reduce(xLT - raan,ZERO,TWOPI);
    capEpAP = capU;

    iterationCount = 0;

    do
    {
        kep1     = capEpAP;
        sinEpAP  = sin(kep1);
        cosEpAP  = cos(kep1);

        kep2     = axN * sinEpAP;
        kep3     = ayN * cosEpAP;
        kep4     = axN * cosEpAP;
        kep5     = ayN * sinEpAP;

        capEpAP  = kep1 + (capU - kep1 + kep2 - kep3) / (1.0 - kep4 - kep5);
        diffAnom = capEpAP - kep1;

        iterationCount++;
    }
    while (fabs(diffAnom) >= 1.0e-7);

    eccAnom = capEpAP - argPer;

    if (fabs(eccAnom) < ONEPPM)
        trueAnomalyX = PI;
    else
        trueAnomalyX = 2.0 * atan(sqrt((1.0 + ecc0) /
                            (1.0 - ecc0)) * tan(eccAnom / 2.0));

    trueAnomalyX = reduce(trueAnomalyX,ZERO,TWOPI);

/*
    printf("iterations = %d\n",iterationCount);
*/

    return;
}

/******************************************************************************/
/*                                                                            */
/* getOrbitNumberX: gets orbit number and fraction of the orbit               */
/*                  'meanAnomalyX' contains the epoch mean anomaly and the    */
/*                  number of revolutions since the epoch                     */
/*                                                                            */
/******************************************************************************/

void getOrbitNumberX()

{
    refOrbit   = (double) epochOrbitNum;                             /* [rev] */
    curOrbit   = refOrbit + meanAnomalyX * CRREV;                    /* [rev] */
    orbitNum   = (long) curOrbit + 1L;
    orbitFract = 100.0 * modf(curOrbit,&dummyd);

    return;
}

/******************************************************************************/
/*                                                                            */
/* getStateVectorX: calculates position and velocity components               */
/*                                                                            */
/******************************************************************************/

void getStateVectorX()

{
    double mVec[3], nVec[3], uVec[3], vVec[3];
    double cosIK, sinIK, cosRK, sinRK, cosUK, sinUK;
    int    i;

    cosUK   = cos(uk);
    sinUK   = sin(uk);
    cosIK   = cos(incK);
    sinIK   = sin(incK);
    cosRK   = cos(raanK);
    sinRK   = sin(raanK);

    mVec[0] = -sinRK * cosIK;
    mVec[1] = cosRK * cosIK;
    mVec[2] = sinIK;

    nVec[0] = cosRK;
    nVec[1] = sinRK;
    nVec[2] = 0.0;

    for (i = 0; i <= 2; i++)
    {
        uVec[i]     = mVec[i] * sinUK + nVec[i] * cosUK;
        vVec[i]     = mVec[i] * cosUK - nVec[i] * sinUK;

        satPosS[i]  = rk * uVec[i];
        satVelS[i]  = rkDot * uVec[i] + rfkDot * vVec[i];

        satPosS[i] *= EARTHRADIUS;                                  /* [km]   */
        satVelS[i] *= EARTHRADIUS / 60.0;                           /* [km/s] */

        satPosGl[i] = satPosS[i];
        satVelGl[i] = satVelS[i];
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* calcSGP4: calculates SGP4 propagation model                                */
/*                                                                            */
/* For perigee less than 220 km, the truncFlag is set and the equations for   */
/* 'smallA' and 'xL' are truncated after the 'c1' term, and the terms         */
/*  involving 'c5', 'deltaArgPer' and 'deltaMeanAn' are dropped.              */
/*                                                                            */
/******************************************************************************/

void calcSGP4(tmArg)

double tmArg;

{
    getTimeArgs(tmArg);

    if (initNorad)
        calcCommonPerturb();

    updateGravityAndDrag();
    updateLongPeriodics();
    keplerX();
    updateShortPeriodics();
    getStateVectorX();
    getOrbitNumberX();

    return;
}

/******************************************************************************/
/*                                                                            */
/* initDeepSpace:                                                             */
/*                                                                            */
/******************************************************************************/

void initDeepSpace()

{
    return;
}

/******************************************************************************/
/*                                                                            */
/* updateGravity:                                                             */
/*                                                                            */
/******************************************************************************/

void updateGravity()

{
    return;
}

/******************************************************************************/
/*                                                                            */
/* updateDrag:                                                                */
/*                                                                            */
/******************************************************************************/

void updateDrag()

{
    return;
}

/******************************************************************************/
/*                                                                            */
/* deepSpaceSecular:                                                          */
/*                                                                            */
/******************************************************************************/

void deepSpaceSecular()

{
    return;
}

/******************************************************************************/
/*                                                                            */
/* deepSpacePeriodics:                                                        */
/*                                                                            */
/******************************************************************************/

void deepSpacePeriodics()

{
    return;
}

/******************************************************************************/
/*                                                                            */
/* calcSDP4:                                                                  */
/*                                                                            */
/******************************************************************************/

void calcSDP4(tmArg)

double tmArg;

{
    getTimeArgs(tmArg);

    if (initNorad)
        calcCommonPerturb();

    initDeepSpace();
    updateGravity();
    updateDrag();
    deepSpaceSecular();
    updateLongPeriodics();
    keplerX();
    updateShortPeriodics();
    getStateVectorX();
    getOrbitNumberX();

    return;
}

/******************************************************************************/
/*                                                                            */
/* getNoradStateVector: calculates state vector following the NORAD SGP4/SDP4 */
/*                      propagation models                                    */
/*                                                                            */
/* uses deep space model if orbital period is longer than 225 minutes         */
/* all time arguments are in minutes                                          */
/*                                                                            */
/* in the SatTrack main code 'getStateVector()' is called before              */
/* 'getNoradStateVector()'                                                    */
/* the former function calculates the 'TLE Mean Model'                        */
/* the latter overwrites the state vector from the former if main() is        */
/* invoked with the '-m' option                                               */
/*                                                                            */
/******************************************************************************/

void getNoradStateVector(timeArg)

double timeArg;

{
    if (modelFlag)                 /* TLE Mean Model will be calculated from  */
        return;                    /* 'getStateVector()'                      */

    if (initNorad)
    {
        getSemiMajorAxis();        /* sets 'deepSpaceFlag' if T >= 225 min    */
        checkPerigeeHeight();      /* checks if perigee height < 0            */
    }

    if (satCrashFlag)
        return;

    if (deepSpaceFlag)
    /*  calcSDP4(timeArg); */      /* overwrites TLE Mean Model state vector  */
        return;                    /* calcSDP4() is still under construction  */
    else
        calcSGP4(timeArg);         /* overwrites TLE Mean Model state vector  */

    initNorad = FALSE;
    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satprop.c                                            */
/*                                                                            */
/******************************************************************************/
