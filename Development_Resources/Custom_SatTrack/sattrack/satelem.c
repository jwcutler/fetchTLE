/******************************************************************************/
/*                                                                            */
/*  Title       : satelem.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 19Jun91                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This function reads the NORAD/NASA 2-line element sets.     */
/*                                                                            */
/*                                                                            */
/*  Data for each satellite consist of three lines in the following format:   */
/*                                                                            */
/*  XXXXXXXXXXX                                                               */
/*  1 AAAAAU 00  0  0 BBBBB.BBBBBBBB +.CCCCCCCC +DDDDD-D +EEEEE-E F  GGGZ     */
/*  2 AAAAA HHH.HHHH III.IIII JJJJJJJ KKK.KKKK LLL.LLLL MM.MMMMMMMMNNNNNZ     */
/*                                                                            */
/*  X = Satellite name                                                        */
/*  A = Catalog number                                                        */
/*  B = Epoch time                                                            */
/*  C = One half of first derivative of mean motion (decay rate)              */
/*  D = One sixth of second derivative of mean motion                         */
/*  E = BSTAR drag coefficient                                                */
/*  F = Ephemeris type                                                        */
/*  G = Number of the element set                                             */
/*  H = Inclination                                                           */
/*  I = RAAN                                                                  */
/*  J = Eccentricity                                                          */
/*  K = Argument of perigee                                                   */
/*  L = Mean anomaly                                                          */
/*  M = Mean motion                                                           */
/*  N = Orbit number                                                          */
/*  Z = Check sum (modulo 10)                                                 */
/*                                                                            */
/*                                                                            */
/*  Line 0 is an eleven-character name. Lines 1 and 2 are the standard        */
/*  two-line orbital element set format identical to that used by NASA and    */
/*  NORAD. The format description is as follows:                              */
/*                                                                            */
/*  Line 0:                                                                   */
/*  Column     Description                                                    */
/*    1-11     Satellite name                                                 */
/*                                                                            */
/*  Line 1:                                                                   */
/*    1- 1     Line number of element set                                     */
/*    3- 7     Satellite number                                               */
/*    8- 8     Classification (U = unclassified)                              */
/*   10-11     International designator (last two digits of launch year)      */
/*   12-14     International designator (launch number of the year)           */
/*   15-17     International designator (piece of launch)                     */
/*   19-20     Epoch year (last two digits of year)                           */
/*   21-32     Epoch (Julian day and fractional portion of the day)           */
/*   34-43     One half of first time derivative of mean motion (decay rate)  */
/*             or ballistic coefficient (depending on ephemeris type)         */
/*   45-52     One sixth of second time derivative of mean motion             */
/*             (decimal point assumed; blank if n/a)                          */
/*   54-61     BSTAR drag coefficient if SGP4/SGP8 general perturbation       */
/*             theory used; otherwise, radiation pressure coefficient         */
/*             (decimal point assumed)                                        */
/*   63-63     Ephemeris type                                                 */
/*   66-68     Element set number                                             */
/*   69-69     Check sum (modulo 10)                                          */
/*             (letters, blanks, periods, plus sign = 0; minus sign = 1)      */
/*                                                                            */
/*  Line 2:                                                                   */
/*    1- 1     Line number of element set                                     */
/*    3- 7     Satellite number                                               */
/*    9-16     Inclination [deg]                                              */
/*   18-25     Right Ascension of the ascending node [deg]                    */
/*   27-33     Eccentricity (decimal point assumed)                           */
/*   35-42     Argument of perigee [deg]                                      */
/*   44-51     Mean anomaly [deg]                                             */
/*   53-63     Mean motion [rev/d]                                            */
/*   64-68     Orbit (revolution number) at epoch [rev]                       */
/*   69-69     Check sum (modulo 10)                                          */
/*                                                                            */
/*  All other columns are blank or fixed.                                     */
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

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

#ifdef HPTERM
#include "hpterm.h"               /* definitions of hpterm macros             */
#else
#include "vt100.h"                /* definitions of VT100 macros              */
#endif

/******************************************************************************/
/*                                                                            */
/* readTleFile: reads entire data file with two-line Keplerian elements into  */
/*              a structure                                                   */
/*                                                                            */
/* return code: 0 = no error                                                  */
/*              1 = file not found                                            */
/*                                                                            */
/******************************************************************************/

int readTleFile(homeDir,elementFile,nTle)

int  nTle;
char *homeDir, *elementFile;

{
    int  k, duplFlag;
    char str[80], line0[80], line1[80], line2[80], inputFile[120];
    FILE *fpi;

    lowerCase(elementFile);
    sprintf(inputFile,"%s/%s/%s",homeDir,TLE,elementFile);

    if ((fpi = fopen(inputFile, "r")) == NULL)
        return(1);

    if (verboseFlag)
        printf("\nreading TLE data from %s ...\n",elementFile);

    while (fgets(str,80,fpi) && nTle < MAXSATS)
    {
        sprintf(line0,"%s",str);
        line0[(int) strlen(line0) - 1] = '\0';
        truncBlanks(line0);
        cleanSatName(line0);

        fgets(str,80,fpi);
        sprintf(line1,"%s",str);
        line1[(int) strlen(line1) - 1] = '\0';
        truncBlanks(line1);

        fgets(str,80,fpi);
        sprintf(line2,"%s",str);
        line2[(int) strlen(line2) - 1] = '\0';
        truncBlanks(line2);

        satNum   = (long) (getElement(line1,3,7) + ONEPPM);
        duplFlag = FALSE;

        for (k = 0; k < nTle; k++)             /* check for duplicate entries */
        {                                      /* using the object number     */
            if (satNum == tle[k].tleSatNum)
                duplFlag = TRUE;
        }

        if (!duplFlag)                         /* add TLE set to structure    */
        {
            strcpy(tle[nTle].tleLine0,line0);
            strcpy(tle[nTle].tleLine1,line1);
            strcpy(tle[nTle].tleLine2,line2);
       
            tle[nTle].tleSatNum = satNum;
            nTle++;
        }
    }

    fclose(fpi);
    numTle = nTle;

    if (verboseFlag)
        printf("data base contains %d element sets\n",numTle);

    return(0);
}

/******************************************************************************/
/*                                                                            */
/* getTleSets: reads all tle sets from tlex.dat                               */
/*                                                                            */
/******************************************************************************/

void getTleSets()

{
    int error;

    error = readTleFile(strpHome,TLEX,0);

    if (error)
    {
        doBeep(); nl(); reverseBlink();
        printf(" File '%s' not found \n",TLEX);
        normal(); nl();
        exit(-1);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* readTle: reads two-line element set for a specified satellite              */
/*                                                                            */
/* return code: 0 = data valid                                                */
/*              1 = check sum error in line 1                                 */
/*              2 = check sum error in line 2                                 */
/*              3 = specified satellite not found                             */
/*              4 = multiple entries for specified satellite                  */
/*                                                                            */
/* satNameFlag:   TRUE allows fraction of satellite name                      */
/*                FALSE only allows exact satellite name                      */
/*                if object number is specified, this flag is without effect  */
/*                                                                            */
/******************************************************************************/

int readTle(satNameFlag,satNameTle,pSatNum,pElementSet,
            pEpochDay,pInclination,pRaan,pEccentricity,
            pArgPerigee,pMeanAnomaly,pMeanMotion,
            pDecayRate,pDecayRateDot,pbStarCoeff,
            pOrbitNum,pEphemerisType)

double *pEpochDay, *pInclination, *pRaan, *pEccentricity, *pArgPerigee;
double *pMeanAnomaly, *pMeanMotion, *pDecayRate, *pDecayRateDot, *pbStarCoeff;
long   *pSatNum, *pOrbitNum;
int    satNameFlag, *pElementSet, *pEphemerisType;
char   *satNameTle;

{
    double epochYear, exponent, atofSatNum, checkLow, checkHigh;
    long   strSatNum;
    long   tleSatNum  = 0L;
    long   saveSatNum = 0L;
    int    error, satNameLen, checkSum, checkValue, takeSatNum;
    int    i, j, m, n;
    int    tleIndex = 0;
    char   line0[80], line1[80], line2[80], str[80], strng[10], saveStr[80];
    char   tleStr[80];

    satNameLen = strlen(satNameTle);
    atofSatNum = atof(satNameTle);

    checkLow   = pow(10.0,(double) (satNameLen - 1)) - ONEPPM;
    checkHigh  = pow(10.0,(double) (satNameLen)) - 1.0 + ONEPPM;

    takeSatNum = (atofSatNum > checkLow && 
                  atofSatNum < checkHigh) ? TRUE : FALSE;

    if (takeSatNum)
        satNameFlag = FALSE;

    upperCase(satNameTle);

    error = FALSE;
    m     = 0;

    for (i = 0; i < numTle; i++)               /* check for ambiguous entries */
    {
        if (m == 1)
        {
            strcpy(saveStr,tleStr);
            saveSatNum = tleSatNum;
        }

        strcpy(str,tle[i].tleLine0);
        upperCase(str);
        strSatNum = tle[i].tleSatNum;

        n = (satNameFlag) ? satNameLen : strlen(str);

        if ((!takeSatNum && n == satNameLen && 
             !strncmp(satNameTle,str, (unsigned int) satNameLen)) || 
            (takeSatNum && (long) (atofSatNum + ONEPPM) == strSatNum))
        {
            strcpy(tleStr,tle[i].tleLine0);
            tleSatNum = tle[i].tleSatNum;

            if (m == 0)
                tleIndex = i;

            if (m == 1)
                printf("\n%5ld:  %s\n",saveSatNum,saveStr);

            if (m >= 1)
                printf("%5ld:  %s\n",tleSatNum,tleStr);

            m++;
        }
    }

    if (m == 0)                                        /* satellite not found */
        return(3);

    if (m > 1)                                         /* multiple entries    */
        return(4);

    strcpy(line0,tle[tleIndex].tleLine0);              /* continue if m = 1   */
    truncBlanks(line0);                                /* (only one entry)    */

    strcpy(satNameTle,line0);
    strcpy(line1,tle[tleIndex].tleLine1);                      /* read line 1 */
    strcpy(line2,tle[tleIndex].tleLine2);                      /* read line 2 */

    if (verboseFlag)
    {
        nl();
        printf("%s\n",line0);
        printf("%s\n",line1);
        printf("%s\n",line2);
        nl();
    }

    for (j = 2; j <= 3; j++)             /* perform CRC test on lines 1 and 2 */
    {
        if (j == 2) sprintf(str,"%s",line1);                /* j = error code */
        if (j == 3) sprintf(str,"%s",line2);

        checkSum = 0;

        for (i = 0; i < 68; i++)
        {
            strng[0]   = str[i];
            strng[1]   = '\0';
            checkValue = atoi(strng);

            if (!strcmp(strng,"-"))
                checkValue = 1;              /* assign check sum value to '-' */

            checkSum += checkValue;
        }

        strng[0] = str[68];
        strng[1] = '\0';

        if (atoi(strng) != checkSum % 10)
        {
            error = j - 1;

            if (verboseFlag)
            {
                doBeep(); nl(); reverseBlink();
                printf(" Check sum error in TLE set, line %d: ",error);
                printf("check sum is %d, should be %d \n",
                    checkSum%10,atoi(strng));
                normal();
            }

            return(error);
        }
    }

    *pSatNum        = (long) (getElement(line1, 3, 7) + ONEPPM);
    *pOrbitNum      = (long) (getElement(line2,64,68) + ONEPPM);
    *pEphemerisType = (int)  (getElement(line1,63,63) + ONEPPM);
    *pElementSet    = (int)  (getElement(line1,66,68) + ONEPPM);

    epochYear       = getElement(line1,19,20);
    *pEpochDay      = getElement(line1,21,32) + epochYear * 1000.0;
    *pInclination   = getElement(line2, 9,16);
    *pRaan          = getElement(line2,18,25);
    *pEccentricity  = getElement(line2,27,33) * 1.0e-7;
    *pArgPerigee    = getElement(line2,35,42);
    *pMeanAnomaly   = getElement(line2,44,51);
    *pMeanMotion    = getElement(line2,53,63);
    *pDecayRate     = getElement(line1,34,43);

    exponent        = getElement(line1,51,52);
    *pDecayRateDot  = getElement(line1,45,50) * pow(10.0,-5.0 + exponent);

    exponent        = getElement(line1,60,61);
    *pbStarCoeff    = getElement(line1,54,59) * pow(10.0,-5.0 + exponent);

    return(0);
}

/******************************************************************************/
/*                                                                            */
/* getElement: returns double of orbital element out of ASCII string          */
/*                                                                            */
/******************************************************************************/

double getElement(gstr,gstart,gstop)

int  gstart, gstop;
char *gstr;

{
    double retval;
    int    k, glength;
    char   gestr[80];

    glength = gstop - gstart + 1;

    for (k = 0; k <= glength; k++)
        gestr[k] = gstr[gstart+k-1];

    gestr[glength] = '\0';

    retval = (double) atof(gestr);
    return(retval);
}

/******************************************************************************/
/*                                                                            */
/* End of function block satelem.c                                            */
/*                                                                            */
/******************************************************************************/
