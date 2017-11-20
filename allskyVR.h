/**************************************************************************************/
/* allskyVR.h                                                         November 2017   */
/*                                                                                    */
/* Usage:                                                                             */
/*         allsky -i exoplanets.csv -f format.txt [-o output-directory]               */
/*                                                                                    */
/*         -i = input file name                                                       */
/*         -f = geometry format file name                                             */
/*         -i = output directory file name                                            */
/*                                                                                    */
/* https://github.com/cfluke/allskyVR                                                 */
/*                                                                                    */
/* Questions or Comments?  cfluke@swin.edu.au                                         */
/*                                                                                    */
/**************************************************************************************/
/* GNU General Public License v3.0                                                    */
/**************************************************************************************/
/* Permissions of this strong copyleft license are conditioned on making available    */
/* complete source code of licensed works and modifications, which include larger     */
/* works using a licensed work, under the same license. Copyright and license notices */
/* must be preserved. Contributors provide an express grant of patent rights.         */
/**************************************************************************************/

/* WARNING: Contains some data definitions that are no longer required... */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/* Default names for exporting specific assets required for PDF and VR */
#define _US2OUTPUT     (char *)"allsky"
#define _SNAPSHOT      (char *)"us2snapshot"
#define _ANAGLYPHNAME  (char *)"us2anaglyph"
#define _CAMERAFILE    (char *)"us2camera.txt"
#define _AFRAMEFILE    (char *)"us2aframe.html"

#define VR_RADIUS 100.0

/* CONSTANTS */
#define D2R  0.017453292519943295       /* Conversion of degrees to radians */
#define RA2R 0.261799387799149408       /* Conversion of RA (hrs) to radians */
#define R2D 57.295779513082322865       /* Conversion of radians to degrees */

#define DEMIN                   -90.0           /* Maximum Declination in degrees */
#define DEMAX                   +90.0           /* Maximum Declination in degrees */
#define RAMIN                     0.0           /* Minimum RA value in hours */
#define RAMAX                    24.0           /* Maximum RA value in hours */
#define MAXTAG 32        /* Maximum length of a VRML tag base */

#define TRUE 1
#define FALSE 0

#define MINUSONE -1
#define UNKNOWN     20
#define VR_TAG      30

#define TEXTURE_SET 3
#define TEXTURE_DEFAULT  0.002


/* A-Frame default values */
#define POINTSIZE 0.005    /* Radius of the A-Frame sphere used for points */
#define POINTSEG   2       /* SegmentWidth and SegmentHeight for spheres */
#define SPHERESEG  4       /*  used to represent points, spheres and textures */
#define TEXTURESEG 3



typedef struct {
   double r, g, b;
} COLOUR;

typedef struct {
   double x, y, z;
} XYZ;

typedef struct ListNode {
   int tagType;
   char *tagName;
   char *visName;
   COLOUR col;
   struct ListNode *next;
} ListNode;


typedef struct LabelNode {
   char *label;
   char *flabel;
   COLOUR col;
   struct LabelNode *next;
} LabelNode;

typedef struct {
   int debug;
   int pstate;
   int cstate;
   int astate;
   int vrstate;
   int styleFlag;       /* Use media9.sty or movie15.sty ? */
   int darkSphere;      /* Draw a dark sphere inside the celestial coordinate frame? */
   char *us2output;
   char *us2input;
   char *us2format;
} PublishState;


typedef struct {
   XYZ min;
   XYZ max;
} Bounds;


enum skyType { VRSTATE0=-1, VRSTATE1=0, VRSTATE2 = 1, VRSTATE3=2, VRSTATE4=3, VRSTATE5=4, VRSTATE6=5 };



typedef struct {
   int set;               /* Type of PublishableDatSet */

   long Npds;             /* Number of data items */
   int Ncategory;         /* Number of data categories */

   XYZ *xyz;		  /* Positions */
   char **catName;        /* Names of categories if read from format file */

   float *radius;         /* Radius of sphere or texture, thickness of point */
   float maxRadius;       /* Maximum radius value for normalising */

   int *category;         /* Array of category indices: [0..Npds-1] */
   COLOUR *colour;        /* Array of RGB category colours: [0..Ncategory-1] */
   int *count;            /* Number of items per category: [0..Ncategory-1] */
   char tagbase[MAXTAG];  /* Base string for category tags with pushVRMLname */

   float tsize;           /* Internal texture size or sphere size */



/* The following are only used for TEXTURE_SETS */
   char **tname;          /* Array of texture file names: [0..Ncategory-1] */
   unsigned int *tid;     /* Array of S2PLOT texture IDs */
   float alpha;           /* Transparency alpha value: [0..1] */
   char trans;            /* Transparency type: [o]paque, [t]ransparent, [s] */

/* Any other relevant property should be defined and initialised elsewhere */


} PublishableDataSet;

