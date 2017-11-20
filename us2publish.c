/*****************************************************************************/
/* us2publish.c                                             November 2017    */
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

#include "us2publish.h"
#include "us2astro.h"


extern PublishState ps;


LabelNode *addMarker(LabelNode *head, XYZ xyz, COLOUR col, XYZ msize)
{
   head->mflag = TRUE;
   head->marker = xyz;
   head->mcol = col;
   head->msize = msize;

   return head;
}

LabelNode *createLabelFreeType(XYZ xyz, COLOUR col, char *label, char *font, 
			LabelNode *next)
{
   char *freetype = getenv((char *)"S2FREETYPE");
   if (freetype == NULL) {
      fprintf(stderr,"Freetype fonts not found.  S2FREETYPE not set. Exiting.\n");
      exit(-1);
   }
   LabelNode *new = (LabelNode *)malloc(sizeof(LabelNode));

   new->label = (char *)calloc(strlen(label)+1,sizeof(char));
   sprintf(new->label,"%s",label);
   new->flabel = NULL;
   new->xyz = xyz; 
   new->col = col;

/* NOTE: It may be necessary to modify the value of _DEFAULTFONT to match your system */
   if (font == NULL) {
      new->tid = ss2ftt(_DEFAULTFONT, new->label, 128, 2);
   } else {
      new->tid = ss2ftt(font, new->label, 128, 2);	
   }
   int h, w; 
   ss2gt(new->tid, &w, &h);
   new->aspect = (float)w/(float)h;

/* Default is to have no marker */
   new->mflag   = FALSE;
   new->marker  = xyz; 
   new->mcol    = col; 
   new->msize.x = 0.0;
   new->msize.y = 0.0;
   new->msize.z = 0.0;

   new->next    = next;

   return new;
}

LabelNode *createLabelLatex(XYZ xyz, COLOUR col, char *label, LabelNode *next)
{
   LabelNode *new = (LabelNode *)malloc(sizeof(LabelNode));
   new->label = (char *)calloc(strlen(label)+1,sizeof(char));
   sprintf(new->label,"%s",label);
   new->xyz = xyz; 
   new->col = col;
   float aspect;
   new->tid = ss2ltt(new->label, &aspect);
   us2palphatexInvert(new->tid);
   new->aspect = aspect;
   new->next = next;

/* Default is to have no marker */
   new->mflag   = FALSE;
   new->marker  = xyz; 
   new->mcol    = col; 
   new->msize.x = 0.0;
   new->msize.y = 0.0;
   new->msize.z = 0.0;

   return new;
}

LabelNode *createLabelTexture(XYZ xyz, COLOUR col, char *texfile, LabelNode *next)
/* CJF: Needs some refinement */
{
   unsigned char *tex;
   LabelNode *new = (LabelNode *)malloc(sizeof(LabelNode));
   new->label = (char *)calloc(strlen(texfile)+1,sizeof(char));
   sprintf(new->label,"%s",texfile);
   new->xyz = xyz; 
   new->col = col;
   new->tid = ss2lt(texfile);
   us2palphatex(new->tid);

   int h, w; 
   tex = ss2gt(new->tid, &w, &h);
   new->aspect = (float)w/(float)h;

/* Default is to have no marker */
   new->mflag   = FALSE;
   new->marker  = xyz; 
   new->mcol    = col; 
   new->msize.x = 0.0;
   new->msize.y = 0.0;
   new->msize.z = 0.0;
    
   new->next = next;
   return new;
}
  


LabelNode *insertLabelFreeType(LabelNode *head, XYZ xyz, COLOUR col, char *label, char *font)
{
   LabelNode *new = createLabelFreeType(xyz, col, label, font, head);
   head = new;
   return head;
}

LabelNode *insertLabelLatex(LabelNode *head, XYZ xyz, COLOUR col, char *label)
{
   LabelNode *new = createLabelLatex(xyz, col, label, head);
   head = new;
   return head;
}

LabelNode *insertLabelTexture(LabelNode *head, XYZ xyz, COLOUR col, char *texfile)
{
   LabelNode *new = createLabelTexture(xyz, col, texfile, head);
   head = new;
   return head;
}


void displayLabels(LabelNode *head, float tsize, XYZ off, float alpha, char trans)
{
   LabelNode *current = head;
   while (current != NULL) {
      ds2vbbp(current->xyz, off, current->aspect, tsize, current->col, current->tid,
			alpha, trans); 
      if (current->mflag == TRUE) {
         ns2line(current->marker.x-current->msize.x, current->marker.y, current->marker.z, 
	         current->marker.x+current->msize.x, current->marker.y, current->marker.z,
		 current->mcol.r, current->mcol.g, current->mcol.b);
         ns2line(current->marker.x, current->marker.y-current->msize.y, current->marker.z, 
	         current->marker.x, current->marker.y+current->msize.y, current->marker.z,
		 current->mcol.r, current->mcol.g, current->mcol.b);
         ns2line(current->marker.x, current->marker.y, current->marker.z-current->msize.z, 
	         current->marker.x, current->marker.y, current->marker.z+current->msize.z,
		 current->mcol.r, current->mcol.g, current->mcol.b);
	
      }
      current = current->next;
   }
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
      case TEXTURE_SET: tagType = NamedBranch; break;
      case SPHERE_SET : tagType = NamedBranch; break;
      case POINT_SET  : tagType = SerialisedNode; break;
      case NamedBranch: tagType = NamedBranch;    break;
      case SerialisedNode : tagType = SerialisedNode; break;
      case VR_TAG     : tagType = UNKNOWN;   break;
      default         : tagType = UNKNOWN;        break;
   }
   ListNode *new = create(tagType, tagName, head);
   head = new;
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



void display(FILE *stream, ListNode *head)
{
   ListNode *current = head;
   while (current != NULL) {
      fprintf(stream,"%d: %s\n",current->tagType,current->tagName);
      current = current->next;
   }
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

void outputTagNamesPDF(FILE *fp, ListNode *head)
{
   ListNode *current = head;
   while (current != NULL) {
      fprintf(fp,"\"%s\" ",current->tagName);
      current = current->next;
   }
}

void outputVisNamesPDF(FILE *fp, ListNode *head)
{
   ListNode *current = head;
   while (current != NULL) {
      if (current->visName != NULL) {
         fprintf(fp,"\"%s\" ",current->visName);
      } else {
         fprintf(fp,"\"%s\" ",current->tagName);
      }
      current = current->next;
   }
}

void outputTagTypes(FILE *fp, ListNode *head)
{
   ListNode *current = head;
   while (current != NULL) {
      fprintf(fp,"%d ",current->tagType);
      current = current->next;
   }
}


PublishState initPublishState(int argc, char *argv[])
{
   PublishState lps;
   lps.pstate = STATE0;
   lps.cstate = NOCAMERA;
   lps.astate = FALSE;
   lps.vrstate = VRSTATE0;
   lps.styleFlag = MEDIA9;

   lps.darkSphere = FALSE;	/* Do not draw the dark sphere */
 
   lps.debug = FALSE;

/* Search for single token arguments */
   int i = 1; 
   while (i<argc) {
      if (strncmp(argv[i],"-m",2)==0) {
         lps.styleFlag = MOVIE15;
      } else if (strncmp(argv[i],"-debug",6)==0) {
         lps.debug = TRUE;
      } else if (strncmp(argv[i],"-darksphere",10)==0) {
         lps.darkSphere = TRUE;	/* Draw a dark sphere on PDF export */
      }
      i++;
   }

/* Search for two-token arguments */
   i = 1;
   lps.us2output = NULL;
   while (i<argc) {
      if (strncmp(argv[i],"-o",2)==0) {
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {	/* Is there another argument? */
	    lps.us2output = (char *)calloc(strlen(argv[i+1])+1,sizeof(char));
            sprintf(lps.us2output,"%s",argv[i+1]);
            fprintf(stderr,"Will write PDF output to PDF-%s and VR output to VR-%s\n",
			argv[i+1], argv[i+1]);
 	    i = argc;
         }
      }
      i++;
   }
   if (lps.us2output == NULL) {
      lps.us2output = (char *)calloc(strlen(_US2OUTPUT)+1,sizeof(char));
      sprintf(lps.us2output,"%s",_US2OUTPUT);
      fprintf(stderr,"No output directory name provided. \n");
      fprintf(stderr,"Will write PDF output to PDF-%s and VR output to VR-%s\n",
			lps.us2output, lps.us2output);
   } 


   lps.us2input = NULL;
   i = 1;
   while (i<argc) {
      if (strncmp(argv[i],"-i",2)==0) {
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {	/* Is there another argument? */
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
         if ((i<(argc-1)) && (argv[i+1][0]!='-')) {	/* Is there another argument? */
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

int us2publishKey(char key)
{
   if (key == 'P') {
        /* Start the state machine now that a PDF is being created */
      ps.pstate = STATE1;
      ps.cstate = SAVECAMERA;
      return 0;                 /* Do not consume the key-press */
   } else if (key == 'S') {
      ps.cstate = SAVECAMERA;
      return 1;                 /* Consume the key-press */
   } else if (key == 'L') {
      ps.cstate = LOADCAMERA;
      return 1;                 /* Consume the key-press */
   } else if (key == 'V') {
      ps.vrstate = VRSTATE1;
      return 1;			/* Consume the key-press */
   }
   return 0;                    /* Do not consume any other key-press */
}



PublishState us2opend(int argc, char *argv[], PublishState lps)
{
   char *device = calloc(32, sizeof(char));

   int i = 1;
   lps.astate = FALSE;

   while (i<argc) {
      if (strcmp(argv[i],"-anaglyph")==0) {
         lps.astate = TRUE;
         i = argc;
      } else {
         i++;
      }
   }

   if (lps.astate == TRUE) {
      sprintf(device, "/s2dsana");
      FILE *fp = fopen(_CAMERAFILE,"r");
      if (fp != NULL) {
         char string[128];
         fgets(string,128,fp);
         int width, height;
         sscanf(string,"dim = %d %d",&width,&height);
         sprintf(string,"%d\n",width);
         setenv("S2PLOT_WIDTH",string,1);
         sprintf(string,"%d\n",height);
         setenv("S2PLOT_HEIGHT",string,1);
         fclose(fp);
         lps.cstate = LOADCAMERA;
      }
      argc = argc-1;
   } else {
      sprintf(device,"/?");
   }

   s2opend(device,argc,argv);

   if (device != NULL) {
      free(device);
      device = NULL;
   }

   return lps;
}




void publishDataSet(PublishableDataSet lpds, int tagflag)
{
   long i;
   int j;
   XYZ stretch = { 0.0, 0.0, 0.0 };

   if (tagflag) {
      char vrname[32];
      for (j=0;j<lpds.Ncategory;j++) {
         sprintf(vrname,"%s%d",lpds.tagbase,j);
         pushVRMLname(vrname);
         for (i=0;i<lpds.Npds;i++) {
            if (lpds.category[i] == j) {
               switch (lpds.set) {
                  case POINT_SET : ns2vthpoint(lpds.xyz[i], lpds.colour[lpds.category[i]], 
					lpds.radius[i]);
                                   break;
                  case SPHERE_SET: ns2vsphere(lpds.xyz[i], lpds.radius[i], 
					lpds.colour[lpds.category[i]]);
                                   break;
                  case TEXTURE_SET: ds2vbb(lpds.xyz[i], stretch, lpds.radius[i],
                                        lpds.colour[lpds.category[i]], lpds.tid[lpds.category[i]],
                                        lpds.alpha, lpds.trans);
                                   break;

               }
            }
         }
      }
      pushVRMLname("ANON");
   } else {
      switch(lpds.set) {
         case POINT_SET  : pushVRMLname("Points"); break;
         case SPHERE_SET : pushVRMLname("Spheres"); break;
         case TEXTURE_SET: pushVRMLname("Textures"); break;
      }
      for (j=0;j<lpds.Ncategory;j++) {
         for (i=0;i<lpds.Npds;i++) {
            if (lpds.category[i] == j) {
               switch (lpds.set) {
                  case POINT_SET : ns2vthpoint(lpds.xyz[i], lpds.colour[lpds.category[i]],
					lpds.radius[i]);
                                   break;
                  case SPHERE_SET: ns2vsphere(lpds.xyz[i], lpds.radius[i], 
					lpds.colour[lpds.category[i]]);
                                   break;
                  case TEXTURE_SET: ds2vbb(lpds.xyz[i], stretch, lpds.radius[i],
                                        lpds.colour[lpds.category[i]], 
					lpds.tid[lpds.category[i]],
                                        lpds.alpha, lpds.trans);
                                   break;
               }
            }
         }
      }
      pushVRMLname("ANON");
   }
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

   if (set == TEXTURE_SET) {
      lpds.tname = (char **)calloc(Ncategory, sizeof(char *));
      lpds.tid   = (unsigned int *)calloc(Ncategory, sizeof(unsigned int));
      for (i=0;i<Ncategory;i++) {
         lpds.tname[i] = (char *)calloc(strlen(tname[i])+1,sizeof(char));
         sprintf(lpds.tname[i],"%s",tname[i]);
         lpds.tid[i] = ss2lt(tname[i]);
         us2palphatex(lpds.tid[i]);
      }
      lpds.alpha = 0.7;                 /* Modify these values elsewhere */
      lpds.trans = 's';                 /* Modify these values elsewhere */
   } else {
      lpds.tname = NULL;
      lpds.tid = NULL;
   }
   return lpds;
}

void toggleText(FILE *fp, char *oname, char *spacer, int styleFlag)
{
   fprintf(fp,"while ($n <= $nmax)\n");
   fprintf(fp,"   set tname = \"$tlist[$n]\"\n");
   fprintf(fp,"   set vname = \"$vlist[$n]\"\n");
   fprintf(fp,"   set jtype = \"$jlist[$n]\"\n");
   fprintf(fp,"   set serialised = %d\n",SerialisedNode);
   fprintf(fp,"   set namedbranch = %d\n",NamedBranch);
   fprintf(fp,"   set unknown = %d\n",UNKNOWN);
   fprintf(fp,"   set pname = `grep -m 1 ${tname} %s-s2direct.map | tr \"}\" \" \" | cut -d \".\" -f2`\n",oname);

   if (styleFlag == MEDIA9) {	/* media9.sty */
      fprintf(fp,"   set astring1=\"annotRM['fig:\"\n");
      fprintf(fp,"   set astring2=\"'\"\n");
   } else if (styleFlag == MOVIE15) {   /* movie15.sty  */
      fprintf(fp,"   sed -i%s\"\" 's/TOGGLE'${n}'/\\\\movieref[3Djscript=%s-toggle'{$n}'.js]{fig:'$oname'}{'$vname'}/' $oname-publish.tex\n",spacer,oname);
   }

   fprintf(fp,"   if (${jtype} == ${serialised}) then\n");
   fprintf(fp,"      cp toggleSerialised.js %s-toggle${n}.js\n",oname);
   fprintf(fp,"      sed -i%s\"\" 's/FIGNAME/'$oname'/' %s-toggle${n}.js\n",spacer,oname);
   fprintf(fp,"      sed -i%s\"\" 's/TNAME/'$tname'/' %s-toggle${n}.js\n",spacer,oname);
   fprintf(fp,"      sed -i%s\"\" 's/PNAME/'$pname'/' %s-toggle${n}.js\n",spacer,oname);
   if (styleFlag == MEDIA9) {
      fprintf(fp,"      sed -i%s\"\" 's/TOGGLE'${n}'/\\\\PushButton[onclick={'$astring1''$oname''$astring2'].context3D.toggleVisibilityOfSerialisedNode(\"'$tname'.'$pname'\");}]{'$vname'}/' $oname-publish.tex\n",spacer);
   }
   fprintf(fp,"   else if (${jtype} == ${namedbranch}) then\n");
   fprintf(fp,"      cp toggleNamedBranch.js %s-toggle${n}.js\n",oname);
   fprintf(fp,"      sed -i%s\"\" 's/FIGNAME/'$oname'/' %s-toggle${n}.js\n",spacer,oname);
   fprintf(fp,"      sed -i%s\"\" 's/TNAME/'$tname'/' %s-toggle${n}.js\n",spacer,oname);
   if (styleFlag == MEDIA9) {
      fprintf(fp,"      sed -i%s\"\" 's/TOGGLE'${n}'/\\\\PushButton[onclick={'$astring1''$oname''$astring2'].context3D.toggleVisibilityOfNamedBranch(\"'$tname'\");}]{'$vname'}/' $oname-publish.tex\n",spacer);
   }
   fprintf(fp,"   else if (${jtype} == ${unknown}) then\n");
   fprintf(fp,"      cp toggleNamedBranch.js %s-toggle${n}.js\n",oname);
   fprintf(fp,"      sed -i%s\"\" 's/FIGNAME/'$oname'/' %s-toggle${n}.js\n",spacer,oname);
   fprintf(fp,"      sed -i%s\"\" 's/TNAME/'$tname'/' %s-toggle${n}.js\n",spacer,oname);
   if (styleFlag == MEDIA9) {
      fprintf(fp,"      sed -i%s\"\" 's/TOGGLE'${n}'/\\\\PushButton[onclick={'$astring1''$oname''$astring2'].context3D.toggleVisibilityOfNamedBranch(\"'$tname'\");}]{'$tname'}/' $oname-publish.tex\n",spacer);
   }
   fprintf(fp,"   endif\n");
   fprintf(fp,"   @ n = $n + 1\n");
   fprintf(fp,"end \n");
}

void makeBuildScript(char *scriptname, char *snapshot, char *outputname, 
			ListNode *tagList, int styleFlag)
{

   if (styleFlag==MOVIE15) {
      fprintf(stderr,"Building for movie15.sty\n");
   } else {
      fprintf(stderr,"Building for media9.sty\n");
   }

   FILE *fp = fopen(scriptname, "w");
   fprintf(fp,"#! /bin/csh -f\n");

   int w, h;
   float width, height;
   ss2qsd(&w, &h);

   fprintf(fp,"set oname=\"%s\"\n",outputname);
   if (w > h) {
      height = (float)h/(float)w;
      width = 1.0;
   } else {
      width = (float)w/(float)h;
      height = 1.0;
   }

   char spacer[3] = "";
   struct utsname sysname;
   uname(&sysname);
   if (strcmp((char *)sysname.sysname,"Darwin")==0) {
      sprintf(spacer," ");
   }

/* Select the LaTeX skeleton */
   fprintf(fp,"cp simple.tex $oname-publish.tex\n");

/* Convert the screen grab into a poster frame */
   fprintf(fp,"convert %s.tga $oname-poster.png\n",snapshot);

/* Set variable values that are passed into the LaTeX skeleton */

   fprintf(fp,"sed -i%s\"\" 's/FIGNAME/'$oname'/' $oname-publish.tex\n",spacer);
   if (styleFlag == MEDIA9){ /* Repeat for media9.sty */
      fprintf(fp,"sed -i%s\"\" 's/FIGNAME/'$oname'/' $oname-publish.tex\n",spacer);
   }
   fprintf(fp,"sed -i%s\"\" 's/HEIGHT/%.2f/' $oname-publish.tex\n",spacer,height);
   fprintf(fp,"sed -i%s\"\" 's/WIDTH/%.2f/' $oname-publish.tex\n\n",spacer,width);
   fprintf(fp,"if (-f %s.tga) then\n",_ANAGLYPHNAME);
   fprintf(fp,"   convert %s.tga $oname-anaglyph.png\n",_ANAGLYPHNAME);
   fprintf(fp,"   sed -i%s\"\" 's/%%%%ANAGLYPH//' $oname-publish.tex\n",spacer);
   fprintf(fp,"endif\n\n");

   int Nt = count(tagList);
   if (Nt > 0) {

      fprintf(fp,"sed -i%s\"\" 's/TDESC/Clicking a label name below the figure toggles display of the corresponding model part. Additional parts of the model can be accessed through the model tree.  /' $oname-publish.tex\n",spacer);
      fprintf(fp,"sed -i%s\"\" 's/TOGGLE/",spacer);

      int i;
      for (i=0;i<Nt;i++) {
         fprintf(fp,"TOGGLE%d \\\\hspace{0.5cm}",i+1);
         if ((i>0) && (i%5==0)) fprintf(fp,"\\\\\\\\ ");
      }
      fprintf(fp,"/' $oname-publish.tex\n\n");

      fprintf(fp,"set tlist = ( ");
      outputTagNamesPDF(fp, tagList); 
      fprintf(fp,")\n\n");

      fprintf(fp,"set vlist = ( ");
      outputVisNamesPDF(fp, tagList); 
      fprintf(fp,")\n\n");
      

      fprintf(fp,"set jlist = ( ");
      outputTagTypes(fp, tagList);
      fprintf(fp,")\n\n");
      fprintf(fp,"@ n = 1\n");
      fprintf(fp,"@ nmax = %d\n\n",Nt);

      toggleText(fp, outputname, spacer, styleFlag);

   } else {
      fprintf(fp,"sed -i%s\"\" 's/TDESC//' $oname-publish.tex\n",spacer);
      fprintf(fp,"sed -i%s\"\" 's/TOGGLE//' $oname-publish.tex\n",spacer);
   }

/* Run pdflatex multiple times */
   fprintf(fp,"pdflatex $oname-publish.tex\n");
   fprintf(fp,"pdflatex $oname-publish.tex\n");
   if (styleFlag == MEDIA9) {
      fprintf(fp,"pdflatex $oname-publish.tex\n");
   }

/* Mac OSX: Automatically open Adobe Reader */
   if (strcmp((char *)sysname.sysname,"Darwin")==0) {
      fprintf(fp,"open -a \"%s\" $oname-publish.pdf\n",_ADOBESTRING);
   }

   fclose(fp);

   char string[128];
   sprintf(string,"chmod 755 %s; ./%s %s %s %d\n",
		scriptname,_MAKERSCRIPT,outputname,scriptname, styleFlag);
   system(string);


}


int generateScripts(PublishState lps, char *oname, ListNode *tagList)
{
   if (lps.pstate == STATE1) {
   /* Need to wait for one frame before exporting the snapshot */
      ss2txh(0);
      cs2thv(0);
      lps.pstate = STATE2;
   } else if (lps.pstate == STATE2) {
      ss2txh(0);
      cs2thv(0);

      if (lps.astate == TRUE) {
         ss2wtga(_ANAGLYPHNAME);
         char string[128]; 
         sprintf(string,"cp %s.tga PDF-%s",_ANAGLYPHNAME,oname); 
         system(string); 
         sprintf(string,"cd PDF-%s; ./%s\n",oname,_BUILDSCRIPT);
         system(string); 

      } else {
         ss2wtga(_SNAPSHOT);
         makeBuildScript(_BUILDSCRIPT,_SNAPSHOT,oname,tagList,lps.styleFlag);
      }
      lps.pstate = STATE0;
   }

   if (lps.cstate == SAVECAMERA) {
      int width, height;
      ss2qsd(&width,&height);

      XYZ pos, up, vd, foc;   
      int wc = 0, set = 0;
      ss2qc(&pos, &up, &vd, wc); 
      ss2qcf(&set, &foc, wc);


      FILE *cfp = fopen(_CAMERAFILE, "w");
      if (cfp == NULL) {
         fprintf(stderr,"Cannot create camera file: %s\n",_CAMERAFILE);
	 return lps.pstate;
      }
      fprintf(cfp,"dim  = %d %d\n",width,height);
      fprintf(cfp,"pos  = %10.4lf %10.4lf %10.4lf\n",pos.x, pos.y, pos.z);
      fprintf(cfp,"up   = %10.4lf %10.4lf %10.4lf\n",up.x, up.y, up.z);
      fprintf(cfp,"view = %10.4lf %10.4lf %10.4lf\n",vd.x, vd.y, vd.z);
      fprintf(cfp,"foc  = %10.4lf %10.4lf %10.4lf\n",foc.x, foc.y, foc.z);
      fclose(cfp);
   }

   return lps.pstate;
}


void readCamera(void)
{
   FILE *cfp = fopen(_CAMERAFILE, "r");
   char string[256];
   XYZ pos, up, vd, foc;
   int wc = 1;
   if (cfp == NULL) {
      fprintf(stderr,"Cannot read camera file: %s\n",_CAMERAFILE);
      return;
   }

   int width, height;
   fgets(string,256,cfp);
   sscanf(string,"dim  = %d %d\n",&width, &height);
   fgets(string,256,cfp);
   sscanf(string,"pos  = %lf %lf %lf\n",&pos.x, &pos.y, &pos.z);
   fgets(string,256,cfp);
   sscanf(string,"up   = %lf %lf %lf\n",&up.x, &up.y, &up.z);
   fgets(string,256,cfp);
   sscanf(string,"view = %lf %lf %lf\n",&vd.x, &vd.y, &vd.z);
   fgets(string,256,cfp);
   sscanf(string,"foc  = %lf %lf %lf\n",&foc.x, &foc.y, &foc.z);

   ss2scaf(pos, up, vd, foc, wc);
   fclose(cfp);
}

void us2publish(char *oname, ListNode *tagList)
{
   ps.pstate = generateScripts(ps, oname, tagList);

   if (ps.cstate == LOADCAMERA) {
      readCamera();
   }
   ps.cstate = NOCAMERA;
}

void us2palphatexInvert(unsigned int id)
{
   int w, h, i, j;
   unsigned char *tex;
   tex = ss2gt(id, &w, &h);
   int idx;
   for (i=0;i<h;i++) {
      for (j=0;j<w;j++) {
         idx = (i*w + j)*4;
         tex[idx + 3] = 255-(int)((tex[idx] + tex[idx+1] + tex[idx+2])/3.0);
         tex[idx] = tex[idx+1] = tex[idx+2] = 255;
      }
   }
   ss2pt(id);
}

void us2palphatex(unsigned int id)
{
   int w, h, i, j;
   unsigned char *tex;
   tex = ss2gt(id, &w, &h);
   int idx;
   for (i=0;i<h;i++) {
      for (j=0;j<w;j++) {
         idx = (i*w + j)*4;
         tex[idx + 3] = (int)((tex[idx] + tex[idx+1] + tex[idx+2])/3.0);
         tex[idx] = tex[idx+1] = tex[idx+2] = 255;
      }
   }
   ss2pt(id);
}




int skyCube(int vrstate, char *target, char *us2format)
{
   static XYZ spos, sup, svd;
   static float sca;
   if (vrstate == VRSTATE0) {
      ss2qc(&spos, &sup, &svd, 0);
      sca = ss2qca();
   }

   if (vrstate >= 0) {
      cs2thv(0);
      ss2txh(0);
      ss2sca(90.0);
   } else {
      ss2sca(sca);
   }
   static int loop = 0;
   int loopwait = 2;

   if (vrstate == VRSTATE1) {
      float ar = ss2qar();
      if (fabs(ar-1.0) > 1E-4) {
         fprintf(stderr,"Cannot generate sky-cube for aspect ratio: %f\n",ar);
         fprintf(stderr,"Check values of SPLOT_WIDTH and S2PLOT_HEIGHT\n");
         vrstate = -1;
         loop = 0;
         ss2sca(sca);
         ss2sc(spos, sup, svd, 0);
         return vrstate;
      }
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 1, 0 };
      XYZ vd = { 0, 0, +1 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("posz");
         vrstate = VRSTATE2;
         loop = 0;
      } else {
         loop++;
      }
   } else if (vrstate == VRSTATE2) {
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 1, 0 };
      XYZ vd = { 0, 0, -1 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("negz");
         vrstate = VRSTATE3;
         loop = 0;
      } else {
         loop++;
      }
   } else if (vrstate == VRSTATE3) {
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 1, 0 };
      XYZ vd = { +1, 0, 0 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("posx");
         vrstate = VRSTATE4;
         loop = 0;
      } else {
         loop++;
      }
   } else if (vrstate == VRSTATE4) {
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 1, 0 };
      XYZ vd = { -1, 0, 0 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("negx");
         vrstate = VRSTATE5;
         loop = 0;
      } else {
         loop++;
      }
   } else if (vrstate == VRSTATE5) {
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 0, -1 };
      XYZ vd = { 0, 1, 0 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("posy");
         vrstate = VRSTATE6;
         loop = 0;
      } else {
         loop++;
      }
   } else if (vrstate == VRSTATE6) {
      XYZ pos = { 0.0, 0.0, 0.0 };
      XYZ up = { 0, 0, +1 };
      XYZ vd = { 0, -1, 0 };
      ss2sc(pos,up,vd,1);
      if (loop == loopwait) {
         ss2wtga("negy");
         char string[512];
	 if (us2format != NULL) {
            sprintf(string,"./us2menumaker VR-%s 1",target);
            system(string);
         } else {
            sprintf(string,"./us2menumaker VR-%s",target);
            system(string);
         }
         sprintf(string,"./us2vrmaker.csh VR-%s",target);
         system(string);
         vrstate = VRSTATE0;
         loop  = 0;
         ss2sca(sca);
         ss2sc(spos, sup, svd, 0);
      } else {
         loop++;
      }
   }
   return vrstate;
}


void polygon2origin(XYZ xyz, XYZ up, XYZ right, float sx, float sy,
                        COLOUR col, unsigned int tid, char trans)
{
   XYZ P[4];
   up     = unit(up);
   right  = unit(right);
   P[1].x = xyz.x - sx*right.x - sy*up.x;
   P[0].x = xyz.x + sx*right.x - sy*up.x;
   P[3].x = xyz.x + sx*right.x + sy*up.x;
   P[2].x = xyz.x - sx*right.x + sy*up.x;
   P[1].y = xyz.y - sx*right.y - sy*up.y;
   P[0].y = xyz.y + sx*right.y - sy*up.y;
   P[3].y = xyz.y + sx*right.y + sy*up.y;
   P[2].y = xyz.y - sx*right.y + sy*up.y;
   P[1].z = xyz.z - sx*right.z - sy*up.z;
   P[0].z = xyz.z + sx*right.z - sy*up.z;
   P[3].z = xyz.z + sx*right.z + sy*up.z;
   P[2].z = xyz.z - sx*right.z + sy*up.z;
   ns2vf4x(P,col,tid,1.0,trans);
}

void publishDataSetSkyCube(PublishableDataSet lpds)
{
   int j;
   long i;
   XYZ pos, up, vd;
   ss2qc(&pos, &up, &vd, 1);
   XYZ right, lup;

   for (j=0;j<lpds.Ncategory;j++) {
      for (i=0;i<lpds.Npds;i++) {
         if (lpds.category[i] == j) {
            right = cross(up, lpds.xyz[i]);
            lup   = cross(right, lpds.xyz[i]);
            polygon2origin(lpds.xyz[i],lup,right,
			lpds.radius[i]*lpds.maxRadius,
			lpds.radius[i]*lpds.maxRadius,
                        lpds.colour[lpds.category[i]],
                        lpds.tid[lpds.category[i]],
                        lpds.trans);
         }
      }
   }
}


void displayLabelsSkyCube(int vrstate, LabelNode *head, float tsize, 
			char trans, float rah)
{
   LabelNode *current = head;
   XYZ pos, up, vd;
   ss2qc(&pos, &up, &vd, 1);
   XYZ right, lup;
   float theta  = (rah-6.0)*(15.0*M_PI/180.0);          /* Radians */
   float stheta = sin(theta), ctheta = cos(theta);
   float tolerance = 0.95;

   while (current != NULL) {
      float sx = tsize, sy = sx/current->aspect;
      if (vrstate < VRSTATE5) {
         right = cross(up, current->xyz);
         lup   = cross(right, current->xyz);
      } else {
         float l1 = hypot(current->xyz.x,hypot(current->xyz.y,current->xyz.z));
         float dotp = (vd.x*current->xyz.x + vd.y*current->xyz.y
                                + vd.z*current->xyz.z)/l1;
         if (fabs(dotp) > tolerance) {
            lup.x =  up.x * ctheta + up.z*stheta;
            lup.z = -up.x * stheta + up.z*ctheta;
            lup.y = up.y;
            right = cross(vd,lup);
            if (vd.y < tolerance) {
               sx *= -1.0;
               sy *= -1.0;
            }
         } else {
            right = cross(vd, current->xyz);
            lup   = cross(right, current->xyz);
         }
         if (vd.y < tolerance) {
            sx *= -1.0;
            sy *= -1.0;
         }
      }
      polygon2origin(current->xyz,lup,right,sx,sy, current->col,current->tid,trans);

      if (current->mflag == TRUE) {
         ns2line(current->marker.x-current->msize.x, current->marker.y, current->marker.z,
                 current->marker.x+current->msize.x, current->marker.y, current->marker.z,
                 current->mcol.r, current->mcol.g, current->mcol.b);
         ns2line(current->marker.x, current->marker.y-current->msize.y, current->marker.z,
                 current->marker.x, current->marker.y+current->msize.y, current->marker.z,
                 current->mcol.r, current->mcol.g, current->mcol.b);
         ns2line(current->marker.x, current->marker.y, current->marker.z-current->msize.z,
                 current->marker.x, current->marker.y, current->marker.z+current->msize.z,
                 current->mcol.r, current->mcol.g, current->mcol.b);

      }
      current = current->next;
   }
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

void exportToAFrame(char *aname, char *mode, PublishableDataSet lpds, 
		float radius, ListNode *tagList)
{
   FILE *fp = fopen(aname,mode);
   FILE *tfp = fopen("taglist.txt",mode);
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
               case POINT_SET   : scale     = POINTSIZE;
                                  segWidth  = POINTSEG;
                                  segHeight = POINTSEG;
                                  break;
               case SPHERE_SET  : scale     = lpds.tsize;
                                  segWidth  = SPHERESEG;
                                  segHeight = SPHERESEG;
                                  break;
               case TEXTURE_SET : scale     = lpds.tsize*2.0;
                                  segWidth  = TEXTURESEG;
                                  segHeight = TEXTURESEG;
                                  break;
            }
            fprintf(fp,"   <a-entity id=\"%s-%ld\" ",lpds.tagbase,i);
            fprintf(fp,"geometry=\"primitive:sphere; radius: %.2f; ", radius*scale);
            fprintf(fp,"segmentsWidth: %d; segmentsHeight: %d\" ", segWidth, segHeight);
            fprintf(fp,"material=\"wireframe: false; color: %s\" ",hstring);
            fprintf(fp,"position=\"%.4f %.4f %.4f\"></a-entity>\n",
                        lpds.xyz[i].x*radius, lpds.xyz[i].y*radius, lpds.xyz[i].z*radius);
         }
      }
      fprintf(fp,"</a-entity>\n");
      if (hstring != NULL) { free(hstring); hstring = NULL; }
   }
   fprintf(fp,"</a-entity>\n");
   fclose(fp);



   int Nt = count(catList);
   if (Nt > 0) {
      outputTagNamesVR(tfp, catList);
   }
   fclose(tfp);

   if (catList != NULL) {
      delete(catList);
   }
}


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

