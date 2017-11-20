/*****************************************************************************/
/* templateSpherical.c                                      November 2017    */
/*                                                                           */
/* Usage:                                                                    */
/* 	templateSpherical  -i <input> -f <format> [-o output-directory]      */
/*                                                                           */
/*         -i = input file name                                              */
/*         -f = geometry format file name                                    */
/*         -i = output directory file name                                   */
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "s2plot.h"
#include "us2publish.h"
#include "us2astro.h"


#define VR_RADIUS 100.0
#define MINUSONE -1

/* US2PUBLISH: REQUIRED */
PublishState ps;
/************************/
PublishableDataSet pds1; 

#ifdef PDS2
PublishableDataSet pds2; 
#endif


int kcb(unsigned char *key)
{
/*** Act on any keypress other than 'P', 'S', 'L', or 'V' ***/


/**** US2PUBLISH: REQUIRED */
   return us2publishKey(*key);
/***************************/

}

void pdfGeometry(void)
/* Function can be modified to pass parameters as arguments or access global variables */
{
/* EXAMPLE: Draw an off-centre  yellow sphere */
/*
   pushVRMLname("YellowSphere");
   ns2sphere(0.5, 0.5, 0.5, 0.1, 1.0, 1.0, 0.0);
   pushVRMLname("ANON");
*/
}

void posterGeometry(void)
/* Function can be modified to pass parameters as arguments or access global variables */
/* Any geometry drawn here will not appear in the interactive PDF model */
{
/* EXAMPLE: Draw an axis-centered red sphere */
/*
   pushVRMLname("RedSphere");
   ns2sphere(0.0, 0.0, 0.0, 0.3, 1.0, 0.0, 0.0);
   pushVRMLname("ANON");
*/
}


/* MODIFY: If this function does not meet your requirements, it can be adjusted */
LabelNode *generateSphericalCoordinateLabels(float radius, int raspace, int despace,
	float hourangle, float raoff, float decoff, COLOUR col, int mflag, XYZ msize) 
/* hourangle = RA where Dec coordinates placed */
{
   LabelNode *labelList = NULL;	    /* Linked list of label names */
   char label[128];				
   float sf = 1.1;		    /* Scale factor on radius for nice labels */
   XYZ xyz;			    /* Current XYZ coordinate for conversions */
   int i;

/* EXAMPLE: Equal spacing in RA - raspace hours per RA line */
/* Output a debug statement as LaTeX creation can be slow */
   fprintf(stderr,"Please wait. Generating LaTeX labels: ");
   for (i=0;i<24;i+=raspace) {			
      xyz = us2rade2xyz((float)i+raoff, decoff, radius*sf);
                                            /* Coordinate position for label */
      sprintf(label,"$\\alpha = %dh$",i);   /* Write the LaTeX string */
      labelList = insertLabelLatex(labelList, xyz, col, label);

/* Adjustments to a Label or Marker can only be made to the most recently    */
/* created Label/Marker.  Otherwise, additional code is required to traverse */
/* the linked list and modify a specific Label/Marker */
      if (mflag == TRUE) {
         xyz = us2rade2xyz((float)i, 0, radius);
					/* Coordinate position for marker */
         labelList = addMarker(labelList, xyz, col, msize);
      }
      fprintf(stderr,".");
   }

/* EXAMPLE: Equal spacing in Declination - despace degrees per Dec line */
   for (i=-90;i<=+90;i+=despace) {
      xyz = us2rade2xyz(hourangle, (float)i+decoff, radius*sf);
      sprintf(label,"$\\delta = %d^{\\circ}$",i); 
      labelList = insertLabelLatex(labelList, xyz, col, label);
      if (mflag == TRUE) {
         xyz = us2rade2xyz(hourangle, (float)i, radius);
         labelList = addMarker(labelList, xyz, col, msize);
      }
      fprintf(stderr,".");
   }
   fprintf(stderr,"\n");

   return labelList;

}

void cb(double *t, int *kc)
{

/* Cannot access VR output when in anaglyph mode */
   if (ps.astate == TRUE) ps.vrstate = VRSTATE0; 	

/* OPTIONAL: Draw any extra geometry that should only be visible when */
/* not writing poster image. If more complete control of visibility is */
/* required at other times, use the *kc keypress variable */
   if (ps.pstate == STATE0) pdfGeometry(); 

/* OPTIONAL: Draw any extra geometry that should only be visible when */
/* creating poster image. If more complete control of visibility is */
/* required at other times, use the *kc keypress variable */
   if (ps.pstate == STATE1) posterGeometry(); 


/* OPPORTUNITY: Could provide more advanced control in associating tags */
/* with geometry */
   int tagflag = TRUE;
/* Setting tagflag to FALSE means that categories are not tagged */ 

   static LabelNode *labelList = NULL;
   float radius = 0.8;			/* Radius of the coordinte sphere */
   float rah    = 3.0;			/* RA where Dec labels appear */
   float decoff = -1.0;			/* Dec where RA labels appear */
   float raoff  = +0.3;
   int mflag    = FALSE;		/* Associate markers with labels? */
   int raspace  = 6;
   int despace  = 30;
   XYZ msize = { 0.01, 0.01, 0.01 };	/* Default marker size */
   COLOUR labelcol = { 1.0, 1.0, 0.3 };	/* Label colour */
   if (labelList == NULL) {
      labelList = generateSphericalCoordinateLabels(radius, raspace, despace, 
		rah, raoff, decoff, labelcol, mflag, msize);
   }

   XYZ off = { 0.01, 0.01, 0.0 };    /* A useful default for the label offset */
   float lsize  = PDFLABEL_DEFAULT*radius;	 /* Size of label texture */
   float alpha  = 1.0;		     /* Transparency */
   char trans   = 's';		     /* Use the transparent + absorption type */

/* MODIFY: This section draws the geometry that is used in the poster image, */
/* interactive figure and A-Frame SkyCube format */

   if (ps.vrstate == VRSTATE0) {     /* Standard behaviour for interaction */ 

/* Only draw the dark sphere if command line flag is TRUE and not in VR */
/* export mode. */
      if (ps.darkSphere == TRUE) {
         pushVRMLname("DarkSphere");
         ns2sphere(0.0, 0.0, 0.0, radius*0.999, 0.01, 0.01, 0.01);
      }

      pushVRMLname("Labels");		
      displayLabels(labelList, lsize, off, alpha, trans);
        /* Traverse the linked list and draw the labels as billboard textures */

      publishDataSet(pds1,tagflag);
#ifdef PDS2
      publishDataSet(pds2,tagflag);  /* Display data using defined atributes */ 
#endif



   } else {			     /* Creating SkyCube for A-Frame viewer */
      float vrlsize  = VRLABEL_DEFAULT*radius;    
      pushVRMLname("Labels");		
      displayLabelsSkyCube(ps.vrstate,labelList, vrlsize, trans, rah);
        /* Traverse the linked list and draw the labels as billboard textures */

      if (pds1.set == TEXTURE_SET) {
         publishDataSetSkyCube(pds1);		/* Display the data */
/**** CJF: May need a radius parameter passed through for texture scaling */

      } else {
         publishDataSet(pds1,tagflag);
      }
#ifdef PDS2
      if (pds2.set == TEXTURE_SET) {
         publishDataSetSkyCube(pds2);		/* Display the data */
      } else {
         publishDataSet(pds2,tagflag);  /* Display using defined atributes */ 
      }
#endif
   }
   pushVRMLname("ANON");


/* MODIFY: Create a linked list of tags that will available as toggles */
/* in the 3D PDF figure caption. The list will be traversed in reverse */
/* order, but toggle ordering can be adjusted in the final LaTex document. */ 
   static ListNode *tagList = NULL;
   if (tagList == NULL) {
      if (ps.us2format==NULL) {
         tagList = insertN(tagList, pds1.set, pds1.tagbase, pds1.Ncategory);
      } else {
         tagList = insertNwithName(tagList, pds1.set, pds1.tagbase, 
				pds1.Ncategory, pds1.catName);
      }
#ifdef PDS2
      tagList = insertN(tagList, pds2.set, pds2.tagbase, pds2.Ncategory);
#endif
      if (ps.darkSphere) { 
         tagList = insert(tagList, NamedBranch, "DarkSphere"); 
      }
      tagList = insert(tagList, SerialisedNode, "Grid");
      tagList = insert(tagList, NamedBranch, "Labels");
   }

/*** US2PUBLISH: REQUIRED ***/
   if (ps.pstate != STATE0) us2publish(ps.us2output, tagList);
   static int loop = 0;

   if (ps.vrstate == VRSTATE1) {
      if (loop == 0) {
         exportToAFrame(_AFRAMEFILE,"w",pds1,VR_RADIUS, tagList);
#ifdef PDS2
         exportToAFrame(_AFRAMEFILE,"a",pds2,VR_RADIUS, NULL);
#endif
      }
      loop++;
   }
   if (ps.vrstate != VRSTATE1) loop = 0;


   ps.vrstate = skyCube(ps.vrstate, ps.us2output, ps.us2format); 
/****************************/

}


void setupEnvironment(Bounds b, int raspace, int despace, float radius)
/**** MODFIY: The following function is a place-holder.  Minimum */
/* requirement is to set the coordinate range using s2swin().    */
/* Additional data can be passed by modifying argument list.  */ 
{

   s2swin(b.min.x,b.max.x, b.min.y,b.max.y, b.min.z,b.max.z);

/* EXAMPLE: A spherical coordinate grid */

   s2sci(S2_PG_LTGREY);         /* Set colour of coordinate grid */
   pushVRMLname("Grid");
   us2radegrid(raspace, despace, radius);    /* Draw coordiante grid */

   
   pushVRMLname("ANON");

/***************************/
   
}

void randomSphere(long Npds, XYZ *xyz, float radius)
/* Random position data values on a sphere with the given radius. */
/* No attempt is made to provide uniform coverage on the sphere.  */
{
   long i; 
   for (i=0;i<Npds;i++) {
      xyz[i].x = drand48()*24.0;
      xyz[i].y = drand48()*180.0-90.0;
      xyz[i]   = us2rade2xyz(xyz[i].x, xyz[i].y, radius);
   }
}

void parseCSVfileXYZC(char *us2input, long *Npds, int  *Ncategory)
/* Parse a CSV file with format: x, y, z, category */
{
   FILE *fp = fopen(us2input,"r");
   *Npds = 0;
   *Ncategory = 0;
   char string[255];
   fgets(string,255,fp);
   XYZ xyz; 
   int cat = 0;
   while (!feof(fp)) {
      sscanf(string,"%lf,%lf,%lf,%d",&xyz.x,&xyz.y,&xyz.z,&cat);
      if (cat > *Ncategory) { *Ncategory = cat; }
      (*Npds)++;
      fgets(string,255,fp);
   }
   fclose(fp);
}

void readCSVfile(char *us2input, long Npds, XYZ *xyz, int *cat, float *radius,
				int off) 
{
   FILE *fp = fopen(us2input,"r");
   char string[255];

   long i;
   for (i=0;i<Npds;i++) {
      fgets(string,255,fp);
      sscanf(string,"%lf,%lf,%lf,%d,%f",&xyz[i].x,&xyz[i].y,&xyz[i].z,
			&cat[i],&radius[i]);
      cat[i]   += off;
      xyz[i].x /= 15.0;
      xyz[i]    = us2rade2xyz(xyz[i].x, xyz[i].y, xyz[i].z);
   }
   fclose(fp);
}

char **readFormatFile(char *us2format, int Ncat, COLOUR *col)
{
   FILE *fp = fopen(us2format, "r");
   int i;
   char string[255];
   int r,g,b; 
 
   char **catName = (char **)calloc(Ncat, sizeof(char *));
   char tmpName[255]; 
   i = 0;

   for (i=0;i<Ncat;i++) {
      fgets(string,255,fp);
      if (feof(fp)) {
         fprintf(stderr,"Warning: insufficient category definitions in %s\n",
			us2format);
      } else {
         sscanf(string,"CAT=%d,%d,%d,%s",&r, &g, &b, tmpName);
         col[i].r = (double)r/255.0;
         col[i].g = (double)g/255.0;
         col[i].b = (double)b/255.0;
         catName[i] = (char *)calloc(strlen(tmpName)+1,sizeof(char));
         sprintf(catName[i],"%s",tmpName);
      }
   }


#ifdef ORIG
   int idx;
   fgets(string,255,fp);
   while (!feof(fp)) {
      if (string[0] == 'C') {	/* Found a colour entry */
         sscanf(string,"CAT=%d,%d,%d",&r, &g, &b);
	 if (idx < Ncat) {
            col[idx].r = (double)r/255.0;
            col[idx].g = (double)g/255.0;
            col[adx].b = (double)b/255.0;
	    idx++;
         } else {
            fprintf(stderr,"Warning: Ignoring additional category colour\n");
            fprintf(stderr,"         in format file: %s\n",us2format);
         }
      }
      fgets(string,255,fp);
   }
#endif
   fclose(fp);

   return catName;
}

char **initTextures(int Ncat, char *tex)
{
   char **tname = (char **)calloc(Ncat, sizeof(char *));
   int i;
   for (i=0;i<Ncat;i++) {
      tname[i] = (char *)calloc(strlen(tex)+1, sizeof(char));
      sprintf(tname[i],"%s",tex);
   }
   return tname;
}

/* To ensure correct output from anaglpyh mode, use the S2PLOT_WIDTH and S2PLOT_HEIGHT environment */
/* variables to define the window dimensions.  Modify eye-separation with [,] keys. */
int main(int argc, char *argv[])
{
   srand48((long)time(NULL)); 		/* Seed the random number generator */

/*** US2PUBLISH: REQUIRED ***/
   ps = initPublishState(argc, argv);
   ps = us2opend(argc, argv, ps);	/* Use this version instead of s2opend(...) */
/****************************/


/* Establish the coordinate system boundaries.  Modify contents and */
/* arguments of the setupEnvironment function as required for your  */
/* application.  */
   Bounds bounds = initBounds(-1.0,+1.0, -1.0,+1.0, -1.0,+1.0);
   float radius  = 1.0;		/* Radius of spherical coordinate system */
   int raspace   = 2;		/* Spacing for Right Ascension */
   int despace   = 10;		/* Spacing for Declination */
   setupEnvironment(bounds, raspace, despace, radius);

/* Create a default data set comprising Npds elements, Ncategory data       */
/* categories and display data points as spheres. Random (default) values   */
/* are assigned for position, category and category colour.  These default  */
/* values should be overwritten through customised initilisation functions: */
/* e.g. reading from a data-file. pds1 is a Global Variable so that it can  */
/* be accessed from within S2PLOT callbacks.  */
   int set       = TEXTURE_SET;
   long Npds     = 0;
   int Ncategory = 0;
   float tsize   = TEXTURE_DEFAULT;     
   char tbase1[] = "Textures";
   char **tname1;

/* EXAMPLE: Modify the data points to lie on the celestial sphere */
   if (ps.us2input == NULL) {
      Npds      = 1000;		/* Default: randomise */
      Ncategory = 4;
      char basename[512];
      sprintf(basename,"%s/star.tga",_BASEDIRECTORY);
      tname1 = initTextures(Ncategory,(char *)basename);
      pds1 = initPublishableDataSet(set, Npds, Ncategory, tbase1, bounds, 
					tname1, tsize);
      randomSphere(pds1.Npds, pds1.xyz, radius);
      pds1.maxRadius = normaliseRadius(pds1.Npds, pds1.radius, pds1.tsize);
   } else {
      parseCSVfileXYZC(ps.us2input, &Npds, &Ncategory);
      char basename[512];
      sprintf(basename,"%s/star.tga",_BASEDIRECTORY);
      tname1 = initTextures(Ncategory,(char *)basename);
      pds1 = initPublishableDataSet(set, Npds, Ncategory, tbase1, bounds, 
					tname1, tsize);
      readCSVfile(ps.us2input, pds1.Npds, pds1.xyz, pds1.category, pds1.radius,
			MINUSONE); 
      pds1.maxRadius = normaliseRadius(pds1.Npds, pds1.radius, pds1.tsize);
   }

   if (ps.us2format != NULL) {
      pds1.catName = readFormatFile(ps.us2format, pds1.Ncategory, pds1.colour);
   }   


#ifdef PDS2
/* Create a second data set, this time using the texture type, which    */
/* requires an array of texture filenames (TGA), with one texture per   */
/* category.  pds2 is a Global Variable so that it can be accessed from */
/* within S2PLOT callbacks.  */
   Npds 	 = 1;
   set           = SPHERE_SET;
   Ncategory     = 1;
   tsize         = SPHERE_DEFAULT;	
   char tbase2[] = "Sphere";
   char **tname2 = NULL;
   pds2 = initPublishableDataSet(set, Npds, Ncategory, tbase2, bounds, 
			tname2, tsize);

/* EXAMPLE: Modify the textured data points to lie on the sphere */
   randomSphere(pds2.Npds, pds2.xyz, radius);
   pds2.maxRadius = normaliseRadius(pds2.Npds, pds2.radius, pds2.tsize);
#endif


/*** US2PUBLISH: REQUIRED ***/
   cs2scb(cb);				/* Install the dynamic callback */
   cs2skcb(kcb);			/* Install the keypress callback */
/****************************/


/* Set the ambient lighting to full white and remove any directional lights */
   COLOUR amb = { 1.0, 1.0, 1.0 };
   ss2sl(amb, 0, NULL, NULL, 1);
   ss2srm(SHADE_FLAT);
   
   s2show(1);				/* Start the S2PLOT display loop */

   return 0;
}
