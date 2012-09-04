/******************************************************************************/
/*                                                                            */
/*  Title       : sattrack.h                                                  */
/*  Author      : Manfred Bester                                              */
/*  Date        : 24Feb92                                                     */
/*  Last change : 06May96                                                     */
/*                                                                            */
/*  Synopsis    : Definitions of constants and parameters for 'sattrack'      */
/*                and other related programs.                                 */
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

#ifdef  TRUE
#undef  TRUE
#endif

#ifdef  FALSE
#undef  FALSE
#endif

#define TRUE                 -1
#define FALSE                 0
 
/******************************************************************************/
/*                                                                            */
/* character strings                                                          */
/*                                                                            */
/******************************************************************************/

#define SATDIR                "/opt/"    /* directory where SatTrack  */
                                                 /* lives; used if HOMEDIR is */
                                                 /* undefined in the Makefile */

#define DATA                  "SatTrack/data"    /* SatTrack sub-directories  */
#define PRED                  "SatTrack/pred"
#define RUN                   "SatTrack/run"
#define TLE                   "SatTrack/tle"
#define SRC                   "SatTrack/src"
#define INC                   "SatTrack/src/include"

#define CITIES                "cities.dat"       /* resource file names       */
#define DEFAULTS              "defaults0.dat"
#define MODES                 "modes.dat"
#define TLEX                  "tlex.dat"

#define REGMAIL               "bester@primenet.com"

/******************************************************************************/
/*                                                                            */
/* default parameters                                                         */
/* (these will be used if they are not specified in defaults.dat)             */
/*                                                                            */
/******************************************************************************/

#define DEFSITE               "South Pole"     /* default ground station      */
#define DEFCALLSIGN           "DL5KR"          /* default station callsign    */
#define DEFSAT                "16609"          /* default satellite (Mir)     */
#define DEFSETTLE             "tlex.dat"       /* default tle set             */
#define DEFSETSTS             "sts.dat"        /* default tle set STS         */
#define DEFSETTYPE            "NASA"           /* default tle set type        */
#define DEFSATGROUP           "am"             /* default satellite group     */
#define DEFDISPTYPE           "M"              /* default live display type   */
#define DEFTIMEZONE           "UTC"            /* default time zone           */
#define DEFXTERM              "xterm"          /* default X terminal type     */
#define DEFANTENNADEV         "/dev/null"      /* default antenna interface   */
#define DEFRADIODEVA          "/dev/null"      /* default radio interface A   */
#define DEFRADIODEVB          "/dev/null"      /* default radio interface B   */

#define DEFDURATION           6.0              /* default duration            */
#define DEFMINELEVATION       0.0              /* default minimum elevation   */
#define DEFTRACKINGFLAG       0                /* default tracking flag       */
#define DEFDISPLINES          24               /* number of lines on display  */
#define DEFXDISPLINES         64               /* number of lines on Xdisplay */
#define DEFATMPRESS           760.0            /* standard atm. press. [mmHg] */
#define DEFAMBTEMP            15.0             /* standard amb. temp.  [C]    */
#define DEFRELHUMID           30.0             /* standard rel. humidity [%]  */

/******************************************************************************/
/*                                                                            */
/* header of NORAD two-line Keplerian element sets                            */
/* (not used in 'sattrack' but in the auxiliary programs)                     */
/*                                                                            */
/******************************************************************************/

#define TLEHEADER             "- Current Two-Line Element Sets "

/******************************************************************************/
/*                                                                            */
/* control flags and character strings for different propagation models       */
/*                                                                            */
/******************************************************************************/

#define TLEMEAN               0
#define LOWEARTH              1
#define DEEPSPACE             2

#define TLEMEANMODEL          "TLE Mean"
#define LOWEARTHMODEL         "SGP4"
#define DEEPSPACEMODEL        "SDP4"

/******************************************************************************/
/*                                                                            */
/* command line arguments passed to the main program                          */
/*                                                                            */
/******************************************************************************/

#define BATCH                 "-B"
#define DEBUG                 "-D"
#define DEFFILE               "-F"
#define GRAPH                 "-G"
#define MODEL                 "-M"
#define QUICK                 "-Q"
#define TIMEZONE              "-T"
#define VERBOSE               "-V"

/******************************************************************************/
/*                                                                            */
/* batch mode command line parameters                                         */
/*                                                                            */
/******************************************************************************/

#define NASA                  "NASA"
#define NORAD                 "NORAD"
#define SHORTPR               "SHORTPR"
#define LONGPR                "LONGPR"
#define TRANSIT               "TRANSIT"
#define NOW                   "NOW"
#define AUTO                  "AUTO"
#define HARDCOPY              "HARDCOPY"

/******************************************************************************/
/*                                                                            */
/* menu selection parameters                                                  */
/*                                                                            */
/******************************************************************************/

#define DX                    "D"
#define FX                    "F"
#define LX                    "L"
#define MX                    "M"
#define PX                    "P"
#define QX                    "Q"
#define RX                    "R"
#define SX                    "S"
#define TX                    "T"
#define VX                    "V"
#define YES                   "Y"
#define NO                    "N"

#define START                 0
#define DISPLAY               1
#define SINGLESAT             2
#define MULTISAT              3
#define PREDICT               4
#define TEST                  5
#define RESTART               6

#define QUIT                  997
#define EXIT                  998
#define VOID                  999

/******************************************************************************/
/*                                                                            */
/* live display widths                                                        */
/*                                                                            */
/******************************************************************************/

#define DISPCOLUMNS           80               /* number of columns on disp   */
#define XDISPCOLUMNS          132              /* number of columns on Xdisp  */

/******************************************************************************/
/*                                                                            */
/* control flags for single and multisat live displays                        */
/*                                                                            */
/******************************************************************************/

#define ERROR                -1
#define OFF                   0
#define ON                    1
#define LIST                  2
#define ORDER                 3
#define ELEV                  4
#define NAME                  5
#define OBJNUM                6
#define MANTRK                7
#define AUTOTRK               8
#define SELECT                9
#define MARK                  10

/******************************************************************************/
/*                                                                            */
/* satellite types                                                            */
/*                                                                            */
/******************************************************************************/

#define BASIC                 0
#define STS                   1
#define MIR                   2
#define FREEDOM               3
#define HST                   4

/******************************************************************************/
/*                                                                            */
/* antenna controller and radio types                                         */
/*                                                                            */
/* these are used to select the proper control sequences in 'satctrl.c' and   */
/* for the ISI telescopes in 'sattele.c'                                      */
/*                                                                            */
/* THE USER NEEDS TO SPECIFY THE ANTENNATYPE AND THE RADIOTYPE BELOW AS ONE   */
/* OF THE OPTIONS FORM THE LIST OF ANTENNA TYPES AND RADIO TYPES. MORE CAN    */
/* BE ADDED IF NEEDED                                                         */
/*                                                                            */
/******************************************************************************/

#define NONE                 -1
#define GENERIC               0

#define ISITELESCOPE          1
#define KANSASCITY            2
#define TRAKBOX               3

#define FT736                 1
#define IC970                 2
#define TS790                 3
#define DUAL                  4

#ifdef ISI
#define ANTENNATYPE           ISITELESCOPE
#define RADIOTYPE             NONE
#else
#define ANTENNATYPE           TRAKBOX        /* <- USER NEEDS TO SPECIFY THIS */
#define RADIOTYPE             NONE           /* <- USER NEEDS TO SPECIFY THIS */
#endif

/******************************************************************************/
/*                                                                            */
/* printer options                                                            */
/*                                                                            */
/******************************************************************************/

#define PRINTCMD              "enscript"
#define PRINTOPT              "-f"
#define FONT                  "Courier"
#define SIZE9                 "9"
#define SIZE10                "10"
#define SIZE12                "12"

/******************************************************************************/
/*                                                                            */
/* math functions                                                             */
/*                                                                            */
/******************************************************************************/

#define SQR(x)                ((x)*(x))

/******************************************************************************/
/*                                                                            */
/* general numerical constants                                                */
/*                                                                            */
/******************************************************************************/

#define ZERO                  0.0
#define ONEPPB                1.0e-9
#define ONEPPM                1.0e-6
#define TWOPPM                2.0e-6
#define ONETHIRD              (1.0/3.0)
#define TWOTHIRDS             (2.0/3.0)
#define THREEHALFS            (3.0/2.0)
#define ONE                   1.0
#define ONEMEG                1.0e6
#define TWOMEG                2.0e6

/******************************************************************************/
/*                                                                            */
/* numerical constants for unit conversions                                   */
/*                                                                            */
/******************************************************************************/

#ifdef  PI
#undef  PI
#endif

#ifdef  TWOPI
#undef  TWOPI
#endif

#define PI                    3.14159265358979323846264338
#define TWOPI                 (2.0*PI)
#define FOURPI                (4.0*PI)
#define HALFPI                (0.5*PI)
#define THREEHALFPI           (1.5*PI)

#define CRH                   (24.0/TWOPI)         /* convert rad into hours  */
#define CRS                   (86400.0/TWOPI)      /* convert rad into sec    */
#define CRD                   (360.0/TWOPI)        /* convert rad into deg    */
#define CRAM                  (21600.0/TWOPI)      /* convert rad into arcmin */
#define CRAS                  (1296000.0/TWOPI)    /* convert rad into arcsec */
#define CDR                   (TWOPI/360.0)        /* convert deg into rad    */
#define CAMR                  (TWOPI/21600.0)      /* convert arcmin into rad */
#define CASR                  (TWOPI/1296000.0)    /* convert arcsec into rad */
#define CRREV                 (1.0/TWOPI)          /* convert rad into rev    */

#define HALFDEG               (0.5*CDR)            /* half a deg [rad]        */

#define MPD                   1440.0               /* minutes per day         */
#define MPD2                  (MPD*MPD)            /* (minutes per day)^2     */
#define MPD3                  (MPD2*MPD)           /* (minutes per day)^3     */
#define SPD                   86400.0              /* seconds per day         */

#define CKMM                  1000.0               /* convert km to m         */
#define CMKM                  1.0e-3               /* convert m to km         */
#define CKMNM                 0.539956804          /* convert km to naut. mil.*/
#define CHZKHZ                1.0e-3               /* convert Hz to kHz       */
#define CKHZMHZ               1.0e-3               /* convert kHz to MHz      */
#define CHZMHZ                1.0e-6               /* convert Hz to MHz       */
#define CKHZHZ                1.0e+3               /* convert kHz to Hz       */
#define CMHZHZ                1.0e+6               /* convert MHz to Hz       */

/******************************************************************************/
/*                                                                            */
/* numerical constants describing the Earth's orbit and figure                */
/*                                                                            */
/* EARTHRADIUS and EARTHFLAT are from: "Astronomical Almanac", 1991, p. K13   */
/*                                                                            */
/******************************************************************************/

#define EARTHSMA              149597892.0          /* 1 AU [km]               */
#define EARTHRADIUS           6378.137             /* equatorial radius [km]  */
#define EARTHECCEN            0.01675104           /* Earth's orbit eccentr.  */
#define EARTHFLAT             (1.0/298.257222)     /* geoid model parameters  */

/******************************************************************************/
/*                                                                            */
/* numerical constants describing the apparent size of the Sun and the        */
/* proximity limit for calculating transits across the solar disk             */
/*                                                                            */
/******************************************************************************/

#define SUNRADIUS             695980.0             /* equatorial radius [km]  */
#define SUNDISKRAD            (16.0*CAMR)          /* Sun disk radius [rad]   */
#define SUNPROX               48.0                 /* Sun prox limit [arcmin] */

/******************************************************************************/
/*                                                                            */
/* numerical constants describing the motions within the solar system         */
/*                                                                            */
/******************************************************************************/

#define JULCENT               36525.0              /* mean solar days / jcy   */
#define TROPCENT              36524.219879         /* mean solar days / cy    */
#define TROPYEAR              (TROPCENT/100.0)     /* mean solar days / year  */
#define JULDAT1900            2415020.0            /* Julian date of 1900.0   */
#define JULDAT1950            2433282.423          /* Julian date of 1950.0   */
#define JULDAT2000            2451545.0            /* Julian date of 2000.0   */
#define SIDSOLAR              1.002737909350       /* sidereal rotation rate  */
#define SIDRATE               (TWOPI/SPD*SIDSOLAR) /* [rad/s]                 */

/******************************************************************************/
/*                                                                            */
/* physical constants                                                         */
/*                                                                            */
/* GM (and KEPLER) are from: "Astronomical Almanac", 1991, p. K13             */
/*                                                                            */
/******************************************************************************/

#define CVAC                  2.99792458e5         /* speed of light [km/s]   */

#define GM                    398600.5000          /* [km^3/s^2]              */
#define GMSGP                 398600.7995          /* value used in SGP model */

#define KEPLER                42241.09773          /* GM^(1/3)*(SPD/2PI)^(2/3)*/
#define KEPLERSGP             42241.10831          /* value used in SGP model */
                                                   /* with a [km] and T [min] */

/******************************************************************************/
/*                                                                            */
/* atmospheric parameters used for calculation of refraction                  */
/*                                                                            */
/******************************************************************************/

#define SCALEHEIGHT           8.0              /* atmosph. scale height [km]  */
#define WAVEL                 0.550            /* wavelength of observ. [um]  */
#define REFWAVEL              0.550            /* reference wavelength [um]   */

/******************************************************************************/
/*                                                                            */
/* satellite parameters                                                       */
/*                                                                            */
/******************************************************************************/

#define DOWNLINKFREQ          146.0            /* default downlink freq [MHz] */
#define UPLINKFREQ            0.0              /* default uplink freq   [MHz] */
#define FREQSTEP              100.0            /* default tuning step   [Hz]  */

#define MAXPHASE              256.0            /* max. value for phase number */

#define MAXFREQS              10               /* max. number sat frequencies */
#define MAXMODES              10               /* max. number sat modes       */

#define MAXCITIES             850              /* max. number of cities       */
#define MAXSATS               600              /* max. number of satellites   */
#define MAXSTATIONS           10               /* max. number of gnd stations */
#define MAXTDRS               5                /* max. number of TDRS sats    */

#define MAXCITYLEN            37               /* max. length of city name    */
#define MAXSATNAMELEN         14               /* max. length of sat name     */

/******************************************************************************/
/*                                                                            */
/* satellite eclipse parameters                                               */
/*                                                                            */
/******************************************************************************/

#define SAT                   0                /* code for calculation of     */
#define SUN                   1                /* azimuth and elevation       */
#define MOON                  2

#define DAY                   0                /* code for display of eclipse */
#define PARTIAL               1                /* conditions                  */
#define NIGHT                 2
#define VISIBLE               3
#define BLANK                 4

#define TWILIGHT              (-6.0*CDR)       /* civil twilight limit  [rad] */
#define VISPHSLIMIT           (120.0*CDR)      /* sun phase angle limit [rad] */
#define VISELELIMIT           (15.0*CDR)       /* visib elevation limit [rad] */

#define VLOWELEV              (-15.0*CDR)      /* low elevation level 1 [rad] */
#define LOWELEV               (-2.0*CDR)       /* low elevation level 2 [rad] */
#define MAXELELIMIT           (40.0*CDR)       /* good pass elev limit  [rad] */

#define TRACKLIMIT1           (-3.0*CDR)       /* antenna/radio track limit 1 */
#define TRACKLIMIT2           (-1.0*CDR)       /* antenna/radio track limit 2 */

#define MAXAZIRATE            (5.0*CDR)        /* maximum azimuth rate [rad]  */
#define MAXELERATE            (5.0*CDR)        /* maximum elevation rate [rad]*/

/******************************************************************************/
/*                                                                            */
/* control flags and parameters for geosynchronous satellites                 */
/*                                                                            */
/******************************************************************************/

#define GEOSYNC               1
#define GEOSTAT               2

#define GEOSYNCMOT            0.05             /* maximum deviation in mean   */
                                               /* motion from 1 rev/d for     */
                                               /* geosynchronous satellites   */

#define GEOSTATINC            (10.0*CDR)       /* maximum inclination [deg]   */
                                               /* for geostationary sats      */

#define MAXDAYS               2.0              /* limit for rise time search  */
                                               /* for non-geosynchonous sats  */

#define MAXDAYSSYNC           3.0              /* limit for rise time search  */
                                               /* for geosynchonous sats [d]  */

/******************************************************************************/
/*                                                                            */
/* event time parameters                                                      */
/*                                                                            */
/******************************************************************************/

#define HALFMIN               (0.5/MPD)        /* half a minute [d]           */
#define ONEMIN                (1.0/MPD)        /* one minute [d]              */
#define THREEMIN              (3.0/MPD)        /* three minutes [d]           */
#define NINEMIN               (9.0/MPD)        /* nine minutes [d]            */
#define TENMIN                (10.0/MPD)       /* ten minutes [d]             */

#define SUNUPDATEINT          (15.0/MPD)       /* update interval Sun [d]     */
#define PRECUPDATEINT         (120.0/MPD)      /* update interval precession  */
#define NUTEUPDATEINT         (120.0/MPD)      /* update interval nutation    */

#define FASTUPDATEINT         5                /* display update interval [s] */
                                               /* xterm single and multisat   */

#define MEDUPDATEINT          5                /* display update interval [s] */
                                               /* non-xterm single satellite  */

#define SLOWUPDATEINT         5                /* display update interval [s] */
                                               /* non-xterm multisat          */

#define SLEEPTIME             50               /* sleep time [ms]             */

#ifdef ISI
#define TRACKTIMEAHEAD        10               /* ISI track time offset [s]   */
#define TRACKINT              1                /* ISI track time interval [s] */
#else
#define TRACKTIMEAHEAD        0                /* USER track time offset [s]  */
#define TRACKINT              1                /* USER track time interval [s]*/
#endif

/******************************************************************************/
/*                                                                            */
/* macros for handling the system timer function                              */
/*                                                                            */
/******************************************************************************/

#ifdef SVR4
#define HOLDSIG       sighold(SIGALRM)             /* block ALRM signal       */
#define RELEASESIG    sigrelse(SIGALRM)            /* release ALRM signal     */
#define PAUSESIG      sigpause(SIGALRM)            /* sleep until ALRM signal */
#else
#define HOLDSIG       sigblock(sigmask(SIGALRM))
#define RELEASESIG    sigblock(0)
#define PAUSESIG      sigpause(0)
#endif

/******************************************************************************/
/*                                                                            */
/* I/O control parameters                                                     */
/*                                                                            */
/******************************************************************************/

#define STDIN_FILENO          0                /* used in checkKeyboard()     */

#define SATCODE               998L             /* control flag to enable ISI  */
                                               /* real-time system for sats   */

/******************************************************************************/
/*                                                                            */
/* X window parameters                                                        */
/*                                                                            */
/******************************************************************************/

#define GRIDSTEPX     (double) 30.0            /* grid step longitude [deg]   */
#define NGRIDX        (int) 5                  /* number of grid lines left   */
                                               /* and right of zero meridian  */

#define GRIDSTEPY     (double) 30.0            /* grid step latitude [deg]    */
#define NGRIDY        (int) 2                  /* number of grid lines        */
                                               /* above and below equator     */

#define GNDSPOT       (unsigned int) 3         /* half the size of the square */
#define GNDSPOTX      (unsigned int) 2         /* half the size of the square */
#define SATSPOT       (unsigned int) 2         /* half the size of the square */
#define TRKSPOT       (unsigned int) 3         /* half the size of the square */
#define CTYSPOT       (unsigned int) 1         /* half the size of the square */

#define NSEGS         (int) 120                /* number of segments per rev  */
#define NREVS         (int) 3                  /* number of revs              */
#define NSEGSGT       (int) (NSEGS*NREVS + 1)  /* (for ground track)          */

#define NSEGSVC       (int) 180                /* visib circle segments       */
#define LCNUM         96315

#define NMIRPOINTS    420                      /* max data points in Mir icon */
#define NSTSPOINTS    210                      /* max data points in STS icon */

#define LARGE         1                        /* map size flag               */
#define MEDIUM        2                        /* map size flag               */
#define SMALL         3                        /* map size flag               */

#define DHLIMIT       1.0                      /* differential height [km]    */

#define KSCLAT        28.608001*CDR            /* KSC latitude  [rad]         */
#define KSCLONG       80.604*CDR               /* KSC longitude [rad]         */
#define KSCALT        0.0                      /* KSC altitude  [m]           */

/******************************************************************************/
/*                                                                            */
/* End of include file sattrack.h                                             */
/*                                                                            */
/******************************************************************************/
