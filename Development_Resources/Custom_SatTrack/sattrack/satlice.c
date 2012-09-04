/******************************************************************************/
/*                                                                            */
/*  Title       : satlice.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 10Feb95                                                     */
/*  Last change : 16Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This function block deals with the license issues.          */
/*                Please do not alter these functions!                        */
/*                                                                            */
/*                Instead of tampering with this module, look into the        */
/*                documentation to find out how to suppress the               */
/*                'registration via email'.                                   */
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
/* checkLicense: don't tamper with this function since this might cause       */
/*               trouble elsewhere!                                           */
/*                                                                            */
/******************************************************************************/

void checkLicense()

{
    int  rFlag;
    char rStr[80];

    sprintf(licenseFile,"%s/.sattrack",strpRgst);

    if ((fpr = fopen(licenseFile,"r")) != NULL)
    {
        fclose(fpr);
        return;
    }

    sprintf(sattrackTitle,"%s %s --- License #%d",sattrName,sattrVersion,LCNUM);
    printf("\n%s %s\n",sattrName,sattrVersion);
    printf("\nCopyright (c) 1992, 1993, 1994, 1995 by Manfred Bester.\n");
    printf("All Rights Reserved.\n");

    reverseBlink();
    printf("\n Congratulations! \n");
    normal();
    printf("\nYou acquired and successfully installed %s.\n\n",sattrackTitle);

    printf("There is no license required for private, non-commercial use.\n");
    printf("However, if you intend to use this software or parts of it\n");
    printf("commercially, please contact the author.\n\n");

    printf("Even for non-commercial applications it would make sense to\n");
    printf("register your copy of SatTrack with the author, so that you\n");
    printf("can be informed about future upgrades or fixes of bugs.\n\n");

    doBeep();
    rFlag = FALSE;

    while (!rFlag)
    {
        printf("Would you like to register your copy of SatTrack now (y/n) ? ");
        mGets(rStr,80,stdin);
        upperCase(rStr);

        if (!strncmp(rStr,"N",1) || !strncmp(rStr,"Y",1))
            rFlag = TRUE;
    }

    if (!strncmp(rStr,"Y",1))
    {
        rFlag = FALSE;

        while (!rFlag)
        {
            printf("Can your computer send electronic mail   ");
            printf("            (y/n) ? ");
            mGets(rStr,80,stdin);
            upperCase(rStr);

            if (!strncmp(rStr,"N",1) || !strncmp(rStr,"Y",1))
                rFlag = TRUE;
        }

        if (!strncmp(rStr,"Y",1))
        {
            if ((fpr = fopen(licenseFile,"w")) == NULL)
            {
                printf("Cannot create license file.\n\n");
                exit(-1);
            }

            else
            {
                fprintf(fpr,"Registration of %s\n",sattrackTitle);
                fclose(fpr);
            }

            printf("\nTrying to send registration via email ...\n");
            mailRegistration();
            printf("\nThanks for your registration!\n\n");
        }

        else
            printf("\nCannot send registration via email!\n\n");

        printf("Starting SatTrack any moment ...\n");
        milliSleep(950);
    }

    else
    {
        printf("\nWell, maybe some other time.\n");
        printf("\nStarting SatTrack any moment ...\n");
        milliSleep(950);
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* mailRegistration: sends registration message                               */
/*                                                                            */
/******************************************************************************/

void mailRegistration()

{
    char mStr[120];

    sprintf(mStr,"mail %s < %s",REGMAIL,licenseFile);
    system(mStr);
    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satlice.c                                            */
/*                                                                            */
/******************************************************************************/
