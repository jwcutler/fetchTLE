/******************************************************************************/
/*                                                                            */
/*  Title       : sattime.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 04Dec91                                                     */
/*  Last change : 06Jan96                                                     */
/*                                                                            */
/*  Synopsis    : This function block contains auxiliary routines dealing     */
/*                with time and date.                                         */
/*                V3.1.2 patch.                                               */
/*                                                                            */
/*                                                                            */
/*  SatTrack is Copyright (c) 1992, 1993, 1994, 1995, 1996 Manfred Bester.    */
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
#include <ctype.h>                                  /* needed for system time */
#include <sys/types.h>                              /* needed for system time */
#include <sys/time.h>                               /* needed for sleep timer */
#include <signal.h>                                 /* needed for sleep timer */
#include <memory.h>                                 /* needed for sleep timer */

#ifndef TIME                                        /* NOT included on Sun-3  */
#include <time.h>                                   /* with SunOS 4.x.x       */
#endif

#ifndef STDLIB                                      /* NOT included on Sun-4  */
#include <stdlib.h>                                 /* with SunOS 4.x.x       */
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* global fields                                                              */
/*                                                                            */
/******************************************************************************/

int monthDays[]  = { 0,31,59,90,120,151,181,212,243,273,304,334,365 };
int noLeapYear[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
int leapYear[]   = { 0,31,29,31,30,31,30,31,31,30,31,30,31 };

/******************************************************************************/
/*                                                                            */
/* getUnixTime: reads Unix system time                                        */
/*                                                                            */
/******************************************************************************/

void getUnixTime(day,month,year,yday,hour,min,sec)

int *day, *month, *year, *yday, *hour, *min, *sec;

{
    char   timeString[80];

    time_t timeofday;
    struct tm *tm;

    time(&timeofday);
    tm = gmtime(&timeofday);
    strncpy(timeString,asctime(tm),16);
    timeString[16] = '\0';

    *day   = tm->tm_mday;
    *month = tm->tm_mon + 1;
    *year  = tm->tm_year;
    *yday  = tm->tm_yday + 1;
    *hour  = tm->tm_hour;
    *min   = tm->tm_min;
    *sec   = tm->tm_sec;

    if (*year > 99) *year = *year - 100;	//jwc, Y2K fix.


    return;
}

/******************************************************************************/
/*                                                                            */
/* getSiderealTime: calculates sidereal time                                  */
/*                  for references see:                                       */
/*                  "The Astronomical Almanac", 1986, page B6, B7 and L2      */
/*                  "The Astronomical Almanac", 1984, page S16                */
/*                                                                            */
/*                  gmsTime    = Greenwich mean sidereal time                 */
/*                  gasTime    = Greenwich apparent sidereal time             */
/*                  lasTime    = local apparent sidereal time                 */
/*                  equEquinox = equation of the equinoxes                    */
/*                                                                            */
/******************************************************************************/

void getSiderealTime(timeArg)

double timeArg;

{
    double tu, tusq, tucb, utcInterval, julianDay, rss;

    utcInterval = modf(timeArg,&dummyd);                               /* [d] */

    julianDate  = timeArg + JULDAT1900 - 0.5;                          /* [d] */
    julianDay   = julianDate - utcInterval;                            /* [d] */

    tu          = (julianDay - JULDAT2000) / JULCENT;                /* [jcy] */
    tusq        = tu*tu;
    tucb        = tu*tusq;

    gmsTime     = 24110.54841 + 8640184.812866*tu + 0.093104*tusq - 6.2e-6*tucb;

    tu          = (julianDate - JULDAT2000) / JULCENT;               /* [jcy] */
    tusq        = tu*tu;
    tucb        = tu*tusq;

    rss         = 1.002737909350795 + 5.9006e-11*tu - 5.9e-15*tusq;

    gmsTime    += rss * utcInterval * SPD;                           /* [sec] */
    gmsTime    *= TWOPI / SPD;                                       /* [rad] */
    gmsTime     = reduce(gmsTime,ZERO,TWOPI);                        /* [rad] */

    getNutation();

    gasTime    = gmsTime + equEquinox;                               /* [rad] */
    gasTime    = reduce(gasTime,ZERO,TWOPI);                         /* [rad] */

    lasTime    = gasTime - siteLong;                                 /* [rad] */
    lasTime    = reduce(lasTime,ZERO,TWOPI);

    return;
}

/******************************************************************************/
/*                                                                            */
/* calendar: transforms the format of the calendar date                       */
/*                                                                            */
/******************************************************************************/

void calendar(iyear,yearday,pmonth,pday)

int iyear, yearday, *pmonth, *pday;

{
    int month, day, check;

    month = 0;

    if (iyear%100 == 0 && iyear%400 != 0)
          check = 1;
    else
          check = iyear%4;

    if (check == 0)
    {
        while (yearday > 0)
        {
             month++; 
             yearday -= leapYear[month];
        }

        day = yearday + leapYear[month];
    } 

    else
    {
        while (yearday > 0)
        {
             month++;
             yearday -= noLeapYear[month];
        } 

        day = yearday + noLeapYear[month];
    }

    *pmonth = month;
    *pday   = day;

    return;
}

/******************************************************************************/
/*                                                                            */
/* dayName: returns the name of the nth day                                   */
/*                                                                            */
/******************************************************************************/

char *dayName(dDay)

int dDay;

{
    return((dDay < 0 || dDay > 6) ? "XXX" : dayNames[dDay]);
}

/******************************************************************************/
/*                                                                            */
/* monthName: returns the name of the nth month                               */
/*                                                                            */
/******************************************************************************/

char *monthName(mMonth)

int mMonth;

{
    return((mMonth < 1 || mMonth > 12) ? "XXX" : monthNames[mMonth-1]);
}

/******************************************************************************/
/*                                                                            */
/* getDayNum: gets the day number for a given date. Jan. 1 of the reference   */
/*            year is day 0. Note that the day number may be negative, if the */
/*            sidereal reference is in the future.                            */
/*                                                                            */
/*            January 1, 1900 is day 0 valid from 1950 through 2049           */
/*                                                                            */
/******************************************************************************/

long getDayNum(gdnD,gdnM,gdnY)

int gdnD, gdnM, gdnY;

{
    long result;
    
    if (gdnY < 50)                  /* allow 4 or 2 digit year specifications */
        gdnY += 2000;
    else
        if (gdnY < 100)
            gdnY += 1900;

    result = (long) ((((gdnY-1901)*1461) >> 2) + monthDays[gdnM-1] + gdnD+365);

    if (gdnY%4 == 0 && gdnM > 2)          /* correct day number for leap year */
        result++;

    return(result);
}

/******************************************************************************/
/*                                                                            */
/* getDate: gets the date from a given day number (see getDayNum)             */
/*                                                                            */
/******************************************************************************/

void getDate(dayNum,year,month,day,yearDay)

long dayNum;
int  *year, *month, *day, *yearDay;

{
    int gdLD, gdYD, gdD, gdM, gdY;

    gdY   = (int) ((double) (4 * dayNum) / 1461.0);
    gdYD  = (int) (dayNum - (long) (365 + (((gdY - 1) * 1461) >> 2)));
    gdY  += 1900;
    gdLD  = (gdY%4 == 0 && gdYD > monthDays[2]) ? 1 : 0;
    gdM   = 0;

    do
    {
        gdM++;
    }
    while (gdYD > monthDays[gdM] + gdLD);

    gdD = gdYD - monthDays[gdM-1];

    if (gdM > 2)
        gdD -= gdLD;

    *year    = gdY;
    *month   = gdM;
    *day     = gdD;
    *yearDay = gdYD;

    return;
}    

/******************************************************************************/
/*                                                                            */
/* decodeDate: decodes date string                                            */
/*                                                                            */
/******************************************************************************/

int decodeDate(dateStr,pday,pmonth,pyear)

int  *pday, *pmonth, *pyear;
char *dateStr;

{
    int  i, error;
    int  d = 0;
    int  m = 0;
    int y = 0;
    char checkStr[10], monthStr[10];

    error = TRUE;

    if (strlen(dateStr) == 7)
    {
        d = -1;
        m = -1;
        y = -1;

        sscanf(dateStr,"%2d%3s%2d",&d,monthStr,&y);

        for (i = 1; i <= 12; i++)
        {
            sprintf(checkStr,"%s",monthName(i));
            upperCase(checkStr);
            upperCase(monthStr);

            if (!strcmp(checkStr,monthStr))
            {
                m = i;

                if (d >= 1 && d <= 31 && y >= 0 && y <= 99)
                    error = FALSE;
            }
        }
    }

    *pday   = d;
    *pmonth = m;
    *pyear  = y;

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* decodeTime: decodes time string                                            */
/*                                                                            */
/******************************************************************************/

int decodeTime(timeStr,phour,pmin,psec)

int  *phour, *pmin, *psec;
char *timeStr;

{
    int h, m, s, error;

    error = TRUE;

    h = -1;
    m = -1;
    s = -1;

    sscanf(timeStr,"%d:%d:%d",&h,&m,&s);

    if (h >= 0 && h <= 23 && m >= 0 && m <= 59 && s >= 0 && s <= 59)
        error = FALSE;

    *phour = h;
    *pmin  = m;
    *psec  = s;

    return(error);
}

/******************************************************************************/
/*                                                                            */
/* convertTime: converts format of time from decimal to integer               */
/*                                                                            */
/******************************************************************************/

void convertTime(time,days,hours,mins,secs)

double time;
int    *days, *hours, *mins, *secs;

{
    double timed, timeh, timem, times, timems;

    timed  = time;
    timeh  = 24.0*modf(timed,&timed);
    timem  = 60.0*modf(timeh,&timeh);
    times  = 60.0*modf(timem,&timem);
    timems = 1000.0*modf(times,&times);

    if (timems >= 500)
        times += 1.0;

    if (fabs(times - 60.0) < 5.0e-4)
    {
        times  = 0.0;
        timem += 1.0;

        if (fabs(timem - 60.0) < 1.0e-4)
        {
            timem  = 0.0;
            timeh += 1.0;

            if (fabs(timeh - 24.0) < 1.0e-4)
            {
                timeh  = 0.0;
                timed += 1.0;
            }
        }
    }

    *days  = (int) timed;
    *hours = (int) timeh;
    *mins  = (int) timem;
    *secs  = (int) times;

    return;
}

/******************************************************************************/
/*                                                                            */
/* printDate: prints date into output file                                    */
/*                                                                            */
/******************************************************************************/

void printDate(outFile,time)

double time;
FILE   *outFile;

{
    long dayNum;
    int  day, month, year, yearDay;

    dayNum = (long) time;

    getDate(dayNum,&year,&month,&day,&yearDay);

    fprintf(outFile,"%d-%d-%d$",day,month,year%100);

    lastDayNum = dayNum;
    return;
}    

/******************************************************************************/
/*                                                                            */
/* printTime: prints time string                                              */
/*                                                                            */
/******************************************************************************/

void printTime(outFile,time)

double time;
FILE   *outFile;

{
    double dayTime;
    long   dayNum;
    int    hours, mins, secs;

    dayNum  = (long) time;
    dayTime = time - (double) dayNum;

    convertTime(dayTime,&dummyi,&hours,&mins,&secs);

    if (time < 1000.0)                                /* for duration of pass */
        hours += (int) dayNum * 24;

    fprintf(outFile,"%02d:%02d:%02d",hours,mins,secs);
    return;
}

/******************************************************************************/
/*                                                                            */
/* printMET: prints MET string into output file                               */
/*                                                                            */
/******************************************************************************/

void printMET(outFile,met)

double met;
FILE   *outFile;

{
    double metDayTime;
    long   metDayNum;
    int    negativeMET, metDay, metHour, metMin, metSec;

    metDayNum   = (long) met;
    metDayTime  = met - (double) metDayNum;

    negativeMET = (met < 0.0) ? TRUE : FALSE;

    if (negativeMET)
        met *= -1.0;

    convertTime(met,&metDay,&metHour,&metMin,&metSec);

    if (negativeMET)
    {
        if (metDay)
            fprintf(outFile,"%3d/%02d:%02d:%02d",
                             -metDay,metHour,metMin,metSec);
        else
            fprintf(outFile," -%d/%02d:%02d:%02d",
                             -metDay,metHour,metMin,metSec);
    }

    else
        fprintf(outFile,"%3d/%02d:%02d:%02d",metDay,metHour,metMin,metSec);

    return;
}

/******************************************************************************/
/*                                                                            */
/* milliSleep: sleeps for a specified time in milliseconds                    */
/*                                                                            */
/******************************************************************************/

static int timerDone;

void milliSleep(msec)

int msec;

{
    long usec;
    struct itimerval it;

    if (msec > 999)                      /* works only below 1 second         */
        msec = 999;

    usec = (long) (msec * 1000);

    signal(SIGALRM,onAlarm);             /* make sure event handler is in     */
                                         /* place before timer gets set       */
 
    HOLDSIG;                             /* block timer so that ALRM does not */
                                         /* occur between 'if (timerDone)'    */
                                         /* and calling PAUSESIG              */
 
    timerDone = FALSE;
 
    memset(&it,0,sizeof(it));
    it.it_value.tv_usec = usec;
    setitimer(ITIMER_REAL,&it,(struct itimerval *)0);

    while (TRUE)
    {
        if (timerDone)
            break;
        else
            PAUSESIG;                    /* this is the sleep function        */
    }

    RELEASESIG;                          /* turn ALRM blocking off again      */

    signal(SIGALRM,SIG_DFL);
    return;
}

/******************************************************************************/
/*                                                                            */
/* onAlarm: turns off timer when expired                                      */
/*                                                                            */
/******************************************************************************/

void onAlarm()

{
    timerDone = TRUE;
    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block sattime.c                                            */
/*                                                                            */
/******************************************************************************/
