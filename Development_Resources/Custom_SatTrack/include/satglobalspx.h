/******************************************************************************/
/*                                                                            */
/*  Title       : satglobalspx.h                                              */
/*  Author      : Manfred Bester                                              */
/*  Date        : 15Apr94                                                     */
/*  Last change : 15Mar95                                                     */
/*                                                                            */
/*  Synopsis    : Definitions of global variables for 'satprop.c'             */
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

extern double aqnv;
extern double argPer, argPer0, argPerCof, argPerDot, argPerDF;
extern double alpDeep, axN, ayCof, ayN, ayNL;
extern double a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
extern double beta, betaL, beta0, beta02, beta04, bfact, bStar;
extern double cam, capEpAP, capU, ccc, clc0, clc1, clc2, clc3;
extern double cosEpAP, cosG, cosGL, cosH, cosHL, cosI, cosIL, cosIQ;
extern double cosInc, cos2Inc, cos4Inc, cosOME, cosQ2, cosRAAN, cosTEM, cosU;
extern double cos2U, cosZF;
extern double c1, c1L, c1SQ, c2, c3, c4, c5, coef0, coef1;
extern double decRate, decRateDot, deltaFact;
extern double delta, delta0, delta1, delta12, delta13, delta2, delta3;
extern double delA, delAM, delArgPer, delE, delInc, delMeanAn, delMeanAn0;
extern double delMeanMot, delL;
extern double delR, delRDot, delRFDot, delRaan, delU, diffAnom;
extern double dAlp, dBet, dLS;
extern double d2, d3, d4;
extern double d2201, d2211, d3210, d3222, d4410, d4422;
extern double d5220, d5232, d5421, d5433;
extern double ecc0, eccSQ, eccCB, eCosE, epoch0, eSinE;
extern double e3, eE2, eccEta, eccAnom, eL2, eM, eQ, eta, etaSQ;
extern double ff2, ff3, fT;
extern double f220, f221, f311, f321, f322, f330, f441, f442;
extern double f522, f523, f542, f543;
extern double gam;
extern double g22, g32, g44, g52, g54;
extern double g200, g201, g211, g300, g310, g322, g410, g422;
extern double g520, g521, g532, g533;
extern double inc0, incK;
extern double kep1, kep2, kep3, kep4, kep5;
extern double lngGr;
extern double meanAn, meanAn0, meanAnCof, meanAnDot, meanAnDF;
extern double meanMot0, meanMotDeep, meanMotDeepSQ, meanMot;
extern double piDot, preEpoch, precFactor;
extern double pE, pGH, pH, pInc, pInvSQ, pL, psiSQ;
extern double q22, q31, q33, qmst4;
extern double raan, raan0, raanCof, raanK, raanDot, raanDF;
extern double rDot, rk, rkDot, rfDot, rfkDot;
extern double theta, theta2, theta4;
extern double saveT, stepp, stepn, step2;
extern double sinEpAP, sinG, sinGL, sinH, sinHL, sinI, sinI2, sinIL, sinIQ;
extern double sinInc, sinMeanAn0, sinMeanAnP, sinOME;
extern double sinRAAN, sinTEM, sinU, sin2U, sinZF;
extern double sma0, sma0Deep, sma1, smallA, smallE, smallR, smallU;
extern double sE, sE2, sE3, sEL, sES, sGH, sGH2, sGH3, sGH4, sGHL, sGHS;
extern double sH, sH2, sH3, sHL;
extern double sHS, sI, sI2, sI3, sIL, sIS, sL, sL2, sL3, sL4, sLL, sLS;
extern double sS, sSE, sSG, sSH, sSI, sSL;
extern double sStar;
extern double s1, s2, s3, s4, s5, s6, s7;
extern double temp, temp1, t2Cof, t3Cof, t4Cof, t5Cof, tFP, tSQ, tCB, tQD, tQN;
extern double uk, upd1, upd2, upd3, upd4, upd5, upd6, upd7;
extern double xfact, xi, xlamo, xNodeS;
extern double x1mth2, x3th2m1, x5th2m1, x7th2m1, x7th2m3, x19th2m4, x2LI, x2OMI;
extern double xGH2, xGH3, xGH4, xH2, xH3, xI2, I3;
extern double xL, xL2, xL3, xL4, xLCof, xLDot, xLI, xLL, xLLDot;
extern double xLT, xLS, xMAM;
extern double x1, x2, x3, x4, x5, x6, x7, x8;
extern double zE, zEL, zES, zF, zM, zMO, zMOL, zMOS, zN, zNL, zNS, zX, zY;
extern double z1, z2, z3, z11, z12, z13, z21, z22, z23, z31, z32, z33;

extern int    deepSpaceFlag, truncFlag, resFlag, syncFlag;

/******************************************************************************/
/*                                                                            */
/* End of include file satglobalsp.h                                          */
/*                                                                            */
/******************************************************************************/
