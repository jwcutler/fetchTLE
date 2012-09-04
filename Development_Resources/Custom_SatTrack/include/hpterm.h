/******************************************************************************/
/*                                                                            */
/*  Title       : hpterm.h                                                    */
/*  Author      : Charlie Panek                                               */
/*  Date        : 09Dec93                                                     */
/*  Last change : 14Mar95                                                     */
/*                                                                            */
/*  Synopsis    : This include file contains the hpterm display macros for    */
/*                the satellite tracking program 'sattrack'.                  */
/*                                                                            */
/*  Adapted from vt100.h by Manfred Bester                                    */
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
/*         escape sequence: ESC&amcnR     n, m = row, column                  */
/*                                                                            */
/******************************************************************************/

#define gotoXY(x,y) printf("\033&a%dc%dR",x-1,y-1)

/******************************************************************************/
/*                                                                            */
/* upCurs: moves cursor up by n lines                                         */
/*         command: ESC&a-0c-nR                                               */
/*                                                                            */
/******************************************************************************/

#define upCurs(n) printf("\033&a-0c-%dR",n)

/******************************************************************************/
/*                                                                            */
/* downCurs: moves cursor down by n lines                                     */
/*           command: ESC&a-0c+nR                                             */
/*                                                                            */
/******************************************************************************/

#define downCurs(n) printf("\033&a-0c+%dR",n)

/******************************************************************************/
/*                                                                            */
/* advCurs: advances cursor by n blanks in the same line                      */
/*          command: ESC&a+nc+0R                                              */
/*                                                                            */
/******************************************************************************/

#define advCurs(n) printf("\033&a+%dc+0R",n)

/******************************************************************************/
/*                                                                            */
/* backCurs: moves cursor backward by n blanks in the same line               */
/*           command: ESC&a-nc+0R                                             */
/*                                                                            */
/******************************************************************************/

#define backCurs(n) printf("\033&a-%dc+0R",n)

/******************************************************************************/
/*                                                                            */
/* clearCurs: clear the line from cursor position                             */
/*            command for clearing the rest of a line: ESCK                   */
/*                                                                            */
/******************************************************************************/

#define clearCurs() printf("\033K")

/******************************************************************************/
/*                                                                            */
/* clearLine: clear the line from cursor position (see also gotoXY())         */
/*            this function is gotoXY() and clearcurs() in one                */
/*                                                                            */
/******************************************************************************/

#define clearLine(x,y) printf("\033&a%dc%dR\033K",x-1,y-1)

/******************************************************************************/
/*                                                                            */
/* clearScreen: clears entire screen from cursor position                     */
/*                                                                            */
/******************************************************************************/

#define clearScreen() printf("\033J")

/******************************************************************************/
/*                                                                            */
/* underline: switches terminal (hpterm) into underline mode                  */
/*            turn on underline option: ESC&dD                                */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define underline() printf("\033&dD")
#else
#define underline()
#endif

/******************************************************************************/
/*                                                                            */
/* reverse: switches terminal (hpterm) into reverse mode                      */
/*          turn on reverse option: ESC&dB                                    */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define reverse() printf("\033&dB")
#else
#define reverse()
#endif

/******************************************************************************/
/*                                                                            */
/* reverseBlink: switches terminal (hpterm) into reverse blink mode           */
/*               turn on reverse and blink option: ESC&dC                     */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define reverseBlink() printf("\033&dC")
#else
#define reverseBlink()
#endif

/******************************************************************************/
/*                                                                            */
/* normal: switches terminal (hpterm) into normal mode, i.e. turn off         */
/*         reverse and blink mode                                             */
/*         turn off reverse and blink option: ESC&d@                          */
/*                                                                            */
/******************************************************************************/

#ifdef REVERSEVIDEO
#define normal() printf("\033&d@")
#else
#define normal()
#endif

/******************************************************************************/
/*                                                                            */
/* clearScr: erases screen from cursor to end                                 */
/*                                                                            */
/******************************************************************************/

#define clearScr() printf("\033J")

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
/* End of include file hpterm.h                                               */
/*                                                                            */
/******************************************************************************/
