/*****************************************************************************/
/* us2publish.h                                             November 2017    */
/*                                                                           */
/* Usage:                                                                    */
/*                                                                           */
/* https://github.com/cfluke/allskyVR                                        */
/*                                                                           */
/* Questions or Comments?  cfluke@swin.edu.au                                */
/*                                                                           */
/*****************************************************************************/
/* GNU General Public License v3.0                                           */
/*****************************************************************************/
/* Permissions of this strong copyleft license are conditioned on making     */
/* available complete source code of licensed works and modifications,       */
/* which include larger works using a licensed work, under the same license. */
/* Copyright and license notices must be preserved. Contributors provide an  */
/* express grant of patent rights.                                           */
/*****************************************************************************/

#ifndef __US2PUBLISH__
#define __US2PUBLISH__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <sys/utsname.h>
#include "s2plot.h"


/* These default values may need to be modified to match your system configuration */

#define _ADOBESTRING (char *)"Adobe Acrobat Reader DC"
#define _DEFAULTFONT (char *)"/Library/Fonts/Verdana Bold.ttf"


/**************************/

/* The Base Directory which must be accessible for correct export of PDF and VR assets */
#define _BASEDIRECTORY (char *)"us2BaseDirectory"

/* The master scripts for building the PDF and VR outputs */
#define _MAKERSCRIPT   (char *)"us2pdfmaker.csh"
#define _VRSCRIPT      (char *)"us2vrmaker.csh"

/* Name of the build script that will be auto-generated and copied to PDF export directory */
#define _BUILDSCRIPT   (char *)"buildscript.csh"

/* Default names for exporting specific assets required for PDF and VR */
#define _US2OUTPUT     (char *)"us2output"
#define _SNAPSHOT      (char *)"us2snapshot"
#define _ANAGLYPHNAME  (char *)"us2anaglyph"
#define _CAMERAFILE    (char *)"us2camera.txt"
#define _AFRAMEFILE    (char *)"us2aframe.html"


#define MAXTAG 32        /* Maximum length of a VRML tag base */

typedef struct {
   int debug;
   int pstate;			
   int cstate;
   int astate;
   int vrstate;
   int styleFlag;	/* Use media9.sty or movie15.sty ? */ 
   int darkSphere;	/* Draw a dark sphere inside the celestial coordinate frame? */
   char *us2output;
   char *us2input;
   char *us2format;
} PublishState;


typedef struct {
   int set;               /* Type of PublishableDatSet */

   long Npds;             /* Number of data items */
   int Ncategory;         /* Number of data categories */

   char **catName;	  /* Names of categories if read from format file */

   XYZ *xyz;              /* Array of XYZ coordinates: [0..Npds-1] */
   float *radius;	  /* Radius of sphere or texture, thickness of point */ 
   float maxRadius;	  /* Maximum radius value for normalising */

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


      
typedef struct ListNode {       
   int tagType;
   char *tagName; 
   char *visName;
   COLOUR col;
   struct ListNode *next;       
} ListNode;

typedef struct LabelNode {       
   unsigned int tid;
   char *label;
   char *flabel;
   XYZ xyz;
   COLOUR col;
   float aspect;
   int mflag;
   XYZ marker;
   COLOUR mcol; 
   XYZ msize;
   struct LabelNode *next;       
} LabelNode;


typedef struct {
   XYZ min;
   XYZ max;
} Bounds;

/* TYPES OF PUBLISHABLE DATA SETS */
#define POINT_SET 1
#define SPHERE_SET 2
#define TEXTURE_SET 3

/* MODEL TREE TYPES FOR CAPTION-BASED TOGGLES */ 
#define SerialisedNode  11
#define NamedBranch 12
#define UNKNOWN     20
#define VR_TAG      30

/* PDF FUNCTIONALITY */
#define STATE0 0
#define STATE1 1
#define STATE2 2

#define NOCAMERA 0
#define LOADCAMERA 1
#define SAVECAMERA 2

/* Must match definitions in us2pdfmaker.csh */
#define MEDIA9  1		/* Use Alexander Grahn's media9.sty style */
#define MOVIE15 2		/* Use Alexander Grahn's movie15.sty style */


/* Geometry default sizes */
#define TEXTURE_DEFAULT  0.002
#define SPHERE_DEFAULT   0.005
#define PDFLABEL_DEFAULT 0.004
#define VRLABEL_DEFAULT  0.07

/* A-Frame default values */
#define POINTSIZE 0.005    /* Radius of the A-Frame sphere used for points */
#define POINTSEG   2       /* SegmentWidth and SegmentHeight for spheres */
#define SPHERESEG  4	   /*  used to represent points, spheres and textures */
#define TEXTURESEG 3

enum skyType { VRSTATE0=-1, VRSTATE1=0, VRSTATE2 = 1, VRSTATE3=2, VRSTATE4=3, VRSTATE5=4, VRSTATE6=5 };

/* Functions visible to standard S2PLOT programs to initialise and call the publishing module */

void us2publish(char *oname, ListNode *tagList);
PublishState initPublishState(int argc, char *argv[]);
PublishState us2opend(int argc, char *argv[], PublishState lps);
int us2publishKey(char key);

PublishableDataSet initPublishableDataSet(int set, long Npds, int Ncategory, char *tagbase,
		Bounds b, char *tname[], float tsize);
void publishDataSet(PublishableDataSet lpds, int tagflag);
void us2palphatex(unsigned int id);
void us2palphatexInvert(unsigned int id);

float normaliseRadius(long N, float *radius, float tsize);

ListNode *create(int tagType, char *tagName, ListNode *next);
ListNode *insert(ListNode *head, int tagType, char *tagName);
ListNode *insertN(ListNode *head, int tagType, char *base, int N);
ListNode *insertNwithName(ListNode *head, int tagType, char *base, int N, 
                char **catName);
int count(ListNode *head);

LabelNode *createLabelFreeType(XYZ xyz, COLOUR col, char *label, char *font, LabelNode *next);
LabelNode *createLabelLatex(XYZ xyz, COLOUR col, char *label, LabelNode *next);
LabelNode *insertLabelFreeType(LabelNode *head, XYZ xyz, COLOUR col, char *label, char *font);
LabelNode *insertLabelLatex(LabelNode *head, XYZ xyz, COLOUR col, char *label);
LabelNode *insertLabelTexture(LabelNode *head, XYZ xyz, COLOUR col, char *texfile);
LabelNode *createLabelTexture(XYZ xyz, COLOUR col, char *texfile, LabelNode *next);
void displayLabels(LabelNode *head, float tsize, XYZ off, float alpha, char trans);

LabelNode *addMarker(LabelNode *head, XYZ xyz, COLOUR col, XYZ msize);

/* skyCube Functions */
int skyCube(int vrstate, char *us2output, char *us2format);
void displayLabelsSkyCube(int vrstate, LabelNode *head, float tsize, char trans, float rah);
void publishDataSetSkyCube(PublishableDataSet lpds);


void exportToAFrame(char *aname, char *mode, PublishableDataSet lpds, 
	float radius, ListNode *tagList);

Bounds initBounds(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

#endif
