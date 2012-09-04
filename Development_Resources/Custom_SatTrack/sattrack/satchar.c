/******************************************************************************/
/*                                                                            */
/*  Title       : satchar.c                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 09Jul93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Routines for character string manipulation.                 */
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
#include <string.h>

#ifndef STDLIB
#include <stdlib.h>
#endif

#include "satglobalsx.h"
#include "sattrack.h"

/******************************************************************************/
/*                                                                            */
/* mGets: Manfred's version of fgets (wipes out newline character at the end) */
/*                                                                            */
/*        if the graphics window is open this function also refreshes the     */
/*        graphics if necessary                                               */
/*                                                                            */
/******************************************************************************/

void mGets(string)

char *string;

{
    int  i, k;

    if (graphicsOpenFlag)
    {
        i = 0;

        do
        {
            k = checkKeyboard(TRUE);

            if (k > 0)
            {
                if (k == 8)
                {
                    if (i > 0)
                    {
                        printf("%c%c%c",(char) 8,(char) 8, (char) 8);
                        printf("   ");
                        printf("%c%c%c",(char) 8,(char) 8, (char) 8);
                        fflush(stdout);
                        i--;
                    }

                    else
                    {
                        printf("%c%c",(char) 8,(char) 8);
                        printf("  ");
                        printf("%c%c",(char) 8,(char) 8);
                        fflush(stdout);
                    }
                }

                else
                {
                    string[i] = (char) k;
                    i++;
                }
            }

            if (graphicsOpenFlag)
                UpdateGraphics(FALSE);

            milliSleep(10);
        }
        while (k != 10 && k != 13);

        string[i] = '\0';
    }

    else
        fgets(string,80,stdin);

    i = (int) strlen(string);

    if (i > 0)
        string[i-1] = '\0';
    else
        string[0]   = '\0';

    return;
}

/******************************************************************************/
/*                                                                            */
/* upperCase: changes lower to upper case letters                             */
/*                                                                            */
/******************************************************************************/

void upperCase(string)

char *string;

{
    int i;

    for (i = 0; i < (int) strlen(string); i++)
    {
        if (string[i] >= 'a' && string[i] <= 'z')
            string[i] -= 'a' - 'A';
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* lowerCase: changes upper to lower case letters                             */
/*                                                                            */
/******************************************************************************/

void lowerCase(string)

char *string;

{
    int i;

    for (i = 0; i < (int) strlen(string); i++)
    {
        if (string[i] >= 'A' && string[i] <= 'Z')
            string[i] -= 'A' - 'a';
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* truncBlanks: truncates trailing blanks from character string               */
/*                                                                            */
/******************************************************************************/

void truncBlanks(string)

char *string;

{
    int i;

    i = (int) strlen(string) - 1;

    while (string[i] == ' ')
    {
        i--;
    }

    string[i+1] = '\0';                          /* add termination character */
    return;
}

/******************************************************************************/
/*                                                                            */
/* shortString: shortens character string up to next blank from the end       */
/*                                                                            */
/******************************************************************************/

void shortString(strng,fullStrng,maxLen)

int  maxLen;
char *strng, *fullStrng;

{
    int i;

    strncpy(strng,fullStrng,maxLen);

    if ((int) strlen(strng) == maxLen)
    {
        if (fullStrng[maxLen-1] == ' ')
            strng[maxLen-1] = '\0';

        else
        {
            if (fullStrng[maxLen] != ',' && fullStrng[maxLen] != ' ')
            {
                i = (int) strlen(strng) - 1;

                do
                {
                    i--;
                }
                while (strng[i] != ' ');

                strng[i] = '\0';
            }
        }

        for (i = 0; i < maxLen; i++)
        {
            if (strng[i] == ',')
                strng[i] = '\0';
        }
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* clipString: clips N characters plus leading blanks off the beginning of a  */
/*             character string                                               */
/*                                                                            */
/******************************************************************************/

void clipString(charStr,N)

int  N;
char *charStr;

{
    int  i, j, firstChar;
    char tmpStr[100];

    firstChar = FALSE;
    j = 0;

    for (i = 0; i < (int) strlen(charStr) - N; i++) 
    {

        if (charStr[i+N] != ' ' || (charStr[i+N] == ' ' && firstChar))
        {
            tmpStr[j] = charStr[i+N];
            j++;
            firstChar = TRUE;
        }
    }

    tmpStr[j] = '\0';                            /* add termination character */
    sprintf(charStr,"%s",tmpStr);
    return;
}

/******************************************************************************/
/*                                                                            */
/* cleanSatName: cleans up satellite name                                     */
/*                                                                            */
/******************************************************************************/

void cleanSatName(charStr)

char *charStr;

{
    int  i;

    if ((int) strlen(charStr) > MAXSATNAMELEN)
        charStr[MAXSATNAMELEN] = '\0';

    if (charStr[MAXSATNAMELEN-1] == '/' ||
        charStr[MAXSATNAMELEN-1] == ' ')
        charStr[MAXSATNAMELEN-1] = '\0';

    for (i = 0; i < (int) strlen(charStr); i++)
    {
        if (charStr[i] == '(')
            charStr[i] = '\0';
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* replaceBlanks: replaces underscore characters by a blank character         */
/*                                                                            */
/******************************************************************************/

void replaceBlanks(charStr)

char *charStr;

{
    int  i;

    for (i = 0; i < (int) strlen(charStr); i++)
    {
        if (charStr[i] == '_')
            charStr[i] = ' ';
    }

    return;
}

/******************************************************************************/
/*                                                                            */
/* getSiteNameID: retrieves site ID from site name string                     */
/*                                                                            */
/*                the site ID has to be in '()' and cannot be the last part   */
/*                of the site name; in other words there has to be a comma or */
/*                a blank behind the closing bracket of the site ID           */
/*                                                                            */
/*                allowed examples:                                           */
/*                                                                            */
/*                Diego Garcia (DGS) (UK)                --> DGS              */
/*                Kennedy Space Center (KSC), FL, USA    --> KSC              */
/*                                                                            */
/******************************************************************************/

void getSiteNameID(string)

char *string;

{
    int i, j, k;
    char str[80];

    i = 0;
    j = 0;
    k = (int) strlen(string);

    do
    {
       i++;
    }
    while (string[i] != '(' && i < k);

    i++;

    do
    {
        str[j] = string[i];
        i++;
        j++;
    }
    while (string[i] != ')' && i < k);

    str[j] = '\0';

    if (string[i] != ',' && string[i] != ' ' && i < k-1)
        sprintf(string,"%s",str);
    else
        *string = '\0';

    return;
}

/******************************************************************************/
/*                                                                            */
/* End of function block satchar.c                                            */
/*                                                                            */
/******************************************************************************/
