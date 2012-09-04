/******************************************************************************/
/*                                                                            */
/*  Title       : satcalc.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 03Mar92                                                     */
/*  Last change : 13Feb96                                                     */
/*                                                                            */
/*  Synopsis    : Auxiliary routines for the satellite tracking program       */
/*                'sattrack' (4.1.4 patch).                                   */
/*                                                                            */
/*                                                                            */
/*  SatTrack is Copyright (c) 1992, 1993, 1994, 1995, 1996 by Manfred Bester. */
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
/* getTrueAnomaly: gets mean and true anomaly                                 */
/*                                                                            */
/******************************************************************************/

void getTrueAnomaly(tmArg)

double tmArg;

{
    getOrbitNumber(tmArg);                          /* also gets mean anomaly */
    getSatPrecession();
    kepler();

    return;
}

/******************************************************************************/
/*                                                                            */
/* getOrbitNumber: gets the orbit number and the mean anomaly                 */
/*                 both orbit number and mean anomaly count from the perigee  */
/*                                                                            */
/******************************************************************************/

void getOrbitNumber(tArg)

double tArg;

{
    double dT, dT2, orbFract;

    dT          = tArg - epochDay;
    dT2         = dT * dT;
    curMotion   = epochMeanMotion + 2.0*decayRate*dT + 6.0*decayRateDot*dT2;

    refOrbit    = (double) epochOrbitNum + epochMeanAnomaly / TWOPI; /* [rev] */
    curOrbit    = refOrbit + curMotion * dT;

    orbitNum    = (long) curOrbit + 1L;
    orbFract    = modf(curOrbit,&dummyd);
    orbitFract  = orbFract * 100.0;
    meanAnomaly = orbFract * TWOPI;

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSatPrecession: calculates precession of the satellite's orbit           */
/*                                                                            */
/*                   for reference see:                                       */
/*                   "The Satellite Experimenter's Handbook", M. Davidoff,    */
/*                   The ARRL, Newington                                      */
/*                                                                            */
/******************************************************************************/

void getSatPrecession()

{
    double satPrecFact;

    getSemiMajorAxis();
    checkPerigeeHeight();

    satPrecFact = pow((EARTHRADIUS / semiMajorAxis), 3.5) /
                          SQR(1.0 - SQR(eccentricity)) * CDR;

    raanPrec    = -9.95 * satPrecFact * cosInclination;
    perigeePrec =  4.97 * satPrecFact * (5.0 * SQR(cosInclination) - 1.0);

    return;
}
 
/******************************************************************************/
/*                                                                            */
/* kepler: solves Kepler's equation iteratively                               */
/*                                                                            */
/******************************************************************************/

void kepler()
 
{
    register double eccen, eccAnom, meanAnom, diffAnom;

    eccen    = eccentricity;
    meanAnom = meanAnomaly;
    eccAnom  = meanAnomaly;                              /* initial guess     */

    do
    {
        diffAnom = (eccAnom - eccen * sin(eccAnom) - 
                    meanAnom) / (1.0 - eccen * cos(eccAnom));
        eccAnom -= diffAnom;
    }
    while (fabs(diffAnom) >= CASR);                     /* 1 arcsec precision */

    if (fabs(eccAnom - PI) < CASR)
        trueAnomaly = PI;
    else
        trueAnomaly = 2.0 * atan(sqrt((1.0 + eccen) / (1.0 - eccen)) * 
                             tan(eccAnom / 2.0));

    trueAnomaly = reduce(trueAnomaly,ZERO,TWOPI);

    return;
}
 
/******************************************************************************/
/*                                                                            */
/* getSubSatPoint: calculates sub-satellite point in geodetic coordinates     */
/*                                                                            */
/******************************************************************************/

void getSubSatPoint(sspObject)

int sspObject;

{
    double geocenPosSSP[3], geocenPosEquat[3];
    double rObj, rEquat, lng, rEff, a, c, h, e2, z, lat, lat1;
    int    i;

    if (sspObject == SAT)
    {
        for (i = 0; i <= 2; i++)
            geocenPosSSP[i] = satPosGl[i];
    }

    if (sspObject == SUN)
    {
        for (i = 0; i <= 2; i++)
            geocenPosSSP[i] = sunPosGl[i];
    }

    rObj = absol(geocenPosSSP);

    geocenPosEquat[0] = geocenPosSSP[0];
    geocenPosEquat[1] = geocenPosSSP[1];
    geocenPosEquat[2] = ZERO;

    rEquat = absol(geocenPosEquat);

    geocenPosEquat[0] /= rEquat;
    geocenPosEquat[1] /= rEquat;

    lng = gasTime - atan2(geocenPosEquat[1],geocenPosEquat[0]);
    lng = reduce(lng,-PI,PI);

    z   = geocenPosSSP[2];                                            /* [km] */
    lat = atan(z/rEquat);
    e2  = 2.0 * EARTHFLAT - SQR(EARTHFLAT);
    a   = EARTHRADIUS;                                                /* [km] */

    do
    {
        lat1 = lat;
        c    = 1.0 / sqrt(1.0 - e2 * SQR(sin(lat1)));
        lat  = atan((z + a * c * e2 * sin(lat1))/rEquat);
      
    } while (fabs(lat1 - lat) > ONEPPM);

    if (sspObject == SAT)
    {
        h            = rEquat / rObj * a / cos(lat) - a * c;
        rEff         = a - h;

        satLat       = lat;
        satLong      = lng;

        satHeight    = rObj- rEff;
        satCrashFlag = (satHeight < 0.0) ? TRUE : FALSE;
    }

    if (sspObject == SUN)
    {
        sunLat       = lat;
        sunLong      = lng;
    }

    return;
}
 
/******************************************************************************/
/*                                                                            */
/* getStateVector: calculates satellite's state vector, i.e. position and     */
/*                 velocity in the mean equatorial coordinate system (M50)    */
/*                                                                            */
/*                 This function also provides essential calculations for the */
/*                 orbit number and squint angle calculations and therefore   */
/*                 needs to be called even when the state vector has been     */
/*                 calculated with the SGP4 model already.                    */
/*                                                                            */
/******************************************************************************/

void getStateVector(timeArg)

double timeArg;
 
{
    double pVec[3], qVec[3];
    double vxOrb, vyOrb, vzOrb;
    double xObs, yObs, zObs, xObsOrb, yObsOrb, zObsOrb;
    double tmp, cosArgPerigee, sinArgPerigee, cosRaan, sinRaan;
    double phi, cosPhi, sinPhi;
    double xOrb = ZERO;
    double yOrb = ZERO;
    double zOrb = ZERO;
    int    i;

    if (propModelType == TLEMEAN)
    {
        getTrueAnomaly(timeArg);
        curArgPerigee  = epochArgPerigee + (timeArg - epochDay) * perigeePrec;
        curRaan        = epochRaan + (timeArg - epochDay) * raanPrec;
        curMeanMotion  = curMotion;
        curInclination = inclination;
    }

    else
    {
        meanAnomaly    = meanAnomalyX;
        trueAnomaly    = trueAnomalyX;
        curArgPerigee  = curArgPerigeeX;
        curRaan        = curRaanX;
        curMeanMotion  = curMotionX;
        curInclination = curInclinationX;
    }

    cosTrueAnomaly = cos(trueAnomaly);
    sinTrueAnomaly = sin(trueAnomaly);

    cosArgPerigee  = cos(curArgPerigee);
    sinArgPerigee  = sin(curArgPerigee);

    cosRaan        = cos(curRaan);
    sinRaan        = sin(curRaan);

    satRadius      = semiMajorAxis * (1.0 - SQR(eccentricity))
                     / (1.0 + eccentricity * cosTrueAnomaly);

    /* the next lines are needed in any case for the squint angle calculation */

    if (propModelType == TLEMEAN || attitudeFlag)
    {
        xOrb    =  satRadius * cosTrueAnomaly;   /* position in orbital plane */
        yOrb    =  satRadius * sinTrueAnomaly;
        zOrb    =  ZERO;
    }

    /* calculate state vector if not yet calculated from SGP4/SDP4 model */

    if (propModelType == TLEMEAN)
    {
        tmp     =  sqrt(GMSGP / (semiMajorAxis * (1.0 - SQR(eccentricity))));

        vxOrb   = -tmp * sinTrueAnomaly;         /* velocity in orbital plane */
        vyOrb   =  tmp * (cosTrueAnomaly + eccentricity);
        vzOrb   =  ZERO;

        pVec[0] =  cosArgPerigee*cosRaan - sinArgPerigee*sinRaan*cosInclination;
        pVec[1] =  cosArgPerigee*sinRaan + sinArgPerigee*cosRaan*cosInclination;
        pVec[2] =  sinArgPerigee*sinInclination;

        qVec[0] = -sinArgPerigee*cosRaan - cosArgPerigee*sinRaan*cosInclination;
        qVec[1] = -sinArgPerigee*sinRaan + cosArgPerigee*cosRaan*cosInclination;
        qVec[2] =  cosArgPerigee*sinInclination;

        for (i = 0; i <= 2; i++)
        {
            satPosGl[i] =  xOrb * pVec[i] +  yOrb * qVec[i];
            satVelGl[i] = vxOrb * pVec[i] + vyOrb * qVec[i];
        }
    }

    /* calculate squint vector if satellite's attitude is specified */

    if (attitudeFlag)
    {
        phi     = reduce(lasTime - curRaan - HALFPI,ZERO,TWOPI);
        cosPhi  = cos(phi);
        sinPhi  = sin(phi);

        xObs    = siteVecGl[0];
        yObs    = siteVecGl[1];
        zObs    = siteVecGl[2];

        xObsOrb = (-sinArgPerigee*cosInclination*cosPhi + 
	            cosArgPerigee*sinPhi) * xObs + 
	          ( sinArgPerigee*cosInclination*sinPhi + 
	            cosArgPerigee*cosPhi) * yObs -
	            sinArgPerigee*sinInclination * zObs;

        yObsOrb = (-cosArgPerigee*cosInclination*cosPhi - 
	            sinArgPerigee*sinPhi) * xObs + 
	          ( cosArgPerigee*cosInclination*sinPhi - 
	            sinArgPerigee*cosPhi) * yObs - 
	            cosArgPerigee*sinInclination * zObs;

        zObsOrb =  -sinInclination*cosPhi * xObs +
                    sinInclination*sinPhi * yObs + 
	            cosInclination * zObs;

        satSquintGl[0] = -xOrb - xObsOrb;
        satSquintGl[1] = -yOrb - yObsOrb;
        satSquintGl[2] = -zOrb - zObsOrb;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSiteVector: computes the site position and velocity in the mean         */
/*                equatorial coordinate system. The matrix 'siteRotMat' is    */
/*                used by getTopoVector() to convert geocentric coordinates   */
/*                to topocentric (observer-centered) coordinates.             */
/*                                                                            */
/******************************************************************************/

void getSiteVector()

{
    double rotVec0[3], rotVec1[3], rotVec2[3];
    double lat, cosSiteLat, sinSiteLat, siteRA, cosSiteRA, sinSiteRA;
    double sqf, a, c, s, h, vecLen;
    int    i;

    lat         = siteLat;
    cosSiteLat  = cos(lat);
    sinSiteLat  = sin(lat);

    siteRA      = lasTime;
    cosSiteRA   = cos(siteRA);
    sinSiteRA   = sin(siteRA);

    sqf = SQR(1.0 - EARTHFLAT);

    a = EARTHRADIUS;
    c = 1.0 / sqrt(SQR(cosSiteLat) + sqf * SQR(sinSiteLat));
    s = sqf * c;
    h = siteAlt;

    /* this vector is the geocentric position of the site [km] */

    sitePosGl[0] = (a*c + h) * cosSiteLat * cosSiteRA;
    sitePosGl[1] = (a*c + h) * cosSiteLat * sinSiteRA;
    sitePosGl[2] = (a*s + h) * sinSiteLat;

    /* this vector is not affected by the non-spherical Earth */

    siteVelGl[0] = -SIDRATE * sitePosGl[1];
    siteVelGl[1] =  SIDRATE * sitePosGl[0];
    siteVelGl[2] =  ZERO;

    /* set up the three unit vectors for the rotation matrix */

    rotVec1[0] = -sinSiteRA;
    rotVec1[1] =  cosSiteRA;
    rotVec1[2] =  ZERO;

    vecLen = absol(sitePosGl);

    for (i = 0; i <= 2; i++)
        rotVec2[i] = sitePosGl[i] / vecLen;

    cross(rotVec2,rotVec1,rotVec0,-1);

    /* this matrix is the geocentric site rotation matrix */

    siteRotMatGl[0][0] = rotVec0[0];
    siteRotMatGl[0][1] = rotVec0[1];
    siteRotMatGl[0][2] = rotVec0[2];
    siteRotMatGl[1][0] = rotVec1[0];
    siteRotMatGl[1][1] = rotVec1[1];
    siteRotMatGl[1][2] = rotVec1[2];
    siteRotMatGl[2][0] = rotVec2[0];
    siteRotMatGl[2][1] = rotVec2[1];
    siteRotMatGl[2][2] = rotVec2[2];

    return;
}

/******************************************************************************/
/*                                                                            */
/* getRange: calculates satellite range                                       */
/*                                                                            */
/******************************************************************************/

void getRange()

{
    double satDist[3];
    int    i;

    for (i = 0; i <= 2; i++)
        satDist[i] = satPosGl[i] - sitePosGl[i];

    satRange    = absol(satDist);

    rangeRate   = ((satVelGl[0] - siteVelGl[0]) * satDist[0] + 
                   (satVelGl[1] - siteVelGl[1]) * satDist[1] +
                   (satVelGl[2] - siteVelGl[2]) * satDist[2]) / satRange;

    satVelocity = absol(satVelGl);

    return;
}

/******************************************************************************/
/*                                                                            */
/* getTopoVector: converts from geocentric mean equatorial coordinates to     */
/*                topocentric coordinates                                     */
/*                                                                            */
/*                topocentric coordinate system used: right-handed            */
/*                +X = south,  +Y = east,  +Z = up                            */
/*                                                                            */
/******************************************************************************/

void getTopoVector(objGeo,pobjTopo)

double objGeo[3], (*pobjTopo)[3];

{
    double objDistT[3], objTopo[3];
    double objDistNorm;
    int    i;

    for (i = 0; i <= 2; i++)
        objDistT[i] = objGeo[i] - sitePosGl[i];

    objDistNorm = absol(objDistT);

    for (i = 0; i <= 2; i++)
        objDistT[i] /= objDistNorm;

    multMatVec(objDistT,objTopo,siteRotMatGl);

    for (i = 0; i <= 2; i++)
        (*pobjTopo)[i] = objTopo[i];

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSunPhaseAngle: calculates phase angle between the ground station and    */
/*                   the Sun, as seen from the satellite                      */
/*                                                                            */
/******************************************************************************/

void getSunPhaseAngle()

{
    double satDistGnd[3], satDistSun[3], satNormGnd, satNormSun, arg;
    int    i;

    for (i = 0; i <= 2; i++)
    {
        satDistGnd[i] = sitePosGl[i] - satPosGl[i];
        satDistSun[i] = sunPosGl[i]  - satPosGl[i];
    }

    satNormGnd    = absol(satDistGnd);
    satNormSun    = absol(satDistSun);
    arg           = scalar(satDistGnd,satDistSun) / satNormGnd / satNormSun;
    sunPhaseAngle = fabs(acos(arg));

    return;
}

/******************************************************************************/
/*                                                                            */
/* getAziElev: calculates object's vector position in the local geodetic      */
/*             frame, as well as azimuth and elevation angles                 */
/*                                                                            */
/******************************************************************************/

void getAziElev(switchFlag)

int switchFlag;

{
    double  localVec[3], aziVec[3], aziPerp[3], refrVec[3];
    double  vectLen, azimuth, elevation, arg;
    double  refIndex, refCorr, diffCorr, airMass;
    int     i;

    if (switchFlag == SAT)
    {
/*  if astronomical precession is needed the following three lines have to be */
/*  activated and the fourth line needs to be commented out                   */
/*
        getPrecMatrix();
        multMatVec(satPosGl,satPosPrec,precMatrix);
        getTopoVector(satPosPrec,localVec);
*/

        getTopoVector(satPosGl,localVec);
    }

/*  for the Sun and the Moon the apparent place is calculated already and     */
/*  astronomical precession is not needed                                     */

    if (switchFlag == SUN)
        getTopoVector(sunPosGl,localVec);

    if (switchFlag == MOON)
        getTopoVector(moonPosGl,localVec);

    aziVec[0]  = localVec[0];
    aziVec[1]  = localVec[1];
    aziVec[2]  = ZERO;

    vectLen    = absol(aziVec);

    aziVec[0] /= vectLen;
    aziVec[1] /= vectLen;

    if (aziVec[0] == ZERO)
        azimuth = (aziVec[1] > ZERO) ? HALFPI : THREEHALFPI;
    else
        azimuth = PI - atan2(aziVec[1],aziVec[0]);

    if (azimuth < ZERO)
        azimuth += PI;

    arg = localVec[2];

    if (arg >  1.0) arg =  1.0;      /* protect against out-of-range problems */
    if (arg < -1.0) arg = -1.0;

    elevation = asin(arg);

    /* apply correction for atmospheric refraction for visible tracking */

    getRefraction(atmPressure,ambTemperature,relHumidity,
                  WAVEL,REFWAVEL,EARTHRADIUS,SCALEHEIGHT,
                  siteAlt,elevation,&refIndex,&refCorr,&diffCorr,&airMass);

    aziPerp[0] =  aziVec[1];
    aziPerp[1] = -aziVec[0];
    aziPerp[2] = ZERO;

    cross(aziPerp,localVec,refrVec,1);

    vectLen = refCorr;

    for (i = 0; i <=2; i++)
        localVec[i] += vectLen * refrVec[i];

    vectLen = absol(localVec);

    for (i = 0; i <=2; i++)
        localVec[i] /= vectLen;

    arg = localVec[2];

    if (arg >  1.0) arg =  1.0;      /* protect against out-of-range problems */
    if (arg < -1.0) arg = -1.0;

    elevation = asin(arg);

    if (switchFlag == SAT)
    {
        for (i = 0; i <= 2; i++)
            localVecSatGl[i] = localVec[i];

        satAzimuth   = azimuth;
        satElevation = elevation;
    }

    if (switchFlag == SUN)
    {
        for (i = 0; i <= 2; i++)
            localVecSunGl[i] = localVec[i];

        sunAzimuth   = azimuth;
        sunElevation = elevation;
    }

    if (switchFlag == MOON)
    {
        for (i = 0; i <= 2; i++)
            localVecMoonGl[i] = localVec[i];

        moonAzimuth   = azimuth;
        moonElevation = elevation;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* satEclipse: calculates the eclipses of the satellite                       */
/*                                                                            */
/* this function assumes that the Sun is point-like rather than extended      */
/* and that the Earth is perfectly spherical                                  */
/*                                                                            */
/******************************************************************************/

int satEclipse(elev)

double elev;

{
    double satPara, satPerp, satDist, alpha, beta;

    satDist = absol(satPosGl);

    satPara = scalar(satPosGl,sunPosGl) / sunDist;
    satPerp = sqrt(SQR(satDist) - SQR(satPara));

    /* case 1: satellite is on side of the Earth facing the Sun */

    if (satPara > ZERO)   /* satellite is always in daylight */
    {
        if (elev < ZERO)
            return(DAY);
        
        if (elev >= ZERO && sunElevation <= TWILIGHT)
            return(VISIBLE);
        else
            return(DAY);
    }

    alpha = asin(EARTHRADIUS / sunDist);
    beta  = atan(satPerp / (satPara + sunDist));

    /* case 2: satellite is on side of the Earth facing away from the Sun */

    if (beta >= alpha)    /* satellite can see the Sun */
    {
        if (elev >= ZERO && sunElevation <= TWILIGHT)
            return(VISIBLE);
        else
            return(DAY);
    }

    else                  /* satellite is in the Earth's shadow */
        return(NIGHT);
}

/******************************************************************************/
/*                                                                            */
/* getDoppler: calculates Doppler shift [Hz]                                  */
/*                                                                            */
/*             the inverted sign on the uplink Doppler shift takes into       */
/*             account the fact that the ground station rather than the       */
/*             satellite needs to adjust the frequency                        */
/*                                                                            */
/******************************************************************************/

void getDoppler()

{
    if (satElevation > TRACKLIMIT1)
    {
        downlinkDopp = -(downlinkFreq + freqOffset) * rangeRate/CVAC;
        uplinkDopp   =  (uplinkFreq + freqOffset*xponderSign) * rangeRate/CVAC;
    }

    else
    {
        downlinkDopp = ZERO;
        uplinkDopp   = ZERO;
    }
    
    return;
}

/******************************************************************************/
/*                                                                            */
/* getPathLoss: calculates path loss for given downlink and uplink            */
/*              frequency [Hz], range [km], and velocity of light [km/s]      */
/*                                                                            */
/******************************************************************************/

void getPathLoss()

{
    if (downlinkFreq > ONEPPM && satElevation > TRACKLIMIT1)
        downlinkLoss = 20.0 * log10(FOURPI * satRange / CVAC * downlinkFreq);
    else
        downlinkLoss = ZERO;

    if (uplinkFreq > ONEPPM && satElevation > TRACKLIMIT1)
        uplinkLoss   = 20.0 * log10(FOURPI * satRange / CVAC * uplinkFreq);
    else
        uplinkLoss   = ZERO;

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSquintAngle: calculates squint angle                                    */
/*                                                                            */
/******************************************************************************/

void getSquintAngle()

{
    double vectLen;

    if (attitudeFlag)
    {
        vectLen     = absol(satSquintGl);
        squintAngle = acos( -(scalar(satAttGl,satSquintGl)) / vectLen);
    }

    else
    {
        squintAngle = ZERO;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getPhase: gets orbital phase of the satellite, i.e. the mean anomaly in    */
/*           units of usually 0 to 256                                        */
/*                                                                            */
/******************************************************************************/

void getPhase()

{
    satPhase = reduce(meanAnomaly/TWOPI*maxPhase + perigeePhase,ZERO,maxPhase);
    return;
}
 
/******************************************************************************/
/*                                                                            */
/* getMode: gets satellite mode                                               */
/*                                                                            */
/******************************************************************************/

void getMode()

{
    int curMode;

    sprintf(modeString,"  ");

    if (numModes > 0)
    {
        for (curMode = 0; curMode < numModes; curMode++)
        {
            if (satPhase >= modes[curMode].minPhase && 
                satPhase <  modes[curMode].maxPhase)
            {
                sprintf(modeString,"%s",modes[curMode].modeStr);
            }
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getShuttleOrbit: gets orbit number for the space shuttle                   */
/*                                                                            */
/* convention for element sets from NASA  : add -1                            */
/*                             from NORAD : add  0                            */
/*                                                                            */
/******************************************************************************/

void getShuttleOrbit()

{
    if (satTypeFlag != STS)
    {
        stsOrbit      = 0.0;
        stsOrbitNum   = 0L;
        stsOrbitFract = 0.0;
        return;
    }

    stsOrbit      = curOrbit + curArgPerigee * CRREV;
    stsOrbit     += (!strcmp(elementType,"NASA")) ? -1.0 : 0.0;
    stsOrbitNum   = (long) stsOrbit;
    stsOrbitFract = 100.0 * modf(stsOrbit,&dummyd);

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satcalc.c                                            */
/*                                                                            */
/******************************************************************************/
