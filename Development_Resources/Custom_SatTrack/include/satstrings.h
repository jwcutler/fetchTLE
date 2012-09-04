/******************************************************************************/
/*                                                                            */
/*  Title       : satstrings.h                                                */
/*  Author      : Manfred Bester                                              */
/*  Date        : 30May94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : ASCII strings for 'sattrack'.                               */
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

/******************************************************************************/
/*                                                                            */
/* global fields                                                              */
/*                                                                            */
/******************************************************************************/

char sattrName[]     = "SatTrack";

char sattrVersion[]  = "V3.1";

char sattrHeader[]   = "Satellite Tracking Program";

char predHeader[]    = "Orbit Prediction";

char dispHeader[]    = "TRACKING DISPLAY";

char graphHeader[]   = "Graphic Tracking Display";

char *visibCode[]    = { "D", "P", "N", "V", " " };

char *dayNames[]     = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

char *monthNames[]   = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
                         "Aug", "Sep", "Oct", "Nov", "Dec" };

/******************************************************************************/
/*                                                                            */
/* help strings                                                               */
/*                                                                            */
/******************************************************************************/

#ifdef XWINDOW

char singleSatHelp[] = 
       "t T s m r g x X c < > - + 0 1 2 ? q CR";

char multiSatHelp[]  = 
       "t T a f d D u U j J k K l L n o s m r x X c < > - + g e E ? q CR";

#else

char singleSatHelp[] = 
       "t T s m r x X c < > - + 0 1 2 ? q CR";

char multiSatHelp[]  = 
       "t T a f d D u U j J k K l L n o s m r x X c < > - + ? q CR";

#endif

/******************************************************************************/
/*                                                                            */
/* time zone information                                                      */
/*                                                                            */
/* 'timeZoneList' contains the acronyms for the time zones and 'timeZoneHour' */
/* the time difference between the particular time zone and UTC               */
/*                                                                            */
/* ATTENTION: if other time zones are added, make sure that the number of     */
/*            entries in 'timeZoneList' and 'timeZoneHour' are EQUAL!         */
/*                                                                            */
/*            the acronyms can be up to 4 characters long,                    */
/*            western time zones are counted NEGATIVE!                        */
/*                                                                            */
/*            UTC  = Universal Time Coordinated                               */
/*            CEST = Central European Standard Time                           */
/*            CEDT = Central European Daylight Savings Time                   */
/*            EEST = Eastern European Standard Time                           */
/*            JST  = Japanese Standard Time                                   */
/*            EEDT = Eastern European Daylight Savings Time                   */
/*            EST  = Eastern Standard Time (USA)                              */
/*            EDT  = Eastern Daylight Savings Time (USA)                      */
/*            CST  = Central Standard Time (USA)                              */
/*            CDT  = Central Daylight Savings Time (USA)                      */
/*            MST  = Mountain Standard Time (USA)                             */
/*            MDT  = Mountain Daylight Savings Time (USA)                     */
/*            PST  = Pacific Standard Time (USA)                              */
/*            PDT  = Pacific Daylight Savings Time (USA)                      */
/*            HST  = Hawaiian Standard Time (USA)                             */
/*                                                                            */
/******************************************************************************/

char   *timeZoneList[] = {  "UTC",  "CEST", "CEDT", "EEST", "EEDT",  "JST",
                            "EST",   "EDT",  "CST",  "CDT",  "MST",  "MDT", 
                            "PST",   "PDT",  "HST" };

double timeZoneHour[]  = {    0.0,     1.0,    2.0,    2.0,    3.0,    9.0,
                             -5.0,    -4.0,   -6.0,   -5.0,   -7.0,   -6.0, 
                             -8.0,    -7.0,  -10.0 };

/******************************************************************************/
/*                                                                            */
/* End of include file satstrings.h                                           */
/*                                                                            */
/******************************************************************************/
