/******************************************************************************/
/*                                                                            */
/*  Title       : satgraph.c                                                  */
/*  Author      : Manfred Bester                                              */
/*  Date        : 13Dec94                                                     */
/*  Last change : 25Aug95                                                     */
/*                                                                            */
/*  Synopsis    : SatTrack graphics display routines (V3.1.1 patch).          */
/*                                                                            */
/*  This version is fully compatible with V3.1 and has the anti-flicker code  */
/*  implemented that was provided by cjd@aplexus.jhuapl.edu.                  */
/*  Also, this version fixes a problem with SGI machines on which the         */
/*  the default font in the X11 applications is larger than usual. The        */
/*  third fix is about some pointers in the X11 event handlers. These are     */
/*  defined more cleanly now.                                                 */
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

#ifdef XWINDOW

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "satglobalsx.h"
#include "sattrack.h"
#include "mir.h"
#include "shuttle.h"
#include "world.xbm"

/******************************************************************************/
/*                                                                            */
/* definitions                                                                */
/*                                                                            */
/* the first five parameters are imported from the bitmap file 'world.xbm'    */
/*                                                                            */
/******************************************************************************/

#define MAPWIDTH      (unsigned int) world_width              /* [pixels]     */
#define MAPHEIGHT     (unsigned int) world_height             /* [pixels]     */
#define MAPTYPE       (unsigned int) world_type               /* 1 or 2       */
#define MAPWIDTHD     (double) world_widthd                   /* [deg]        */
#define MAPHEIGHTD    (double) world_heightd                  /* [deg]        */

#define GRIDSCALEX    ((double) MAPWIDTH  / MAPWIDTHD)        /* [pixels/deg] */
#define GRIDSCALEY    ((double) MAPHEIGHT / MAPHEIGHTD)       /* [pixels/deg] */

#define MARGINTOP     (unsigned int) 20                       /* black frame  */
#define MARGINBOT     (unsigned int) 20                       /* around the   */
#define MARGINLFT     (unsigned int) 10                       /* world map    */
#define MARGINRGT     (unsigned int) 10                       /* [pixels]     */

#define WINWIDTH      (unsigned int) (MAPWIDTH  + MARGINLFT + MARGINRGT)
#define WINHEIGHT     (unsigned int) (MAPHEIGHT + MARGINTOP + MARGINBOT)
#define HEADERROW     (unsigned int) (MARGINTOP - 5)
#define FOOTERROW     (unsigned int) (MARGINTOP + MAPHEIGHT + 15)

#define NUMFONTS      1

/******************************************************************************/
/*                                                                            */
/* X Window stuff and other global definitions                                */
/*                                                                            */
/******************************************************************************/

static Display            *theDisplay;
static Window             theWindow, frameWindow;
static GC                 theGC;
static Colormap           theColormap;
static XColor             color;
static Drawable           drawable, backgroundDrawable, foregroundDrawable;
static Pixmap             pixmap;
static XSegment           gridZero[2], gridLinesX[NGRIDX*2], 
                          gridLinesY[NGRIDY*2];
static XSegment           visibCircleSatPix[NSEGSVC], 
                          visibCircleGndPix[NSEGSVC];
static XSegment           visibCircleSunPix[NSEGSVC], groundTrackPix[NSEGSGT];
static XPoint             stsPoints[NSTSPOINTS], mirPoints[NMIRPOINTS];
static Widget             topLevel, frame;
static XtAppContext       appContext;
static XEvent             theEvent;
static XtInputMask        mask, lastMask;
static Boolean            eventFlag;
static Arg                warg[3];
static XWindowAttributes  attr;
static XFontStruct        *fontStruct[NUMFONTS];

static double             gndLtd, gndLng, ctyLtd, ctyLng, objEle, 
                          satAzi, satEle, satLtd, satLng, satHgt, 
                          mirLtd, mirLng, mirHgt, tdrsHgt, 
                          sepLtd, sepLng, sepHgt, sunLtd, sunLng, sunHgt, 
                          ltd, lng, hgt, visHgt, dH;

static long               selOrbitNum;

static unsigned long      foregroundPixel, backgroundPixel, 
                          redPixel, greenPixel, lightGreenPixel, 
                          nightBluePixel, darkBluePixel, oceanBluePixel, 
                          lightBluePixel, veryLightBluePixel, 
                          darkYellowPixel, lightYellowPixel, 
                          blackPixel, darkGreyPixel, mediumGreyPixel, 
                          whitePixel; 

static unsigned int       w, h;

static int                x, y, theScreen, depth, nPoints, initFlag, mapSize, 
                          gndSpot, satSpot, tdrsFlag, mirFlag, metFlag, 
                          satFlag, selFlag, dayFlag, orbFlag, lchFlag, craFlag, 
                          nTdrs, nShuttle, gndLocX, gndLocY, ctyLocX, ctyLocY, 
                          satLocX, satLocY, mirLocX, mirLocY, sunLocX, sunLocY, 
                          argC;

static int                failFlag = FALSE;

char                      idStr[80], str[80], str1[120], str2[120], 
                          longStr[120], appName[80], selName[40], tdrsName[40], 
                          *argV[5];


typedef struct {
    XSegment gndCircle[NSEGSVC];
    } groundType;

static groundType gndCircleSave[MAXSTATIONS];


typedef struct {
    XSegment tdrsCircle[NSEGSVC];
    } tdrsType;

static tdrsType tdrsCircleSave[MAXTDRS];

/******************************************************************************/
/*                                                                            */
/* font specifications                                                        */
/*                                                                            */
/******************************************************************************/

char *fontType[] = { "6x13" };

/******************************************************************************/
/*                                                                            */
/* DrawShuttle: draws space shuttle icon                                      */
/*                                                                            */
/*              orbFlag = FALSE  in orbit                                     */
/*              orbFlag = TRUE   not in orbit                                 */
/*                                                                            */
/******************************************************************************/

void DrawShuttle(nStr,selFl,dayFl)

int  selFl, dayFl;
char *nStr;

{
    int i;

    nPoints = sizeof (stsPointsWhite) / sizeof (XPoint);

    for (i = 0; i < nPoints; i++)
    {
        if (!orbFlag)                                  /* on-orbit attitude   */
        {
            stsPoints[i].x =  stsPointsWhite[i].x + (short int) satLocX;
            stsPoints[i].y =  stsPointsWhite[i].y + (short int) satLocY;
        }

        else                                           /* pre-launch attitude */
        {
            stsPoints[i].x =  stsPointsWhite[i].y + (short int) satLocX;
            stsPoints[i].y = -stsPointsWhite[i].x + (short int) satLocY;
        }
    }

    if (orbFlag)
        dayFl = DAY;

    if (dayFl == NIGHT)
        XSetForeground(theDisplay,theGC,mediumGreyPixel);
    else
        XSetForeground(theDisplay,theGC,whitePixel);

    XDrawPoints(theDisplay,drawable,theGC,stsPoints,nPoints,CoordModeOrigin);

    nPoints = sizeof (stsPointsBlack) / sizeof (XPoint);

    for (i = 0; i < nPoints; i++)
    {
        if (!orbFlag)                                  /* on-orbit attitude   */
        {
            stsPoints[i].x =  stsPointsBlack[i].x + (short int) satLocX;
            stsPoints[i].y =  stsPointsBlack[i].y + (short int) satLocY;
        }

        else                                           /* pre-launch attitude */
        {
            stsPoints[i].x =  stsPointsBlack[i].y + (short int) satLocX;
            stsPoints[i].y = -stsPointsBlack[i].x + (short int) satLocY;
        }
    }

    XSetForeground(theDisplay,theGC,blackPixel);
    XDrawPoints(theDisplay,drawable,theGC,stsPoints,nPoints,CoordModeOrigin);

    /* show mission number if several shuttles are in the multisat list */

    if (nShuttle > 1)
    {
        if (!orbFlag)
            x = ((int) (MARGINLFT + MAPWIDTH) - satLocX > 100) 
                         ? 6 : -12 - ((int) (strlen(nStr)) * 6);
        else
            x = 13;

        if (!orbFlag)
            y = ((int) (MARGINTOP + MAPHEIGHT) - satLocY < 25) ? -8 : 20;
        else
            y = -4;

        if (selFl)
            XSetForeground(theDisplay,theGC,lightBluePixel);
        else
            XSetForeground(theDisplay,theGC,lightYellowPixel);

        XDrawString(theDisplay,drawable,theGC,satLocX+x,satLocY+y,
                    nStr,(int) strlen(nStr));
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawMir: draws Mir space station icon                                      */
/*                                                                            */
/******************************************************************************/

void DrawMir(dayFl)

int dayFl;

{
    int i;

    nPoints = sizeof (mirPointsWhite) / sizeof (XPoint);

    for (i = 0; i < nPoints; i++)
    {
        mirPoints[i].x = mirPointsWhite[i].x + (short int) satLocX;
        mirPoints[i].y = mirPointsWhite[i].y + (short int) satLocY;
    }

    if (dayFl == NIGHT)
        XSetForeground(theDisplay,theGC,mediumGreyPixel);
    else
        XSetForeground(theDisplay,theGC,whitePixel);

    XDrawPoints(theDisplay,drawable,theGC,mirPoints,nPoints,CoordModeOrigin);

    nPoints = sizeof (mirPointsBlack) / sizeof (XPoint);

    for (i = 0; i < nPoints; i++)
    {
        mirPoints[i].x = mirPointsBlack[i].x + (short int) satLocX;
        mirPoints[i].y = mirPointsBlack[i].y + (short int) satLocY;
    }

    XSetForeground(theDisplay,theGC,blackPixel);
    XDrawPoints(theDisplay,drawable,theGC,mirPoints,nPoints,CoordModeOrigin);

    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawSat: draws satellite other than Mir or STS                             */
/*                                                                            */
/*          selFl = TRUE    color is blue   (status = SELECT or MARK)         */
/*                = FALSE   color is yellow                                   */
/*                                                                            */
/*          dayFl = NIGHT   color is dark   (blue or yellow)                  */
/*                = ELSE    color is light  (blue or yellow)                  */
/*                                                                            */
/******************************************************************************/

void DrawSat(nStr,selFl,dayFl)

int  selFl, dayFl;
char *nStr;

{
    satSpot = (selFl) ? TRKSPOT : SATSPOT;

    if (selFl && dayFl != NIGHT)
        XSetForeground(theDisplay,theGC,lightBluePixel);

    if (selFl && dayFl == NIGHT)
        XSetForeground(theDisplay,theGC,darkBluePixel);

    if (!selFl && dayFl != NIGHT)
        XSetForeground(theDisplay,theGC,lightYellowPixel);

    if (!selFl && dayFl == NIGHT)
        XSetForeground(theDisplay,theGC,darkYellowPixel);

    XFillRectangle(theDisplay,drawable,theGC,
                   satLocX - (int) satSpot,satLocY - (int) satSpot,
                   (unsigned int) (satSpot*2+1),(unsigned int) (satSpot*2+1));

    x = ((int) (MARGINLFT + MAPWIDTH) - satLocX > 100) 
                 ? 10 : -10 - ((int) (strlen(nStr)) * 6);

    y = (satLocY - (int) MARGINTOP < 25) ? 20 : -8;

    if (selFl)
        XSetForeground(theDisplay,theGC,lightBluePixel);
    else
        XSetForeground(theDisplay,theGC,lightYellowPixel);

    XDrawString(theDisplay,drawable,theGC,satLocX+x,satLocY+y,
                nStr,(int) strlen(nStr));

    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawFrame: draws the black frame around world map and the text in the      */
/*            header and footer line                                          */
/*                                                                            */
/******************************************************************************/

void DrawFrame()

{
    x = 0;
    y = 0;
    w = (unsigned int) (MARGINLFT + MAPWIDTH + MARGINRGT);
    h = (unsigned int) MARGINTOP;

    XSetForeground(theDisplay,theGC,blackPixel);
    XFillRectangle(theDisplay,drawable,theGC,x,y,w,h);

    if (trackObject == SAT)
    {
        sprintf(str,"%s",selName);
        objEle = satEle;
    }

    if (trackObject == SUN)
    {
        sprintf(str,"Sun");
        objEle = sunElevation * CRD;
    }

    if (trackObject == MOON)
    {
        sprintf(str,"Moon");
        objEle = moonElevation * CRD;
    }

    if (trackingFlag)
    {
        if (objEle > ONEPPM)
            XSetForeground(theDisplay,theGC,lightGreenPixel);
        else
            XSetForeground(theDisplay,theGC,redPixel);

        x = 12;

        XFillRectangle(theDisplay,drawable,theGC,
                   (int) (MARGINLFT+1),(int) (MARGINTOP-14),
                   (unsigned int) (9),(unsigned int) (9));
    }

    XSetForeground(theDisplay,theGC,whitePixel);
    XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+2+x),
                (int) HEADERROW,str,(int) strlen(str));

    sprintf(str,"Orbit: %ld",selOrbitNum);

    if (mapSize == SMALL)
        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.24*MAPWIDTH),
                (int) HEADERROW,str,(int) strlen(str));
    else
        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.18*MAPWIDTH),
                (int) HEADERROW,str,(int) strlen(str));

    if (mapSize == LARGE)
        sprintf(str,"Azi: %5.1f  Ele: %5.1f",satAzi,satEle);

    if (mapSize == MEDIUM)
        sprintf(str,"Azi: %3.0f  Ele: %3.0f",satAzi,satEle);

    if (mapSize == LARGE || mapSize == MEDIUM)
        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.33*MAPWIDTH),
               (int) HEADERROW,str,(int) strlen(str));

    if (mapSize == LARGE)
    {
        sprintf(str1,"Lat: %4.1f %s",fabs(satLtd),
               (satLtd >= 0.0) ? "N":"S");
        sprintf(str2,"%s  Lng: %5.1f %s",str1,fabs(satLng),
               (satLng >= 0.0) ? "W":"E");

        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.53*MAPWIDTH),
               (int) HEADERROW,str2,(int) strlen(str2));
    }

    if (mapSize == MEDIUM)
    {
        sprintf(str1,"Lat: %2.0f %s",fabs(satLtd),
               (satLtd >= 0.0) ? "N":"S");
        sprintf(str2,"%s  Lng: %3.0f %s",str1,fabs(satLng),
               (satLng >= 0.0) ? "W":"E");

        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.54*MAPWIDTH),
               (int) HEADERROW,str2,(int) strlen(str2));
    }

    if (mapSize == SMALL)
    {
        sprintf(str1,"Lat: %2.0f %s",fabs(satLtd),
               (satLtd >= 0.0) ? "N":"S");
        sprintf(str2,"%s  Lng: %3.0f %s",str1,fabs(satLng),
               (satLng >= 0.0) ? "W":"E");

        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.43*MAPWIDTH),
               (int) HEADERROW,str2,(int) strlen(str2));
    }

    if (mapSize == LARGE || mapSize == MEDIUM)
    {
        sprintf(str,"%s",xTimeStr);
        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.78*MAPWIDTH),
               (int) HEADERROW,str,(int) strlen(str));
    }

    if (mapSize == SMALL)
    {
        sprintf(str,"%s",xTimeStr);
        XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+0.73*MAPWIDTH),
               (int) HEADERROW,str,(int) strlen(str));
    }

    x = 0;
    y = (int) (MARGINTOP + MAPHEIGHT);
    w = (unsigned int) (MARGINLFT + MAPWIDTH + MARGINRGT);
    h = (unsigned int) MARGINBOT;

    XSetForeground(theDisplay,theGC,blackPixel);
    XFillRectangle(theDisplay,drawable,theGC,x,y,w,h);

    XSetForeground(theDisplay,theGC,whitePixel);
    sprintf(str1,"Ground Track: %s",xGndTrkStr);

    if (metFlag && mapSize == SMALL)
    {
        strncpy(longStr,str1,60);
        shortString(str2,longStr,60);
        strcpy(str1,str2);
    }

    XDrawString(theDisplay,drawable,theGC,(int) (MARGINLFT+2),
                (int) FOOTERROW,str1,(int) strlen(str1));

    if (metFlag)
    {
        sprintf(str,"%s",xMetStr);

        if (mapSize == SMALL)
            XDrawString(theDisplay,drawable,theGC,
                    (int) (MARGINLFT+0.78*MAPWIDTH),
                    (int) FOOTERROW,str,(int) strlen(str));
        else
            XDrawString(theDisplay,drawable,theGC,
                    (int) (MARGINLFT+0.82*MAPWIDTH),
                    (int) FOOTERROW,str,(int) strlen(str));
    }

    x = 0;
    y = (int) MARGINTOP;
    w = (unsigned int) MARGINLFT;
    h = (unsigned int) MAPHEIGHT;

    XSetForeground(theDisplay,theGC,blackPixel);
    XFillRectangle(theDisplay,drawable,theGC,x,y,w,h);

    x = (int) (MARGINLFT + MAPWIDTH);
    y = (int) MARGINTOP;
    w = (unsigned int) MARGINRGT;
    h = (unsigned int) MAPHEIGHT;

    XFillRectangle(theDisplay,drawable,theGC,x,y,w,h);

    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawToBackground: draws to background drawable                             */
/*                                                                            */
/******************************************************************************/

void DrawToBackground()

{
    drawable = backgroundDrawable;
    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawToForeground: draws to foreground drawable                             */
/*                                                                            */
/******************************************************************************/

void DrawToForeground()

{
    drawable = foregroundDrawable;
    return;
}

/******************************************************************************/
/*                                                                            */
/* BackToFore: switches background to foreground                              */
/*                                                                            */
/******************************************************************************/

void BackToFore()

{
    XCopyArea(theDisplay,backgroundDrawable,foregroundDrawable,theGC,
              0,0,WINWIDTH,WINHEIGHT,0,0);
    return;
}

/******************************************************************************/
/*                                                                            */
/* DrawMap: draws graphics window with world map and other features           */
/*                                                                            */
/******************************************************************************/

void DrawMap()

{
    int i, nSats;

    DrawToBackground();

    /* load initial satellite data and check if Mir is in the list */
    /* (the latter is needed for STS-Mir docking maneuvers)        */
    /* also count number of space shuttles in the list             */

    mirFlag  = FALSE;
    metFlag  = FALSE;
    nShuttle = 0;

    if (liveDispFlag == MULTISAT)
    {
        for (i = 0; i < numSats; i++)
        {
            selFlag = sat[i].statusFl;
            satFlag = sat[i].satTypeFl;

            if (selFlag == SELECT)
            {
                lchFlag = sat[i].launchFl;
                orbFlag = sat[i].preorbitFl;
                craFlag = sat[i].crashFl;

                satLtd  = sat[i].satLtd * CRD;;
                satLng  = sat[i].satLng * CRD;;
                satHgt  = sat[i].satHgt;
                satAzi  = sat[i].satAzi * CRD;
                satEle  = sat[i].satEle * CRD;
                ctyLtd  = sat[i].ctyLtd;
                ctyLng  = sat[i].ctyLng;

                tdrsHgt = satHgt;               /* needed for ZOE calculation */

                if (satFlag == STS)
                    selOrbitNum = sat[i].stsOrbNum;
                else
                    selOrbitNum = sat[i].satOrbNum;

                if (objectNumFlag)
                    sprintf(selName,"%ld",sat[i].satIdNum);
                else
                    sprintf(selName,"%s",sat[i].satellite);

                if (lchFlag)
                {
                    strcpy(xMetStr,sat[i].xMetStr);
                    metFlag = (strlen(xMetStr)) ? TRUE : FALSE;
                }
            }

            if (satFlag == MIR)                /* needed for docking maneuver */
            {
                mirLtd  = sat[i].satLtd * CRD;;
                mirLng  = sat[i].satLng * CRD;;
                mirHgt  = sat[i].satHgt;
                mirFlag = TRUE;

                CalcMapLocation(mirLtd,mirLng,&mirLocX,&mirLocY);
            }

            if (satFlag == STS)
                nShuttle++;
        }
    }

    else
    {
        lchFlag = launchFlag;
        orbFlag = preOrbitFlag;
        craFlag = satCrashFlag;

        satLtd  = satLat * CRD;
        satLng  = satLong * CRD;
        satHgt  = satHeight;

        satAzi = satAzimuth * CRD;
        satEle = satElevation * CRD;

        ctyLtd  = cityLat;
        ctyLng  = cityLong;

        metFlag = (strlen(xMetStr) && lchFlag) ? TRUE : FALSE;

        if (satTypeFlag == STS)
            selOrbitNum = stsOrbitNum;
        else
            selOrbitNum = orbitNum;

        sprintf(selName,"%s",satName);
    }

    if (craFlag)
    {
        satLtd = ZERO;
        satLng = ZERO;
        satHgt = ZERO;
        satAzi = ZERO;
        satEle = ZERO;
        selOrbitNum = 0L;
        *xGndTrkStr = '\0';
    }

    sunLtd = sunLat * CRD;
    sunLng = sunLong * CRD;
    sunHgt = sunDist;

    gndLtd = siteLat * CRD;
    gndLng = siteLong * CRD;

    if (debugFlag)
    {
        sunLtd = ZERO;
        sunLng = ZERO;

        groundStation[0].ltd =  0.0 * CDR;
        groundStation[0].lng = 60.0 * CDR;
        sprintf(groundStation[0].gndStnID,"TEST");
    }

    XSetForeground(theDisplay,theGC,foregroundPixel);
    XSetBackground(theDisplay,theGC,backgroundPixel);

    /* draw world map */

    XCopyPlane(theDisplay,(Drawable) pixmap,(Drawable) drawable,theGC,
               (int) 0,(int) 0,
               (unsigned int) MAPWIDTH,(unsigned int) MAPHEIGHT, 
               (int) MARGINLFT,(int) MARGINTOP,(unsigned long) 1);

    /* draw grid */

    XSetForeground(theDisplay,theGC,mediumGreyPixel);
    XDrawSegments(theDisplay,drawable,theGC,gridZero,2);
    XSetForeground(theDisplay,theGC,darkGreyPixel);
    XDrawSegments(theDisplay,drawable,theGC,gridLinesX,NGRIDX*2);
    XDrawSegments(theDisplay,drawable,theGC,gridLinesY,NGRIDY*2);

    /* draw black frame with header and footer lines */

    DrawFrame();

    /* draw ground stations and nearest city if satellite is in orbit */

    dH = fabs(satHgt - lastSatGraphHeight);

    if (!craFlag && !orbFlag)
    {
        /* ground stations */

        for (i = 0; i < numGroundStations; i++)
        {
            gndLtd = groundStation[i].ltd * CRD;
            gndLng = groundStation[i].lng * CRD;

            if (i == 0)
            {
                if (strlen(groundStation[0].gndStnID))
                    strcpy(idStr,groundStation[0].gndStnID);
                else
                    strcpy(idStr,siteName);
            }

            else
                strcpy(idStr,groundStation[i].gndStnID);

            gndSpot = (i == 0) ? GNDSPOT : GNDSPOTX;

            CalcMapLocation(gndLtd,gndLng,&gndLocX,&gndLocY);
            XSetForeground(theDisplay,theGC,redPixel);

            /* recalculate visibility circle of the ground stations only when */
            /* satellite height changes by more than DHLIMIT (1 km)           */

            if (dH > DHLIMIT)
            {
                CalcVisibCircle(&visibCircleGndPix[0],gndLtd,gndLng,satHgt,0.0);
                SaveGroundSegments(&visibCircleGndPix[0],i);
            }

            else
                LoadGroundSegments(&visibCircleGndPix[0],i);

            XDrawSegments(theDisplay,drawable,theGC,visibCircleGndPix,NSEGSVC);
            XFillRectangle(theDisplay,drawable,theGC,
                   gndLocX - (int) gndSpot,gndLocY - (int) gndSpot,
                   (unsigned int) (gndSpot*2+1),(unsigned int) (gndSpot*2+1));

            x = ((int) (MARGINLFT + MAPWIDTH) - gndLocX > 100) 
                         ? 10 : -10 - ((int) (strlen(idStr)) * 6);

            y = (gndLocY - (int) MARGINTOP < 25) ? 20 : -8;

            XSetForeground(theDisplay,theGC,veryLightBluePixel);
            XDrawString(theDisplay,drawable,theGC,gndLocX+x,gndLocY+y,
                        idStr,(int) strlen(idStr));
        }

        /* nearest city */

        CalcMapLocation(ctyLtd,ctyLng,&ctyLocX,&ctyLocY);

        XSetForeground(theDisplay,theGC,redPixel);
        XFillRectangle(theDisplay,drawable,theGC,
                   ctyLocX - (int) CTYSPOT,ctyLocY - (int) CTYSPOT,
                   (unsigned int) (CTYSPOT*2+1),(unsigned int) (CTYSPOT*2+1));
    }

    /* recalculate Sun and terminator when they change by half a pixel */

    if (curTimeGl - lastSunGraphTime > 0.5 / (double) MAPWIDTH)
    {
        CalcMapLocation(sunLtd,sunLng,&sunLocX,&sunLocY);
        CalcVisibCircle(&visibCircleSunPix[0],sunLtd,sunLng,sunHgt,0.0);
        lastSunGraphTime = curTimeGl;
    }

    /* draw Sun and terminator */

    XSetForeground(theDisplay,theGC,lightYellowPixel);
    XDrawSegments(theDisplay,drawable,theGC,visibCircleSunPix,NSEGSVC);
    sprintf(str,"*");
    XDrawString(theDisplay,drawable,theGC,sunLocX-2,sunLocY+5,
                str,(int) strlen(str));

    /* draw ground track */

    if (!initFlag && !craFlag)
    {
        if (newGndTrkFlag)
        {
            CalcGroundTrack();
            newGndTrkFlag = FALSE;
        }

        XSetForeground(theDisplay,theGC,lightBluePixel);
        XDrawSegments(theDisplay,drawable,theGC,groundTrackPix,NSEGSGT);
    }

    /* draw all satellites */

    nSats = (liveDispFlag == MULTISAT) ? numSats : 1;
    nTdrs = 0;

    for (i = 0; i < nSats; i++)
    {
        if (nSats > 1)
        {
            satFlag = sat[i].satTypeFl;
            selFlag = sat[i].statusFl;
            dayFlag = sat[i].eclCode;
            orbFlag = sat[i].preorbitFl;
            craFlag = sat[i].crashFl;

            ltd     = sat[i].satLtd * CRD;
            lng     = sat[i].satLng * CRD;
            hgt     = sat[i].satHgt;

            if (objectNumFlag)
                sprintf(str,"%ld",sat[i].satIdNum);
            else
                sprintf(str,"%s",sat[i].satellite);

            sprintf(tdrsName,"%s",sat[i].satellite);
        }

        else
        {
            satFlag = satTypeFlag;
            selFlag = SELECT;
            dayFlag = eclipseCode;
            orbFlag = preOrbitFlag;
            craFlag = satCrashFlag;

            ltd     = satLat * CRD;
            lng     = satLong * CRD;
            hgt     = satHeight;
            tdrsHgt = 0.0;

            sprintf(str,"%s",satName);
        }

        /* check if satellite to be drawn is TDRS */

        tdrsFlag = (!strncmp(tdrsName,"TDRS",4)) ? TRUE : FALSE;

        if ((selFlag && !orbFlag) || (!selFlag && tdrsFlag))
        {
            visHgt = (tdrsFlag) ? tdrsHgt : 0.0;

            if (tdrsFlag && nTdrs < MAXTDRS)   /* displayed satellite is TDRS */
            {
                /* TDRS is selected: show foot print of TDRS on the ground    */

                if (selFlag) 
                {
                    XSetForeground(theDisplay,theGC,veryLightBluePixel);
                    CalcVisibCircle(&visibCircleSatPix[0],ltd,lng,hgt,0.0);
                }

                /* TDRS is not selected: show visibility as seen from other   */
                /* selected satellite                                         */

                else
                {
                    XSetForeground(theDisplay,theGC,mediumGreyPixel);

                    if (dH > DHLIMIT)
                    {
                        CalcVisibCircle(&visibCircleSatPix[0],
                                        ltd,lng,hgt,visHgt);
                        SaveTdrsSegments(&visibCircleSatPix[0],nTdrs);
                    }

                    else
                        LoadTdrsSegments(&visibCircleSatPix[0],nTdrs);
                }

                nTdrs++;
            }

            else /* displayed satellite is not TDRS */
            {
                XSetForeground(theDisplay,theGC,veryLightBluePixel);
                CalcVisibCircle(&visibCircleSatPix[0],ltd,lng,hgt,visHgt);
            }

            XDrawSegments(theDisplay,drawable,theGC,visibCircleSatPix,NSEGSVC);
        }

        if (!craFlag)
        {
            CalcMapLocation(ltd,lng,&satLocX,&satLocY);

            if (satFlag == BASIC)
                DrawSat(str,selFlag,dayFlag);

            if (satFlag == MIR)
                DrawMir(dayFlag);

            if (satFlag == STS)
            {
                if (mirFlag)
                {
                    sepLng = reduce((lng-mirLng),0.0,360.0);
                    sepLtd = fabs(ltd - mirLtd);
                    sepHgt = fabs(hgt - mirHgt);

                    /* if separation of STS and Mir is less than 5 deg and */
                    /* the height difference is less than 5 km:            */
                    /* show Mir and STS docked together                    */

                    if ((sepLng < 5.0 || sepLng > 355.0) && sepLtd < 5.0 && 
                         sepHgt < 10.0)
                    {
                        satLocX = mirLocX -  8;
                        satLocY = mirLocY + 16;
                    }
                }

                DrawShuttle(str,selFlag,dayFlag);
            }
        }
    }

    if (dH > DHLIMIT)
        lastSatGraphHeight = satHgt;

    BackToFore();
    XFlush(theDisplay);
    DrawToForeground();

    initFlag = FALSE;
    return;
}

/******************************************************************************/
/*                                                                            */
/* CreateColors: creates all colors needed                                    */
/*                                                                            */
/******************************************************************************/

void CreateColors()

{
    theColormap = DefaultColormap(theDisplay,theScreen);
    color.flags = DoRed | DoGreen | DoBlue;

    color.red   = (unsigned short) 65535;                              /* red */
    color.green = (unsigned short) 20000;
    color.blue  = (unsigned short) 25000;

    XAllocColor(theDisplay,theColormap,&color);
    redPixel = color.pixel;

    color.red   = (unsigned short) 18000;                            /* green */
    color.green = (unsigned short) 48000;
    color.blue  = (unsigned short)  2000;

    XAllocColor(theDisplay,theColormap,&color);
    greenPixel = color.pixel;

    color.red   = (unsigned short) 18000;                      /* light green */
    color.green = (unsigned short) 52000;
    color.blue  = (unsigned short)  2000;

    XAllocColor(theDisplay,theColormap,&color);
    lightGreenPixel = color.pixel;

    color.red   = (unsigned short)  5000;                       /* night blue */
    color.green = (unsigned short)  5000;
    color.blue  = (unsigned short) 35000;

    XAllocColor(theDisplay,theColormap,&color);
    nightBluePixel = color.pixel;

    color.red   = (unsigned short) 34000;                        /* dark blue */
    color.green = (unsigned short) 34000;
    color.blue  = (unsigned short) 52000;

    XAllocColor(theDisplay,theColormap,&color);
    darkBluePixel = color.pixel;

    color.red   = (unsigned short) 12500;                       /* ocean blue */
    color.green = (unsigned short)  7500;
    color.blue  = (unsigned short) 50000;

    XAllocColor(theDisplay,theColormap,&color);
    oceanBluePixel = color.pixel;

    color.red   = (unsigned short) 40000;                       /* light blue */
    color.green = (unsigned short) 60000;
    color.blue  = (unsigned short) 65535;

    XAllocColor(theDisplay,theColormap,&color);
    lightBluePixel = color.pixel;

    color.red   = (unsigned short) 62000;                  /* very light blue */
    color.green = (unsigned short) 65535;
    color.blue  = (unsigned short) 65535;

    XAllocColor(theDisplay,theColormap,&color);
    veryLightBluePixel = color.pixel;

    color.red   = (unsigned short) 52000;                      /* dark yellow */
    color.green = (unsigned short) 52000;
    color.blue  = (unsigned short) 34000;

    XAllocColor(theDisplay,theColormap,&color);
    darkYellowPixel = color.pixel;

    color.red   = (unsigned short) 65535;                     /* light yellow */
    color.green = (unsigned short) 65535;
    color.blue  = (unsigned short) 35000;

    XAllocColor(theDisplay,theColormap,&color);
    lightYellowPixel = color.pixel;

    color.red   = (unsigned short)     0;                            /* black */
    color.green = (unsigned short)     0;
    color.blue  = (unsigned short)     0;

    XAllocColor(theDisplay,theColormap,&color);
    blackPixel = color.pixel;

    color.red   = (unsigned short) 40000;                        /* dark grey */
    color.green = (unsigned short) 40000;
    color.blue  = (unsigned short) 45000;

    XAllocColor(theDisplay,theColormap,&color);
    darkGreyPixel = color.pixel;

    color.red   = (unsigned short) 48000;                      /* medium grey */
    color.green = (unsigned short) 48000;
    color.blue  = (unsigned short) 52000;

    XAllocColor(theDisplay,theColormap,&color);
    mediumGreyPixel = color.pixel;

    color.red   = (unsigned short) 65535;                            /* white */
    color.green = (unsigned short) 65535;
    color.blue  = (unsigned short) 65535;

    XAllocColor(theDisplay,theColormap,&color);
    whitePixel = color.pixel;

    return;
}

/******************************************************************************/
/*                                                                            */
/* CalcGrid: calculates grid                                                  */
/*                                                                            */
/******************************************************************************/

void CalcGrid()

{
    double gridStepX, gridStepY;
    int    i, j, mapCenterX, mapCenterY;

    mapCenterX     = (int) (MARGINLFT + MAPWIDTH  / 2 - 1);
    mapCenterY     = (int) (MARGINTOP + MAPHEIGHT / 2 - 1);

    gridZero[0].x1 = mapCenterX;                             /* zero meridian */
    gridZero[0].x2 = mapCenterX;                 
    gridZero[0].y1 = (int) MARGINTOP;
    gridZero[0].y2 = (int) (MARGINTOP + MAPHEIGHT - 1);
    
    gridZero[1].x1 = (int) MARGINLFT;                              /* equator */
    gridZero[1].x2 = (int) (MARGINLFT + MAPWIDTH - 1);
    gridZero[1].y1 = mapCenterY;
    gridZero[1].y2 = mapCenterY;
    
    gridStepX      = GRIDSTEPX * GRIDSCALEX;
    gridStepY      = GRIDSTEPY * GRIDSCALEY;

    for (i = 0; i < NGRIDX; i++)                                  /* vertical */
    {
        j = 2 * i;

        gridLinesX[j].x1   = mapCenterX + (int) ((double) (i+1) * gridStepX);
        gridLinesX[j].x2   = mapCenterX + (int) ((double) (i+1) * gridStepX);

        gridLinesX[j+1].x1 = mapCenterX - (int) ((double) (i+1) * gridStepX);
        gridLinesX[j+1].x2 = mapCenterX - (int) ((double) (i+1) * gridStepX);

        gridLinesX[j].y1   = (int) MARGINTOP;
        gridLinesX[j].y2   = (int) (MARGINTOP + MAPHEIGHT - 1);

        gridLinesX[j+1].y1 = (int) MARGINTOP;
        gridLinesX[j+1].y2 = (int) (MARGINTOP + MAPHEIGHT - 1);
    }

    for (i = 0; i < NGRIDY; i++)                                /* horizontal */
    {
        j = 2 * i;

        gridLinesY[j].x1   = (int) MARGINLFT;
        gridLinesY[j].x2   = (int) (MARGINLFT + MAPWIDTH - 1);

        gridLinesY[j+1].x1 = (int) MARGINLFT;
        gridLinesY[j+1].x2 = (int) (MARGINLFT + MAPWIDTH - 1);

        gridLinesY[j].y1   = mapCenterY + (int) ((double) (i+1) * gridStepY);
        gridLinesY[j].y2   = mapCenterY + (int) ((double) (i+1) * gridStepY);

        gridLinesY[j+1].y1 = mapCenterY - (int) ((double) (i+1) * gridStepY);
        gridLinesY[j+1].y2 = mapCenterY - (int) ((double) (i+1) * gridStepY);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* CalcMapLocation: calculates location of ground station, satellite and Sun  */
/*                                                                            */
/******************************************************************************/

void CalcMapLocation(mapLtd,mapLng,mapX,mapY)

double mapLtd, mapLng;
int    *mapX, *mapY;

{
    *mapX  = (int) ((MAPWIDTHD  / 2.0 - mapLng) * GRIDSCALEX - 0.5);
    *mapY  = (int) ((MAPHEIGHTD / 2.0 - mapLtd) * GRIDSCALEY - 0.5);

    *mapX += (int) MARGINLFT;
    *mapY += (int) MARGINTOP;

    return;
}

/******************************************************************************/
/*                                                                            */
/* CalcVisibCircle: calculates circle of visibility around sub-satellite      */
/*                  point, using an Euler matrix for the vector rotation      */
/*                                                                            */
/******************************************************************************/

void CalcVisibCircle(visibCirclePix,circLtd,circLng,circHgt,exHgt)

XSegment *visibCirclePix;
double   circLtd, circLng, circHgt, exHgt;

{
    double descMat[3][3], u[3], q[3];
    double arg, circStep, qabs;
    double the, psi, gamma, gammax, beta, lambda, lat, lng;
    double cosThe, sinThe, cosPsi, sinPsi;
    double cosBeta, sinBeta, cosLambda, sinLambda;

    int    i, k, posX, posY;

    the    = HALFPI - circLtd*CDR;
    psi    = HALFPI - circLng*CDR;

    cosThe = cos(the);
    sinThe = sin(the);
    cosPsi = cos(psi);
    sinPsi = sin(psi);

    descMat[0][0] =  sinPsi*cosThe;
    descMat[0][1] = -cosPsi;
    descMat[0][2] =  sinPsi*sinThe;
    descMat[1][0] =  cosPsi*cosThe;
    descMat[1][1] =  sinPsi;
    descMat[1][2] =  cosPsi*sinThe;
    descMat[2][0] = -sinThe;
    descMat[2][1] =  0.0;
    descMat[2][2] =  cosThe;

    arg  = EARTHRADIUS / (EARTHRADIUS + circHgt);

    if (arg >  1.0) arg =  1.0;
    if (arg < -1.0) arg = -1.0;

    gamma = acos(arg);

    if (exHgt > ONEPPM)                         /* for calculation of the ZOE */
    {
        arg = EARTHRADIUS / (EARTHRADIUS + exHgt);

        if (arg >  1.0) arg =  1.0;
        if (arg < -1.0) arg = -1.0;

        gammax = acos(arg);
        gamma += gammax;
    }

    beta     = HALFPI - gamma;
    cosBeta  = cos(beta);
    sinBeta  = sin(beta);

    circStep = 360.0 / ((double) NSEGSVC);

    for (k = 0; k < NSEGSVC;  k++)
    {
        lambda    = (double) (circStep * k) * CDR;
        cosLambda = cos(lambda);
        sinLambda = sin(lambda);

        u[0] = cosLambda*cosBeta;
        u[1] = sinLambda*cosBeta;
        u[2] = sinBeta;

        multMatVec(u,q,descMat);

        qabs  = absol(q);

        for (i = 0; i <= 2; i++)
            q[i] /= qabs;

        arg   = q[2];

        if (arg >  1.0) arg =  1.0;
        if (arg < -1.0) arg = -1.0;

        lat   = asin(arg) * CRD;

        q[2]  = 0.0;
        qabs  = absol(q);

        q[0] /= qabs;
        q[1] /= qabs;

        lng   = atan2(q[1],q[0]) * CRD;
        lng   = reduce(lng,-MAPWIDTHD/2.0,MAPWIDTHD/2.0);

        posX  = (int) ((MAPWIDTHD  / 2.0 - lng) * GRIDSCALEX + MARGINLFT - 0.5);
        posY  = (int) ((MAPHEIGHTD / 2.0 - lat) * GRIDSCALEY + MARGINTOP - 0.5);

        visibCirclePix[k].x1 = (short int) posX;
        visibCirclePix[k].y1 = (short int) posY;

        if (k > 0)
        {
            visibCirclePix[k-1].x2 = visibCirclePix[k].x1;
            visibCirclePix[k-1].y2 = visibCirclePix[k].y1;
        }
    }

    visibCirclePix[NSEGSVC-1].x2 = visibCirclePix[0].x1;      /* close circle */
    visibCirclePix[NSEGSVC-1].y2 = visibCirclePix[0].y1;

    CleanSegments(&visibCirclePix[0],NSEGSVC,TRUE);

    return;
}

/******************************************************************************/
/*                                                                            */
/* SaveGroundSegments: saves segments of visibility circle for all ground     */
/*                     stations                                               */
/*                                                                            */
/******************************************************************************/

void SaveGroundSegments(xSegments,num)

XSegment *xSegments;
int num;

{
    int i;

    for (i = 0; i < NSEGSVC; i++)
    {
        gndCircleSave[num].gndCircle[i].x1 = xSegments[i].x1;
        gndCircleSave[num].gndCircle[i].x2 = xSegments[i].x2;
        gndCircleSave[num].gndCircle[i].y1 = xSegments[i].y1;
        gndCircleSave[num].gndCircle[i].y2 = xSegments[i].y2;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* LoadGroundSegments: loads segments of visibility circle for all ground     */
/*                     stations                                               */
/*                                                                            */
/******************************************************************************/

void LoadGroundSegments(xSegments,num)

XSegment *xSegments;
int num;

{
    int i;

    for (i = 0; i < NSEGSVC; i++)
    {
        xSegments[i].x1 = gndCircleSave[num].gndCircle[i].x1;
        xSegments[i].x2 = gndCircleSave[num].gndCircle[i].x2;
        xSegments[i].y1 = gndCircleSave[num].gndCircle[i].y1;
        xSegments[i].y2 = gndCircleSave[num].gndCircle[i].y2;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* SaveTdrsSegments: saves segments of visibility circle for TDRS satellites  */
/*                                                                            */
/******************************************************************************/

void SaveTdrsSegments(xSegments,num)

XSegment *xSegments;
int num;

{
    int i;

    for (i = 0; i < NSEGSVC; i++)
    {
        tdrsCircleSave[num].tdrsCircle[i].x1 = xSegments[i].x1;
        tdrsCircleSave[num].tdrsCircle[i].x2 = xSegments[i].x2;
        tdrsCircleSave[num].tdrsCircle[i].y1 = xSegments[i].y1;
        tdrsCircleSave[num].tdrsCircle[i].y2 = xSegments[i].y2;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* LoadTdrsSegments: loads segments of visibility circle for TDRS satellites  */
/*                                                                            */
/******************************************************************************/

void LoadTdrsSegments(xSegments,num)

XSegment *xSegments;
int num;

{
    int i;

    for (i = 0; i < NSEGSVC; i++)
    {
        xSegments[i].x1 = tdrsCircleSave[num].tdrsCircle[i].x1;
        xSegments[i].x2 = tdrsCircleSave[num].tdrsCircle[i].x2;
        xSegments[i].y1 = tdrsCircleSave[num].tdrsCircle[i].y1;
        xSegments[i].y2 = tdrsCircleSave[num].tdrsCircle[i].y2;
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* CalcGroundTrack: calculates ground track in pixels                         */
/*                                                                            */
/******************************************************************************/

void CalcGroundTrack()

{
    int  k, gndX, gndY;

    for (k = 0; k < NSEGSGT; k++)
    {
        gndX = (int) ((MAPWIDTHD/2.0  - groundTrack[k].lng) * GRIDSCALEX + 
                       MARGINLFT - 0.5);

        gndY = (int) ((MAPHEIGHTD/2.0 - groundTrack[k].ltd) * GRIDSCALEY + 
                       MARGINTOP - 0.5);

        groundTrackPix[k].x1 = (short int) gndX;
        groundTrackPix[k].y1 = (short int) gndY;

        if (k > 0)
        {
            groundTrackPix[k-1].x2 = groundTrackPix[k].x1;
            groundTrackPix[k-1].y2 = groundTrackPix[k].y1;
        }
    }

    groundTrackPix[NSEGSGT-1].x2 = groundTrackPix[NSEGSGT-1].x1;
    groundTrackPix[NSEGSGT-1].y2 = groundTrackPix[NSEGSGT-1].y1;

    CleanSegments(&groundTrackPix[0],NSEGSGT,FALSE);

    return;
}

/******************************************************************************/
/*                                                                            */
/* CleanSegments: cleans up line segments (wrap around at map boundaries)     */
/*                                                                            */
/******************************************************************************/

void CleanSegments(xSegments,numSegments,visibCircleFlag)

XSegment *xSegments;
int      numSegments, visibCircleFlag;

{
    int k, k0, n, dx, dy, dX, dY;

    k0 = (visibCircleFlag) ?  0 : -1;

    for (k = 0; k < numSegments + k0; k++)
    {
        n = (visibCircleFlag && k == numSegments - 1) ? 0 : k+1;

        if (xSegments[k].x1 - xSegments[k].x2 > (short int) (MAPWIDTH / 2))
        {
            dx = (int) (MAPWIDTH + MARGINLFT) - (int) xSegments[k].x1;
            dX = (int) xSegments[k].x2 - (int) MARGINLFT + dx;

            dY = (int) (xSegments[k].y2 - xSegments[k].y1);

            if (dX != 0)
                dy = (int) ((double) dY / (double) dX * (double) dx);
            else
                dy = 0;

            xSegments[n].x1 = xSegments[k].x2 - (short int) (dX - dx);
            xSegments[k].x2 = xSegments[k].x1 + (short int) dx;

            xSegments[n].y1 = xSegments[k].y2 - (short int) (dY - dy);
            xSegments[k].y2 = xSegments[k].y1 + (short int) dy;
        }

        if (xSegments[k].x2 - xSegments[k].x1 > (short int) (MAPWIDTH / 2))
        {
            dx = (int) xSegments[k].x1 - (int) MARGINLFT;
            dX = (int) (MAPWIDTH + MARGINLFT) - (int) xSegments[k].x2 + dx;

            dY = (int) (xSegments[k].y2 - xSegments[k].y1);

            if (dX != 0)
                dy = (int) ((double) dY / (double) dX * (double) dx);
            else
                dy = 0;

            xSegments[n].x1 = xSegments[k].x2 + (short int) (dX - dx);
            xSegments[k].x2 = xSegments[k].x1 - (short int) dx;

            xSegments[n].y1 = xSegments[k].y2 - (short int) (dY - dy);
            xSegments[k].y2 = xSegments[k].y1 + (short int) dy;
        }
    }

    for (k = 0; k < numSegments; k++)              /* clean up left and right */
    {
        if (xSegments[k].x1 < (short int) MARGINLFT)
            xSegments[k].x1 = (short int) MARGINLFT;

        if (xSegments[k].x2 < (short int) MARGINLFT)
            xSegments[k].x2 = (short int) MARGINLFT;

        if (xSegments[k].x1 > (short int) (MAPWIDTH + MARGINLFT - 1))
            xSegments[k].x1 = (short int) (MAPWIDTH + MARGINLFT - 1);

        if (xSegments[k].x2 > (short int) (MAPWIDTH + MARGINLFT - 1))
            xSegments[k].x2 = (short int) (MAPWIDTH + MARGINLFT - 1);
    }

    for (k = 0; k < numSegments; k++)              /* clean up top and bottom */
    {
        if (xSegments[k].y1 < (short int) MARGINTOP)
            xSegments[k].y1 = (short int) MARGINTOP;

        if (xSegments[k].y2 < (short int) MARGINTOP)
            xSegments[k].y2 = (short int) MARGINTOP;

        if (xSegments[k].y1 > (short int) (MAPHEIGHT + MARGINTOP - 1))
            xSegments[k].y1 = (short int) (MAPHEIGHT + MARGINTOP - 1);

        if (xSegments[k].y2 > (short int) (MAPHEIGHT + MARGINTOP - 1))
            xSegments[k].y2 = (short int) (MAPHEIGHT + MARGINTOP - 1);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* main program                                                               */
/*                                                                            */
/******************************************************************************/

int StartGraphics()

{
    int n = 0;

    if (failFlag)
    {
        dispMessage("Attempt to open graphics window failed before.");
        return(FALSE);
    }

    initFlag = TRUE;

    if (firstGraphics)
    {
        lastSatGraphHeight = -2.0 * DHLIMIT;
        lastSunGraphTime   = ZERO;
        numGroundStations  = 1;

        argV[0] = "sattrack";
        argC    = 1;

        sprintf(appName,"%s",graphicsTitle);
 
        XtToolkitInitialize();
        appContext = XtCreateApplicationContext();
    }

    theDisplay = XtOpenDisplay(appContext,NULL,appName,appName,NULL,0,
                               &argC,argV);

    if (!theDisplay)
    {
        dispMessage("Attempt to open graphics window failed.");
        failFlag = TRUE;
        return(FALSE);
    }

    theScreen = DefaultScreen(theDisplay);
    topLevel  = XtAppCreateShell(appName,appName,
                    applicationShellWidgetClass,theDisplay,NULL,0);

    if (firstGraphics)
    {
        CreateColors();
        CalcGrid();

        mapSize = (MAPWIDTH > (unsigned int) 700) ? LARGE   : MEDIUM;
        mapSize = (MAPWIDTH > (unsigned int) 550) ? mapSize : SMALL;

        if (MAPTYPE == 1)                                         /* fill map */
        {
            foregroundPixel = greenPixel;
            backgroundPixel = oceanBluePixel;

/* 
            foregroundPixel = oceanBluePixel;
            backgroundPixel = nightBluePixel;
*/
        }

        else                                                      /* line map */
        {
            foregroundPixel = lightGreenPixel;
            backgroundPixel = nightBluePixel;
        }

        n = 0;
        XtSetArg(warg[n],XtNwidth,WINWIDTH); n++;
        XtSetArg(warg[n],XtNheight,WINHEIGHT); n++;
        XtSetArg(warg[n],XtNbackground,backgroundPixel); n++;
    }

    frame = XtCreateManagedWidget("frame",widgetClass,topLevel,warg,n);

    XtAppAddTimeOut(appContext,1L,
                    (XtTimerCallbackProc) TimeEvent,(caddr_t) NULL);

    XtAddEventHandler(frame,StructureNotifyMask,FALSE,
                      (XtEventHandler) ResizeMap,(caddr_t) NULL);
    XtAddEventHandler(frame,VisibilityChangeMask,FALSE,
                      (XtEventHandler) RedrawMap,(caddr_t) NULL);
    XtAddEventHandler(frame,ExposureMask,FALSE,
                      (XtEventHandler) RedrawMap,(caddr_t) NULL);

    XtRealizeWidget(topLevel);

    theWindow = XtWindow(topLevel);
    theGC     = XCreateGC(theDisplay,theWindow,0,0);
    drawable  = XtWindow(frame);

    fontStruct[0] = XLoadQueryFont(theDisplay,fontType[0]);

    if (fontStruct[0] == 0)
    {
        dispMessage("Cannot load font '%s'.",fontType[0]);
        failFlag = TRUE;
        return(FALSE);
    }

    XSetFont(theDisplay,theGC,fontStruct[0]->fid);

    foregroundDrawable = drawable;

    frameWindow = XtWindow(frame);

    if (XGetWindowAttributes(theDisplay,frameWindow,&attr))
        depth = attr.depth;
    else
        depth = DefaultDepth(theDisplay,DefaultScreen(theDisplay));

    backgroundDrawable = XCreatePixmap(theDisplay,drawable,
                                       WINWIDTH,WINHEIGHT,depth);

    pixmap    = XCreateBitmapFromData(theDisplay,(Drawable) drawable,
                                      (char *) world_bits,MAPWIDTH,MAPHEIGHT);

    XMapRaised(theDisplay,(Drawable) drawable);
    XClearWindow(theDisplay,theWindow);

    UpdateGraphics(TRUE);                                  /* initial display */

    firstGraphics = FALSE;
    return(TRUE);
}

/******************************************************************************/
/*                                                                            */
/* RedrawMap: redraws map                                                     */
/*                                                                            */
/******************************************************************************/

void RedrawMap()

{
    DrawMap();
    return;
}

/******************************************************************************/
/*                                                                            */
/* ResizeMap: resizes map                                                     */
/*                                                                            */
/******************************************************************************/

void ResizeMap()

{
    DrawMap();
    return;
}

/******************************************************************************/
/*                                                                            */
/* TimeEvent: performs update of the map                                      */
/*                                                                            */
/******************************************************************************/

void TimeEvent(i,id)

int i;
XtIntervalId *id;

{
    DrawMap();
    return;
}

/******************************************************************************/
/*                                                                            */
/* CreateGraphicsTimeOut: sets graphics timer                                 */
/*                                                                            */
/******************************************************************************/

void CreateGraphicsTimeOut()

{
    XtAppAddTimeOut(appContext,1L,TimeEvent,0);
    return;
}

/******************************************************************************/
/*                                                                            */
/* UpdateGraphics: updates graphics display                                   */
/*                                                                            */
/******************************************************************************/

void UpdateGraphics(strtFlag)

int strtFlag;

{
    int  nEvents;
    char qStr[800], qqStr[800];

    lastMask = (unsigned long) 0;
    nEvents  = 0;

    if (strtFlag)
        milliSleep(100);

    sprintf(qStr,"X11 event queue (%d):",strtFlag);

    /* check the top of the event queue in order to find out */
    /* if an event is pending                                */
    /* the code within the 'while' loop is not optimized yet */

    while ((mask = XtAppPending(appContext)) != (unsigned long) 0)
    {
        strcpy(qqStr,qStr);
        sprintf(qStr,"%s %ld",qqStr,(long) mask);

        /* the next line gets the next event structure and dispatches */
        /* the event automatically already if it's a timer event      */

        XtAppNextEvent(appContext,&theEvent);

        /* if the event is an exposure or visibility change event and */
        /* it has not been dispatched yet, dispatch it now            */

        if (mask != lastMask && mask != (unsigned long) 2)
        {
            eventFlag = XtDispatchEvent(&theEvent);
            lastMask  = mask;
        }

        nEvents++;
    }

    if (nEvents > 0)
    {
        if (nEvents > 1)
        {
            strcpy(qqStr,qStr);
            sprintf(qStr,"%s (%d events)",qqStr,nEvents);
        }

        if (debugFlag)
            dispMessage(qStr);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* QuitGraphics: quits graphics display                                       */
/*                                                                            */
/******************************************************************************/

void QuitGraphics()

{
    XUnloadFont(theDisplay,fontStruct[0]->fid);
    XFreePixmap(theDisplay,pixmap);
    XFreePixmap(theDisplay,backgroundDrawable);
    XFreeGC(theDisplay,theGC);
    XDestroyWindow(theDisplay,theWindow);
    XCloseDisplay(theDisplay);

    return;
}

/******************************************************************************/
/*                                                                            */
/* dummy routines (these are used when SatTrack is compiled without X Window) */
/*                                                                            */
/******************************************************************************/

#else

int StartGraphics()
{ return(99); }

void CreateGraphicsTimeOut()
{ return; }

void UpdateGraphics(dummyI)
int dummyI;
{ return; }

void QuitGraphics()
{ return; }

#endif

/******************************************************************************/
/*                                                                            */
/* End of function block satgraph.c                                           */
/*                                                                            */
/******************************************************************************/
