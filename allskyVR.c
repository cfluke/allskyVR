/**************************************************************************************/
/* allskyVR.C                                                         November 2017   */
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


/* WARNING: This initial version is largely comment-free... */

#include "allskyVR.h"

PublishState ps;
PublishableDataSet pds1;

#define ASSETS   "us2assets.html"
#define LINKS    "us2links.html"
#define SKYCUBE  "SkyCube"
#define MENU     "Menu"

#define LRADIUS 0.8
#define SX      0.3
#define SY      0.3
#define SZ      0.3

Bounds initBounds(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
   Bounds b;
   b.min.x = xmin;
   b.min.y = ymin;
   b.min.z = zmin;
   b.max.x = xmax;
   b.max.y = ymax;
   b.max.z = zmax;
   return b;
}

XYZ rade2xyz(float ra, float dec, float radius)
/* Converts a RA (0..24) and Dec (-90..+90) to an XYZ point */
{
   XYZ xyz;
   double p, t, rsinp;

   p = (DEMAX - dec)*D2R;               /* Convert to radians */
   t = ra*RA2R;                         /* Convert to radians */

   rsinp =  radius * sin(p);            /* Save one calculation */
   xyz.x =  rsinp  * cos(t);
   xyz.z = -rsinp  * sin(t);
   xyz.y =  radius * cos(p);

   return xyz;
}


float normaliseRadius(long N, float *radius, float tsize)
{
   float maxRadius = radius[0];
   long i;
   for (i=1;i<N;i++) {
      if (radius[i] > maxRadius) maxRadius = radius[i];
   }
   if (maxRadius <= 0) {
      fprintf(stderr,"Warning: cannot use a negative radius\n");
      return 1.0;
   }

   for (i=0;i<N;i++) {
      radius[i] = tsize*(radius[i]/maxRadius);
   }
   return maxRadius;
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
      xyz[i]    = rade2xyz(xyz[i].x, xyz[i].y, xyz[i].z);
   }
   fclose(fp);
}


char **readFormatFile(char *us2format, int Ncat, COLOUR *col)
{
   FILE *fp = fopen(us2format, "r");
   int i, idx=0;
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

   fclose(fp);

   return catName;
}


PublishableDataSet initPublishableDataSet(int set, long Npds, int Ncategory,
                char *tagbase, Bounds b, char *tname[], float tsize)
{
   PublishableDataSet lpds;
   long i;

   lpds.set  = set;
   lpds.Npds = Npds;
   lpds.Ncategory = Ncategory;
   lpds.tsize = tsize;

   lpds.catName  = NULL;

   lpds.xyz      = (XYZ *)calloc(Npds, sizeof(XYZ));
   lpds.radius   = (float *)calloc(Npds, sizeof(float));
   lpds.maxRadius = -1;

   lpds.category = (int *)calloc(Npds, sizeof(int));
   lpds.count    = (int *)calloc(Npds, sizeof(int));
   for (i=0;i<Npds;i++) {
      lpds.count[i] = 0;
   }
 

  for (i=0;i<Npds;i++) {
      lpds.category[i] = (int)floor(drand48()*Ncategory);
      lpds.count[lpds.category[i]]++;
      lpds.xyz[i].x = drand48()*(b.max.x-b.min.x)+b.min.x;
      lpds.xyz[i].y = drand48()*(b.max.y-b.min.y)+b.min.y;
      lpds.xyz[i].z = drand48()*(b.max.z-b.min.z)+b.min.z;
      lpds.radius[i] = 1.0;
   }

/* Initialise category colour to random. Will be overwritten if format fileo */
/* is provided */
   lpds.colour = (COLOUR *)calloc(Ncategory, sizeof(COLOUR));
   for (i=0;i<Ncategory;i++) {
      lpds.colour[i].r = drand48();
      lpds.colour[i].g = drand48();
      lpds.colour[i].b = drand48();
   }

   sprintf(lpds.tagbase,"%s",tagbase);

   return lpds;
}


ListNode *create(int tagType, char *tagName, ListNode *next)
{
   ListNode *new = (ListNode *)malloc(sizeof(ListNode));
   new->tagType = tagType;
   new->tagName = (char *)calloc(strlen(tagName)+1,sizeof(char));
   new->visName = NULL;
   new->col.r = 1.0;
   new->col.g = 1.0;
   new->col.b = 1.0;
   sprintf(new->tagName,"%s",tagName);
   new->next = next;
   return new;
}


ListNode *insert(ListNode *head, int tagType, char *tagName)
{
   switch (tagType) {
      default         : tagType = UNKNOWN;        break;
   }
   ListNode *new = create(tagType, tagName, head);
   head = new;
   return head;
}

ListNode *insertN(ListNode *head, int tagType, char *base, int N)
{
   int i;
   char *dstring;
   dstring = (char *)calloc(strlen(base)+10, sizeof(char));
   for (i=(N-1);i>=0;i--) {
      sprintf(dstring,"%s%d",base,i);
      head = insert(head, tagType, dstring);
   }
   if (dstring != NULL) { free(dstring); dstring = NULL; }
   return head;
}

ListNode *insertNwithName(ListNode *head, int tagType, char *base, int N,
                char **catName)
{
   int i;
   char *dstring;
   dstring = (char *)calloc(strlen(base)+10, sizeof(char));
   for (i=(N-1);i>=0;i--) {
      sprintf(dstring,"%s%d",base,i);
      head = insert(head, tagType, dstring);
      head->visName = (char *)calloc(strlen(catName[i])+1, sizeof(char));
      sprintf(head->visName,"%s",catName[i]);
   }
   if (dstring != NULL) { free(dstring); dstring = NULL; }
   return head;
}

char *makeHexString(COLOUR col)
{
   char *hstring = (char *)calloc(32, sizeof(char));
   char chr[4], chg[4], chb[4];

   unsigned int hr = (unsigned int)floor(col.r*255);
   unsigned int hg = (unsigned int)floor(col.g*255);
   unsigned int hb = (unsigned int)floor(col.b*255);

   if (hr < 16) sprintf(chr,"0%x",hr); else sprintf(chr,"%x",hr);
   if (hg < 16) sprintf(chg,"0%x",hg); else sprintf(chg,"%x",hg);
   if (hb < 16) sprintf(chb,"0%x",hb); else sprintf(chb,"%x",hb);

   sprintf(hstring,"#%s%s%s",chr,chg,chb);

   return hstring;

}

void delete(ListNode *this)
{
   if (this == NULL) {
/* End of the list */
      return;
   }
   ListNode *next = this->next;
   free(this);
   this = NULL;
   delete(next);
}

void outputTagNamesVR(FILE *fp, ListNode *head)
{
   ListNode *current = head;
   while (current != NULL) {
      fprintf(fp,"%s %f %f %f %s\n",current->tagName,
                current->col.r, current->col.g, current->col.b,
                current->visName);
      current = current->next;
   }
}

int count(ListNode *head)
{
   int N = 0;
   ListNode *current = head;
   while (current != NULL) {
      N++;
      current = current->next;
   }
   return N;
}

void exportToAFrame(char *aname, char *mode, PublishableDataSet lpds,
                float radius, ListNode *tagList)
{
   FILE *fp = fopen(aname,mode);
   int j;
   long i;
   char *hstring = NULL;
   float scale;
   int segWidth = 4, segHeight = 4;
   ListNode *catList = NULL;

   fprintf(fp,"<a-entity id=Set-%s>\n",lpds.tagbase);
   char catString[256];
   for (j=0;j<lpds.Ncategory;j++) {
      hstring = makeHexString(lpds.colour[j]);
      sprintf(catString,"%s%d",lpds.tagbase,j);
      fprintf(fp,"<a-entity id=\"Set-%s\">\n", catString);

      catList = insert(catList, VR_TAG, catString);
      catList->col = lpds.colour[j];
      if (lpds.catName != NULL) {
         catList->visName = (char *)calloc(strlen(lpds.catName[j])+1,
                                        sizeof(char));

         sprintf(catList->visName,"%s",lpds.catName[j]);
      } else {
         catList->visName = catList->tagName;
      }
      for (i=0;i<lpds.Npds;i++) {
         if (lpds.category[i] == j) {
            switch (lpds.set) {
               case TEXTURE_SET : scale     = lpds.tsize*2.0;
                                  segWidth  = TEXTURESEG;
                                  segHeight = TEXTURESEG;
                                  break;
            }
            fprintf(fp,"   <a-entity id=\"%s-%ld\" ",lpds.tagbase,i);
            fprintf(fp,"geometry=\"primitive:sphere; radius: %.2f; ", 
			radius*scale);
            fprintf(fp,"segmentsWidth: %d; segmentsHeight: %d\" ", 
			segWidth, segHeight);
            fprintf(fp,"material=\"wireframe: false; color: %s\" ",hstring);
            fprintf(fp,"position=\"%.4f %.4f %.4f\"></a-entity>\n",
                        lpds.xyz[i].x*radius, lpds.xyz[i].y*radius, 
			lpds.xyz[i].z*radius);
         }
      }
      fprintf(fp,"</a-entity>\n");
      if (hstring != NULL) { free(hstring); hstring = NULL; }
   }
   fprintf(fp,"</a-entity>\n");
   fclose(fp);


   static int loop = 0;
   static FILE *mfp = NULL;
   static FILE *afp = NULL;
   static FILE *lfp = NULL;
   afp = fopen(ASSETS,"w");
   lfp = fopen(LINKS,"w");

   ListNode *tcurrent = tagList;

   XYZ mscale = { SX, SY, SZ };
   int Nt = count(tagList)+2;
   float lradius = LRADIUS;
   float dphi = 2.0*M_PI/(float)(Nt-1);

   tcurrent = tagList;
   int idx = 0;
   XYZ pos;
   while (tcurrent != NULL) {
      fprintf(afp,"\t<img id=\"%s\" crossorigin=\"anonymous\" ",
                tcurrent->tagName);
      fprintf(afp,"src=\"%s.jpg\">\n",tcurrent->tagName);

      if (strcmp(tcurrent->tagName,MENU) != 0) {
         pos.x = lradius*cos((float)idx*dphi);
         pos.y = lradius*sin((float)idx*dphi);
         pos.z = -4.0;
         fprintf(lfp,"<a-entity template=\"src: #link\" ");
         fprintf(lfp,"data-target=\"#Set-%s\" ",tcurrent->tagName);
         fprintf(lfp,"data-thumb=\"#%s\" ",tcurrent->tagName);
         fprintf(lfp,"position=\"%.4lf %.4lf %.4lf\" scale=\"%.2lf %.2lf %.2lf\"",
                pos.x,pos.y,pos.z,mscale.x,mscale.y,mscale.z);
         fprintf(lfp,"></a-entity>\n");
      }
      tcurrent = tcurrent->next;
      idx++;
   }
   fprintf(afp,"\t<img id=\"%s\" crossorigin=\"anonymous\" ",
                MENU);
   fprintf(afp,"src=\"%s.jpg\">\n",MENU);

   fprintf(afp,"\t<img id=\"%s\" crossorigin=\"anonymous\" ",SKYCUBE);
   fprintf(afp,"src=\"%s.jpg\">\n",SKYCUBE);

   pos.x = lradius*cos((float)idx*dphi);
   pos.y = lradius*sin((float)idx*dphi);
   pos.z = -4.0;
   fprintf(lfp,"<a-entity template=\"src: #link\" ");
   fprintf(lfp,"data-target=\"#Set-%s\" ",SKYCUBE);
   fprintf(lfp,"data-thumb=\"#%s\" ",SKYCUBE);
   fprintf(lfp,"position=\"%.4lf %.4lf %.4lf\" scale=\"%.2lf %.2lf %.2lf\"",
          pos.x,pos.y,pos.z,mscale.x,mscale.y,mscale.z);
   fprintf(lfp,"></a-entity>\n");

   fclose(afp);
   fprintf(lfp,"   </a-entity>\n");
   fprintf(lfp,"</a-entity>\n");
   fclose(lfp);

}


PublishState initPublishState(int argc, char *argv[])
{
   PublishState lps;
   lps.vrstate = VRSTATE0;
   lps.darkSphere = FALSE;      /* Do not draw the dark sphere */

   lps.debug = FALSE;

/* Search for single token arguments */
   int i = 1;

/* Search for two-token arguments */
   i = 1;
   lps.us2output = NULL;
   while (i<argc) {
      if (strncmp(argv[i],"-o",2)==0) {
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {     
					/* Is there another argument? */
            lps.us2output = (char *)calloc(strlen(argv[i+1])+1,sizeof(char));
            sprintf(lps.us2output,"%s",argv[i+1]);
            fprintf(stderr,"Will write VR output to VR-%s\n", argv[i+1]);
            i = argc;
         }
      }
      i++;
   }


   if (lps.us2output == NULL) {
      lps.us2output = (char *)calloc(strlen(_US2OUTPUT)+1,sizeof(char));
      sprintf(lps.us2output,"%s",_US2OUTPUT);
      fprintf(stderr,"No output directory name provided. \n");
      fprintf(stderr,"Will write VR output to VR-%s\n", lps.us2output);
   }

   lps.us2input = NULL;
   i = 1;
   while (i<argc) {
      if (strncmp(argv[i],"-i",2)==0) {
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {     
			/* Is there another argument? */
            lps.us2input = (char *)calloc(strlen(argv[i+1])+1,sizeof(char));
            sprintf(lps.us2input,"%s",argv[i+1]);
            FILE *fp = fopen(lps.us2input,"r");
            if (fp == NULL) {
               fprintf(stderr,"Input file %s does not exist\n",argv[i+1]);
               if (lps.us2input != NULL) {
                  free(lps.us2input);
                  lps.us2input = NULL;
               }
               i = argc;
            } else {
               fclose(fp);
               fprintf(stderr,"Reading input from %s\n", argv[i+1]);
            }
            i = argc;
         }
      }
      i++;
   }
   if (lps.us2input == NULL) {
      fprintf(stderr,"No valid input filename provided. \n");
      fprintf(stderr,"Will operate in debug mode with random data\n");
      lps.debug = TRUE;
   }
   lps.us2format = NULL;
   i = 1;
   while (i<argc) {
      if (strncmp(argv[i],"-f",2)==0) {
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {     
				/* Is there another argument? */
            lps.us2format = (char *)calloc(strlen(argv[i+1])+1,sizeof(char));
            sprintf(lps.us2format,"%s",argv[i+1]);
            FILE *fp = fopen(lps.us2format,"r");
            if (fp == NULL) {
               fprintf(stderr,"Input file %s does not exist\n",argv[i+1]);
               if (lps.us2format != NULL) {
                  free(lps.us2format);
                  lps.us2format = NULL;
               }
               i = argc;
            } else {
               fclose(fp);
               fprintf(stderr,"Reading format from %s\n", argv[i+1]);
            }
            i = argc;
         }
      }
      i++;
   }
   if (lps.us2format == NULL) {
      fprintf(stderr,"No valid format filename provided. \n");
      fprintf(stderr,"Will operate in debug mode with random colour values\n");
   }
   return lps;
}


int main(int argc, char *argv[])
{
   srand48((long)time(NULL));           /* Seed the random number generator */

  ps = initPublishState(argc, argv);

/* Establish the coordinate system boundaries.  Modify contents and */
/* arguments of the setupnvironment function as required for your  */
/* application.  */
   Bounds bounds = initBounds(-1.0,+1.0, -1.0,+1.0, -1.0,+1.0);
   float radius  = 1.0;         /* Radius of spherical coordinate system */
   int raspace   = 3;           /* Spacing for Right Ascension */
   int despace   = 15;          /* Spacing for Declination */

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

/* EXAMPLE: Modify the data points to lie on the celestial sphere */
   parseCSVfileXYZC(ps.us2input, &Npds, &Ncategory);
   pds1 = initPublishableDataSet(set, Npds, Ncategory, tbase1, bounds,
                                        NULL, tsize);
   readCSVfile(ps.us2input, pds1.Npds, pds1.xyz, pds1.category, pds1.radius,
                        MINUSONE);
   pds1.maxRadius = normaliseRadius(pds1.Npds, pds1.radius, pds1.tsize);

   if (ps.us2format != NULL) {
      pds1.catName = readFormatFile(ps.us2format, pds1.Ncategory, pds1.colour);
   }

   fprintf(stderr,"Using %s\n",ps.us2format);

   int tagflag = TRUE;
   static LabelNode *labelList = NULL;
   float rah    = 3.0;                  /* Spacing of hour angle for labels */
   int mflag    = TRUE;                 /* Associate markers with labels? */
   COLOUR labelcol = { 1.0, 1.0, 0.0 }; /* Label colour */

   static ListNode *tagList = NULL;
   if (tagList == NULL) {
      if (ps.us2format==NULL) {
         tagList = insertN(tagList, pds1.set, pds1.tagbase, pds1.Ncategory);
      } else {
         tagList = insertNwithName(tagList, pds1.set, pds1.tagbase,
                                pds1.Ncategory, pds1.catName);
      }
   }

   exportToAFrame(_AFRAMEFILE,"w",pds1,VR_RADIUS, tagList);

   char string[256];

   sprintf(string,"./allskyVRmaker.csh VR-%s",ps.us2output);
   system(string);

   return 0;
}

