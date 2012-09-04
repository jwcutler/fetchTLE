/******************************************************************************/
/*                                                                            */
/*  Title       : satvect.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 15Aug93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Auxiliary math routines for the satellite tracking program  */
/*                'sattrack'.                                                 */
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
/* absol: calculates length (absolute value) of a vector in 3D                */
/*                                                                            */
/******************************************************************************/

double absol(absVec)

double absVec[3];

{
    double absVal;

    absVal = sqrt(SQR(absVec[0]) + SQR(absVec[1]) + SQR(absVec[2]));

    return(absVal);
}

/******************************************************************************/
/*                                                                            */
/* cross: calculates vector product of two vectors in 3D                      */
/*                                                                            */
/*        for right-handed cross products 'vecSign' has to be set to '1'      */
/*                                                                            */
/******************************************************************************/

void cross(xVec,yVec,pcrVec,vecSign)

double xVec[3], yVec[3], (*pcrVec)[3];
int    vecSign;

{
    (*pcrVec)[0] = (xVec[1]*yVec[2] - xVec[2]*yVec[1]) * (double) vecSign;
    (*pcrVec)[1] = (xVec[2]*yVec[0] - xVec[0]*yVec[2]) * (double) vecSign;
    (*pcrVec)[2] = (xVec[0]*yVec[1] - xVec[1]*yVec[0]) * (double) vecSign;

    return;
} 

/******************************************************************************/
/*                                                                            */
/* multMatVec: multiplies a matrix with a vector in 3D                        */
/*                                                                            */
/******************************************************************************/

void multMatVec(amVec,pbmVec,mtx)

double amVec[3], (*pbmVec)[3], mtx[3][3];

{
    int i;

    for (i = 0; i <= 2; i++)
    {
        (*pbmVec)[i] = mtx[i][0]*amVec[0] + 
                       mtx[i][1]*amVec[1] +
                       mtx[i][2]*amVec[2];
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* scalar: calculates scalar product of two vectors in 3D                     */
/*                                                                            */
/******************************************************************************/

double scalar(asVec,bsVec)

double asVec[3], bsVec[3];

{
    double scalarProd;

    scalarProd = asVec[0]*bsVec[0] + asVec[1]*bsVec[1] + asVec[2]*bsVec[2];

    return(scalarProd);
}

/******************************************************************************/
/*                                                                            */
/* End of function block satvect.c                                            */
/*                                                                            */
/******************************************************************************/
