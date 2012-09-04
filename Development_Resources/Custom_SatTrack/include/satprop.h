/******************************************************************************/
/*                                                                            */
/*  Title       : satprop.h                                                   */
/*  Author      : Manfred Bester                                              */
/*  Date        : 06Dec93                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Definitions of constants for 'satprop.c'                    */
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

#include "sattrack.h"

#define AE       1.0
#define AE2      (AE * AE)
#define AE3      (AE2 * AE)
#define AE4      (AE2 * AE2)

#define KE       0.0743669161         /* (KEPLER/EARTHRADIUS)^3/2 * TWOPI/MPD */

#define Q0       120.0
#define S0       78.0

#define Q0MS0T4  (pow(((Q0 - S0) * AE / EARTHRADIUS),4.0))
#define S        (AE * (1.0 + S0 / EARTHRADIUS))

#define JJ2      1.0826158e-3                       /* J2 harmonic (WGS '72)  */
#define JJ3      -0.253881e-5                       /* J3 harmonic (WGS '72)  */
#define JJ4      -1.655970e-6                       /* J4 harmonic (WGS '72)  */

#define CK2      (0.5 * JJ2 * AE2)                  /* WGS '72 physical and   */
#define CK4      (-0.375 * JJ4 * AE4)               /* geopotential constants */

#define A3OCK2   (-(JJ3) / CK2 * AE3)

#define ZNS      1.19459e-5                         /* SDP4 constants         */
#define C1SS     2.9864797e-6
#define ZES      0.01675
#define ZNL      1.5835218e-4
#define C1L      4.7968065e-7
#define ZEL      0.05490

#define COSIS    0.91744867
#define SINIS    0.39785416
#define COSGS    0.1945905
#define SINGS   -0.98088458
#define COSHS    1.0
#define SINHS    0.0

#define Q22      1.7891679e-6
#define Q31      2.1460748e-6
#define Q33      2.2123015e-7

#define G22      5.7686396
#define G32      0.95240898
#define G44      1.8014998
#define G52      1.0508330
#define G54      4.4108898

#define ROOT22   1.7891679e-6
#define ROOT32   3.7393792e-7
#define ROOT44   7.3636953e-9
#define ROOT52   1.1428639e-7
#define ROOT54   2.1765803e-9

#define THDT     4.3752691e-3

#define FASX2    0.13130908
#define FASX4    2.88431980
#define FASX6    0.37448087

/******************************************************************************/
/*                                                                            */
/* End of include file satprop.h                                              */
/*                                                                            */
/******************************************************************************/
