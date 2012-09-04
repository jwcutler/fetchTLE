/******************************************************************************/
/*                                                                            */
/*  Title       : satcity.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 17Sep92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines that deal with the city data base.                 */
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
#include <string.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* ground track vector segments                                               */
/*                                                                            */
/******************************************************************************/

char *gndTrkVector[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                         "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW" };

/******************************************************************************/
/*                                                                            */
/* getGroundTrack: calculates the distance and direction of the nearest city  */
/*                                                                            */
/******************************************************************************/

void getGroundTrack(latSat,lngSat,cityLtd,cityLng,groundTrkDist,
                    groundTrkDir,groundTrkCity)

double latSat, lngSat, *cityLtd, *cityLng, *groundTrkDist;
char   *groundTrkDir, *groundTrkCity;

{
    double cityVec[3], sspVec[3], distVec[3], northVec[3], eastVec[3];
    double aziX, aziY, azi, alpha, distR, distRad, arg;
    double cosLatSite, sinLatSite, cosLngSite, sinLngSite;
    double cosLatSat, sinLatSat, cosLngSat, sinLngSat;
    double latLimit = ZERO;
    double lngLimit = ZERO;
    int    i, j, dir, cityPointer;

    cityPointer = 0;                       /* default setting: the North Pole */
                                           /* (or first entry in cities.dat)  */

    cosLatSat = cos(latSat);               /* geodetic latitude and longitude */
    sinLatSat = sin(latSat);
    cosLngSat = cos(lngSat);
    sinLngSat = sin(lngSat);

    sspVec[0] = cosLngSat*cosLatSat;
    sspVec[1] = sinLngSat*cosLatSat;
    sspVec[2] = sinLatSat;

    distRad   = ONEMEG;

    if (fabs(latSat) > 70*CDR)         /* search limits for polar regions     */
    {
        latLimit =  30.1*CDR;
        lngLimit = 360.1*CDR;
    }

    if (fabs(latSat) <= 70*CDR)        /* search limits for rest of the world */
    {
        latLimit =  45.0*CDR;
        lngLimit =  45.0*CDR;
    }

    if (latSat > 30*CDR && latSat <  50*CDR &&
        lngSat > 70*CDR && lngSat < 130*CDR)
    {
        latLimit =  20.0*CDR;          /* search limits for the United States */
        lngLimit =  20.0*CDR;
    }

    for (i = 0; i < numCities; i++)
    {
        if (fabs(latSat - city[i].lat) < latLimit &&
            fabs(lngSat - city[i].lng) < lngLimit)
        {
            distVec[0] = city[i].X - sspVec[0];
            distVec[1] = city[i].Y - sspVec[1];
            distVec[2] = city[i].Z - sspVec[2];

            distR = absol(distVec);

            if (distR < distRad - ONEPPM)
            {
                distRad     = distR;
                cityPointer = i;
            }
        }

        newCityNum = cityPointer;
    }

    sprintf(groundTrkCity,"%s",city[cityPointer].cty);

    cityVec[0] = city[cityPointer].X;
    cityVec[1] = city[cityPointer].Y;
    cityVec[2] = city[cityPointer].Z;

    for (j = 0; j <= 2; j++)
        distVec[j] = cityVec[j] - sspVec[j];

    arg = scalar(cityVec,sspVec);

    if (arg >  1.0) arg =  1.0;
    if (arg < -1.0) arg = -1.0;

    alpha = acos(arg);

    *groundTrkDist = alpha * EARTHRADIUS;
    *cityLtd       = city[cityPointer].lat * CRD;
    *cityLng       = city[cityPointer].lng * CRD;

    cosLatSite  = cos(city[cityPointer].lat);
    sinLatSite  = sin(city[cityPointer].lat);
    cosLngSite  = cos(city[cityPointer].lng);
    sinLngSite  = sin(city[cityPointer].lng);

    northVec[0] = -cosLngSite*sinLatSite;
    northVec[1] = -sinLngSite*sinLatSite;
    northVec[2] =  cosLatSite;

    cross(cityVec,northVec,eastVec,-1);

    aziX = scalar(northVec,distVec);
    aziY = scalar(eastVec,distVec);

    if (aziX == 0.0)
        azi = (aziY > 0.0) ? HALFPI : THREEHALFPI;
    else
        azi = PI - atan2(aziY,aziX);

    if (azi < 0.0)
        azi += PI;

    azi *= CRD;

    dir  = (int) ((azi + 11.25) / 22.5) % 16;

    if (cityPointer == 0) dir = 8;                          /* the North Pole */
    if (cityPointer == 1) dir = 0;                          /* the South Pole */

    sprintf(groundTrkDir,"%s",gndTrkVector[dir]);
    return;
}

/******************************************************************************/
/*                                                                            */
/* readCities: reads the city data base into a structure                      */
/*                                                                            */
/******************************************************************************/

void readCities()

{
    double lat, lng, alt;
    int    i;
    char   cityDat[80], cityName[50], str[100];
    FILE   *cityFile;

    if (verboseFlag)
        printf("\nreading city data base ...\n");

    sprintf(cityDat,"%s/%s/%s",strpHome,DATA,CITIES);

    if ((cityFile = fopen(cityDat,"r")) == NULL)
    {
        printf("cannot open input file '%s'\n",cityDat);
        exit(-1);
    }

    numCities = 0;

    while (fgets(str,80,cityFile) && numCities < MAXCITIES)
    {
        sscanf(str,"`%[^`]`,%lf,%lf,%lf",cityName,&lat,&lng,&alt);

        city[numCities].lat = lat;
        city[numCities].lng = lng;
        city[numCities].alt = alt;

        if ((int) strlen(cityName) > MAXCITYLEN)
        {
            i = MAXCITYLEN;

            do
            {
                i--;
            }
            while (cityName[i] != ' ');

            if (cityName[i-1] == ',')
                i--;

            cityName[i] = '\0';
        }

        strcpy(city[numCities].cty,cityName);

        city[numCities].lat *= CDR;
        city[numCities].lng *= CDR;

        numCities++;
    }

    fclose(cityFile);

    if (verboseFlag)
        printf("data base contains %d locations\n",numCities);

    return;
}

/******************************************************************************/
/*                                                                            */
/* calcCityVectors: calculates city vectors for the nearest city display      */
/*                  in the geodetic reference system                          */
/*                                                                            */
/******************************************************************************/

void calcCityVectors()

{
    double cityCosLat, citySinLat, cityCosLng, citySinLng;
    int    i;

    for (i = 0; i < numCities; i++)
    {
        cityCosLat = cos(city[i].lat);
        citySinLat = sin(city[i].lat);
        cityCosLng = cos(city[i].lng);
        citySinLng = sin(city[i].lng);

        city[i].X  = cityCosLat * cityCosLng;
        city[i].Y  = cityCosLat * citySinLng;
        city[i].Z  = citySinLat;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getMaidenHead: calculates Maidenhead locator for geodetic coordinates      */
/*                                                                            */
/******************************************************************************/

void getMaidenHead(mLtd,mLng,mStr)

double mLtd, mLng;
char   *mStr;

{
    int i, j, k, l, m, n;

    mLng = reduce(180.0 - mLng * CRD + ONEPPM,0.0,360.0);
    mLtd = reduce( 90.0 + mLtd * CRD + ONEPPM,0.0,360.0);

    i = (int) (mLng / 20.0);
    j = (int) (mLtd / 10.0);

    mLng -= (double) i * 20.0;
    mLtd -= (double) j * 10.0;

    k = (int) (mLng / 2.0);
    l = (int) (mLtd / 1.0);

    mLng -= (double) k * 2.0;
    mLtd -= (double) l * 1.0;

    m = (int) (mLng * 12.0);
    n = (int) (mLtd * 24.0);

    sprintf(mStr,"%c%c%d%d%c%c",
        'A' + (char) i, 'A' + (char) j, k, l, 'A' + (char) m, 'A' + (char) n);

    return;
}

/******************************************************************************/
/*                                                                            */
/* getPosFromMaidenHead: calculates position from Maidenhead locator          */
/*                                                                            */
/******************************************************************************/

int getPosFromMaidenHead(mStr,mLtd,mLng)

char   *mStr;
double *mLtd, *mLng;

{
    int  error;

    upperCase(mStr);

    error = ((int) strlen(mStr) != 6) ? TRUE : FALSE;

    if (mStr[0] < 'A' || mStr[0] > 'R') error = TRUE;
    if (mStr[1] < 'A' || mStr[1] > 'R') error = TRUE;
    if (mStr[2] < '0' || mStr[2] > '9') error = TRUE;
    if (mStr[3] < '0' || mStr[3] > '9') error = TRUE;
    if (mStr[4] < 'A' || mStr[4] > 'X') error = TRUE;
    if (mStr[5] < 'A' || mStr[5] > 'X') error = TRUE;

    *mLng = (double) (mStr[0] - 'A') * 20.0 + 
            (double) (mStr[2] - '0') * 2.0 + 
            (double) (mStr[4] - 'A') / 12.0 + 2.50 / 60.0;

    if (*mLng < 0.0 || *mLng > 360.050)
        error = TRUE;

    *mLtd = (double) (mStr[1] - 'A') * 10.0 + 
            (double) (mStr[3] - '0') * 1.0 + 
            (double) (mStr[5] - 'A') / 24.0 + 1.25 / 60.0;

    if (*mLtd < 0.0 || *mLtd > 180.025)
        error = TRUE;

    *mLng -= 180.0;
    *mLng *=  -1.0;
    *mLng *=   CDR;

    *mLtd -=  90.0;
    *mLtd *=   CDR;

    if (error)
    {
        *mLng = 0.0;
        *mLtd = 0.0;
    }

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* End of function block satcity.c                                            */
/*                                                                            */
/******************************************************************************/
