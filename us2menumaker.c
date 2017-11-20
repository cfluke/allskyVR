/*****************************************************************************/
/* us2menumaker.c                                           November 2017    */
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

#include <stdio.h>
#include <stdlib.h>
#include "s2plot.h"
#include "us2publish.h"

/* US2PUBLISH: REQUIRED */
PublishState ps;
int gicube = VRSTATE0;
/************************/

#define TARGET "VR-us2output"
#define SKYCUBE  "SkyCube"
#define MENU     "Menu"

#define MENUEXPORT "us2menuexport.csh"
#define ASSETS	   "us2assets.html"
#define LINKS      "us2links.html"

#define SX     0.4
#define SY     0.4
#define SZ     0.4
#define RADIUS 0.8

#define S2_WIDTH  "800"
#define S2_HEIGHT "800"
#define S2_FTIME  "0"

LabelNode *labelList = NULL;
ListNode *tagList = NULL;

char *target;
int format = 0;


int countLabel(LabelNode *head)
{
   int N = 0;
   LabelNode *current = head;
   while (current != NULL) {
      N++;
      current = current->next;
   }
   return N;
}

void cb(double *t, int *kc)
{
   char trans = 's';

   LabelNode *current = labelList;
   ss2tsc("clr");
   XYZ xyz;
   XYZ P[4];
   float width = 0.05;
   float height = 0.05;

   int sw, sh;
   ss2qsd(&sw, &sh);
   
   static int loop = 0;
   static FILE *fp = NULL;
   static FILE *afp = NULL;
   static FILE *lfp = NULL;
   if (loop == 0) {
      fp = fopen(MENUEXPORT,"w"); 
      afp = fopen(ASSETS,"w"); 
      lfp = fopen(LINKS,"w"); 
      fprintf(fp,"#! /bin/csh -f\n");
   }

   int Nt = countLabel(labelList); 
   float radius = RADIUS;
   float dphi = 2.0*M_PI/(float)(Nt-1);
   XYZ pos;

   XYZ scale = { SX, SY, SZ };

   int s2height = atoi(S2_HEIGHT);
   
   int idx = 0;
   if (loop == 1) {
      while (current != NULL) {
         xyz = current->xyz;
         P[0].z = P[1].z = P[2].z = P[3].z = xyz.x;
         height = width/current->aspect;
         P[0].x = P[3].x = xyz.x - width;
         P[1].x = P[2].x = xyz.x + width;
         P[0].y = P[1].y = xyz.y + height;
         P[2].y = P[3].y = xyz.y - height;
         ns2vf4x(P, current->col, current->tid, 1.0, trans);

         P[0].x = P[3].x = xyz.x - width*1.05;
         P[1].x = P[2].x = xyz.x + width*1.05;
         P[0].y = P[1].y = xyz.y + width*1.05;
         P[2].y = P[3].y = xyz.y - width*1.05;
         s2wcube(P[0].x,P[1].x, P[0].y, P[2].y, P[0].z,P[0].z);

         P[0].x = P[3].x = xyz.x - width*1.20;
         P[1].x = P[2].x = xyz.x + width*1.20;
         P[0].y = P[1].y = xyz.y + width*1.20;
         P[2].y = P[3].y = xyz.y - width*1.20;
    
         int a = (int)floor(P[2].x*sw)-(int)floor(P[0].x*sw); 
         int b = (int)floor(P[1].y*sh)-(int)floor(P[2].y*sh); 
         if (a%2 == 1) a++;
         if (b%2 == 1) b++;

         fprintf(fp,"convert -crop %dx%d+%d+%d labelme.tga %s.jpg\n",
		a,b,
		(int)floor(P[0].x*sw),s2height-(int)floor(P[0].y*sh), 
                current->flabel);
         fprintf(fp,"mv %s.jpg %s\n",current->flabel,target);

         fprintf(afp,"\t<img id=\"%s\" crossorigin=\"anonymous\" ",
		current->flabel);
         fprintf(afp,"src=\"%s.jpg\">\n",current->flabel);

         if (strcmp(current->label,MENU) != 0) {
	    pos.x = radius*cos(-(float)idx*dphi);
            pos.y = radius*sin(-(float)idx*dphi);
            pos.z = -4.0;
            fprintf(lfp,"<a-entity template=\"src: #link\" ");
	    fprintf(lfp,"data-target=\"#Set-%s\" ",current->flabel);
            fprintf(lfp,"data-thumb=\"#%s\" ",current->flabel);
            fprintf(lfp,"position=\"%.4lf %.4lf %.4lf\" scale=\"%.2lf %.2lf %.2lf\"",
		pos.x,pos.y,pos.z,scale.x,scale.y,scale.z);
            fprintf(lfp,"></a-entity>\n");
         }
         current = current->next;
         idx++;
     }
   }
   ss2tsc("");

   if (loop == 2) {
     ss2wtga("labelme");
   }

   loop++;
   if (loop > 4) {
      fclose(fp);
      fclose(afp);
      fprintf(lfp,"   </a-entity>\n");
      fprintf(lfp,"</a-entity>\n");
      fclose(lfp);
      exit(0);
   }
}

int main(int argc, char *argv[])
{
   if (argc == 2) {
      target = (char *)calloc(strlen(argv[1])+1,sizeof(char));
      sprintf(target,"%s",argv[1]);
      format = 0;
   } else if (argc == 3) {
      target = (char *)calloc(strlen(argv[1])+1,sizeof(char));
      sprintf(target,"%s",argv[1]);
      format = 1;
   } else {
      target = (char *)calloc(strlen(TARGET)+1,sizeof(char));
      sprintf(target,"%s",TARGET);
      format = 0;
   }

   setenv("S2PLOT_WIDTH",  S2_WIDTH, 1);
   setenv("S2PLOT_HEIGHT", S2_HEIGHT, 1);
   setenv("S2PLOT_FADETIME", S2_FTIME, 1);


   s2opend("/s2mono",argc,argv);
   s2swin(-1,1,-1,1,-1,1);

   char font[128];
   sprintf(font,"%s",_DEFAULTFONT);

   COLOUR labelcol = { 1.0, 1.0, 0.0 }; /* Label colour */
   XYZ xyz = { 0.0, 0.0, 0.05 };
   xyz.y = 0.1;

   FILE *fp = fopen("taglist.txt","r");
   char string[128], label[128];

/* These two must exist */
   tagList = insert(tagList, UNKNOWN, MENU); 
   if (format) {
      tagList->visName = tagList->tagName;
      tagList->col = labelcol;
   }
   tagList = insert(tagList, UNKNOWN, SKYCUBE);
   if (format) {
      tagList->visName = tagList->tagName;
      tagList->col = labelcol;
   }

   fgets(string, 128, fp);
   COLOUR col;
   char text[32];
   while (!feof(fp)) {
      if (format == 0) {
         sscanf(string,"%s %lf %lf %lf",label,&col.r, &col.g, &col.b);
      } else {
         sscanf(string,"%s %lf %lf %lf %s",label, &col.r, &col.g, &col.b, text);
      }
      tagList = insert(tagList, UNKNOWN, label);
      tagList->col = col;

      if (format == 1) {
         tagList->visName = (char *)calloc(strlen(text)+1,sizeof(char));
         sprintf(tagList->visName,"%s",text);
      }
      fgets(string, 128, fp);
   }
   fclose(fp);
 
   xyz.x = 0.1;
   ListNode *current = tagList;
   while (current != NULL) {
      if (format == 0) {
         labelList = insertLabelFreeType(labelList, xyz, current->col,
			current->tagName, font);
      } else {
         labelList = insertLabelFreeType(labelList, xyz, current->col,
			current->visName, font);
      }
      labelList->flabel = (char *)calloc(strlen(current->tagName)+1,sizeof(char));
      sprintf(labelList->flabel,"%s",current->tagName);
      xyz.x += 0.2;
      if (xyz.x > 1.0) {
         xyz.x = 0.1;
         xyz.y += 0.2;
      }
      current = current->next;
   }

   COLOUR amb = { 1.0, 1.0, 1.0 };
   ss2sl(amb, 0, NULL, NULL, 1);

   cs2scb(cb);
   s2show(1);
   return 1;
}
