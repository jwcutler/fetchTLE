/******************************************************************************/
/*                                                                            */
/*  Title       : vt100.h                                                     */
/*  Author      : Manfred Bester                                              */
/*  Date        : 04May92                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This include file contains the VT100 display macros for     */
/*                the satellite tracking program 'sattrack'.                  */
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
/* doBeep: generates a beep as warning                                        */
/*                                                                            */
/******************************************************************************/

#ifdef NOBEEP
#define doBeep()
#else
#define doBeep() putchar('\007')
#endif

/******************************************************************************/
/*                                                                            */
/* gotoXY: cursor positioning, go to column "x" and row "y"                   */
/*         escape sequence: ESC[n;mH     n, m = row, column                   */
/*                                                                            */
/******************************************************************************/

#define gotoXY(x,y) printf("\033[%d;%dH",y,x)

/******************************************************************************/
/*                                                                            */
/* upCurs: moves cursor up by n lines                                         */
/*         command: ESC[nA   printf("\033[nA")                                */
/*                                                                            */
/******************************************************************************/

#define upCurs(n) printf("\033[%dA",n)

/******************************************************************************/
/*                                                                            */
/* downCurs: moves cursor down by n lines                                     */
/*           command: ESC[nB   printf("\033[nB")                              */
/*                                                                            */
/******************************************************************************/

#define downCurs(n) printf("\033[%dB",n)

/******************************************************************************/
/*                                                                            */
/* advCurs: advances cursor by n blanks in the same line                      */
/*          command: ESC[nC   printf("\033[nC")                               */
/*                                                                            */
/******************************************************************************/

#define advCurs(n) printf("\033[%dC",n)

/******************************************************************************/
/*                                                                            */
/* backCurs: moves cursor backward by n blanks in the same line               */
/*           command: ESC[nD   printf("\033[nD")                              */
/*                                                                            */
/******************************************************************************/

#define backCurs(n) printf("\033[%dD",n)

/******************************************************************************/
/*                                                                            */
/* clearCurs: clear the line from cursor position                             */
/*            command for clearing the rest of a line:                        */
/*            ESC[K    printf("\033[K")                                       */
/*                                                                            */
/******************************************************************************/

#define clearCurs() printf("\033[K")

/******************************************************************************/
/*                                                                            */
/* clearLine: clear the line from cursor position (see also gotoXY())         */
/*            this function is gotoXY() and clearcurs() in one                */
/*                                                                            */
/******************************************************************************/

#define clearLine(x,y) printf("\033[%d;%dH\033[K",y,x)

/******************************************************************************/
/*                                                                            */
/* clearScreen: clears entire screen from cursor position                     */
/*                                                                            */
/******************************************************************************/

#define clearScreen() printf("\033[J")

/******************************************************************************/
/*                                                                            */
/* underline: switches terminal (VT100) into underline mode                   */
/*            turn on underline option: ESC[4m                                */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define underline() printf("\033[4m")
#else
#define underline()
#endif

/******************************************************************************/
/*                                                                            */
/* reverse: switches terminal (VT100) into reverse mode                       */
/*          turn on reverse option: ESC[7m                                    */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define reverse() printf("\033[7m")
#else
#define reverse()
#endif

/******************************************************************************/
/*                                                                            */
/* reverseBlink: switches terminal (VT100) into reverse blink mode            */
/*               turn on reverse and blink option: ESC[5;7m                   */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define reverseBlink() printf("\033[5;7m")
#else
#define reverseBlink()
#endif

/******************************************************************************/
/*                                                                            */
/* normal: switches terminal (VT100) into normal mode, i.e. turn off          */
/*         reverse and blink mode                                             */
/*         turn off reverse and blink option: ESC[0m                          */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define normal() printf("\033[0m")
#else
#define normal()
#endif

/******************************************************************************/
/*                                                                            */
/* nl: prints 'new line' character                                            */
/*                                                                            */
/******************************************************************************/

#define nl() putchar('\n')

/******************************************************************************/
/*                                                                            */
/* bl: prints 'blank' character                                               */
/*                                                                            */
/******************************************************************************/

#define bl() putchar(' ')

/******************************************************************************/
/*                                                                            */
/* End of include file vt100.h                                                */
/*                                                                            */
/******************************************************************************/
