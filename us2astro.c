/*****************************************************************************/
/* us2astro.c                                               November 2017    */
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


#include "us2astro.h"



XYZ us2rade2xyz(float ra, float dec, float radius)
/* Converts a RA (0..24) and Dec (-90..+90) to an XYZ point */
{
   XYZ xyz;
   double p, t, rsinp;

   p = (DEMAX - dec)*D2R;             	/* Convert to radians */
   t = ra*RA2R;                   	/* Convert to radians */

   rsinp =  radius * sin(p);           	/* Save one calculation */
   xyz.x =  rsinp  * cos(t);
   xyz.z = -rsinp  * sin(t);
   xyz.y =  radius * cos(p);

   return xyz;
}



/**** s2pt-style functions ****/
/**** Use s2sci and s2sch - single colour and size for all symbols */

void us2radept1(float ra, float dec, float radius, int symbol)
{
   XYZ xyz;
   xyz = us2rade2xyz(ra, dec, radius);
   s2pt1(xyz.x, xyz.y, xyz.z, symbol);
}

void us2radept(int n, float *ra, float *dec, float radius, int symbol)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz(ra[i], dec[i], radius);
      s2pt1(xyz.x, xyz.y, xyz.z, symbol);
   }
}


void us2radeptrs(int n, float *ra, float *dec, float *radius, int *symbol)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz(ra[i], dec[i], radius[i]);
      s2pt1(xyz.x, xyz.y, xyz.z, symbol[i]);
   }
}



/**** ns2point-style functions ****/

void us2radepoint(float ra, float dec, float radius, float red, float green, 
		float blue)
{
   XYZ xyz = us2rade2xyz(ra, dec, (float)radius);
   ns2point(xyz.x, xyz.y, xyz.z, red, green, blue);
}

void us2radethpoint(float ra, float dec, float radius, float red, float green, 
		float blue, float size)
{
   XYZ xyz = us2rade2xyz(ra, dec, radius);
   ns2thpoint(xyz.x, xyz.y, xyz.z, red, green, blue, size);
}

void us2radevpoint(XYZ rade, COLOUR col)
{
   XYZ xyz = us2rade2xyz((float)rade.x, (float)rade.y, (float)rade.z);
   ns2vpoint(xyz, col);
}

void us2radevthpoint(XYZ rade, COLOUR col, float size)
{
   XYZ xyz = us2rade2xyz((float)rade.x, (float)rade.y, (float)rade.z);
   ns2vthpoint(xyz, col, size);
}

void us2radevnpoint(XYZ *rade, COLOUR col, int n)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz((float)rade[i].x, (float)rade[i].y, (float)rade[i].z);
      ns2vpoint(xyz, col);
   }
}

void us2radevnthpoint(XYZ *rade, COLOUR col, float size, int n)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz((float)rade[i].x, (float)rade[i].y, (float)rade[i].z);
      ns2vthpoint(xyz, col, size);
   }
}

void us2radevnpointc(XYZ *rade, COLOUR *col, int n)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz((float)rade[i].x, (float)rade[i].y, (float)rade[i].z);
      ns2vpoint(xyz, col[i]);
   }
}


void us2radevnthpointc(XYZ *rade, COLOUR *col, float size, int n)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz((float)rade[i].x, (float)rade[i].y, (float)rade[i].z);
      ns2vthpoint(xyz, col[i], size);
   }
}

void us2radevnthpointct(XYZ *rade, COLOUR *col, float *size, int n)
{
   XYZ xyz;
   int i;
   for (i=0;i<n;i++) {
      xyz = us2rade2xyz((float)rade[i].x, (float)rade[i].y, (float)rade[i].z);
      ns2vthpoint(xyz, col[i], size[i]);
   }
}


void us2alphatex(unsigned int id)
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

void us2radevtexture(XYZ rade, float size, COLOUR col, unsigned int tid, 
			float alpha, char trans)
{
   XYZ xyz = us2rade2xyz((float)rade.x, (float)rade.y, (float)rade.z);
   XYZ stretch = { 0.0, 0.0, 0.0 };		/* No stretching of texture */
   
   ds2vbb(xyz, stretch, size, col, tid, alpha, trans); 
}





#define RASTEPS                  48
#define DESTEPS                  48
#define DEPS                     1.0E-6 /* A small number */
void us2deline(float de, float ramin, float ramax, float radius)
{
   float rr = ramin*RA2R;
   float p1 = radius * sin((DEMAX-de)*D2R);
   float p2 = radius * cos((DEMAX-de)*D2R);

   float d1, d2; 
   float deg = (ramax - ramin)*15.0;
   int nseg = 100;

   XYZ centre = { 0.0, 0.0, 0.0 };
   XYZ normal = { 0.0, radius, 0.0 };
   XYZ start  = { p1, p2, 0.0 };

   /* Rotate the starting vector to ramin */
   d1 =  cos(rr) * start.x; 
   d2 = -sin(rr) * start.x; 
   start.x = d1;
   start.z = d2;

   if ((radius < 1.0) && (ramin <= 0)  && (ramax >= 24.0)) {
      s2circxz(centre.x,start.y,0, start.x, nseg, 1.0);
   } else {
      ns2varc(centre, normal, start, deg, nseg);
   }
/*
   ns2arc(centre.x, centre.y, centre.z,
	  normal.x, normal.y, normal.z, 
          0.5, 0.5, 0.0,
	   deg, nseg);
*/

}

void us2raline(float ra, float demin, float demax, float radius)
{
   us2radeline(ra, demin, ra, demax, radius);
}

void us2radegrid(int rastep, int destep, float radius)
{
   int i;
   if (rastep > 0) {
      for (i=RAMIN;i<=RAMAX;i+=rastep) {
         us2raline((float)i, DEMIN, DEMAX, radius);
      }
   }

   if (destep > 0) {
      for (i=DEMIN;i<=DEMAX;i+=destep) {
         us2deline((float)i, RAMIN, RAMAX, radius);
      }
   }
}

void us2radeline(float ra1, float de1, float ra2, float de2, float radius)
{
   XYZ xyz1, xyz2;
   XYZ centre = { 0.0, 0.0, 0.0 };
   XYZ normal;
   float angle;
   int nseg = 100;

   xyz1 = us2rade2xyz(ra1, de1, radius);
   xyz2 = us2rade2xyz(ra2, de2, radius);

   normal = cross(xyz1, xyz2);
   angle  = dota(xyz1, xyz2) * R2D;


   s2slw(2);
   ns2varc(centre, normal, xyz1, angle, nseg);
   s2slw(1);
   
} 


void us2radearrow(double ra, double de, double r1, double r2, int ci, int sz)
{  
   double p, t, rsinp; 
   float x[2], y[2], z[2];
   
   p = (90.0 - de)*D2R; 
   t = ra*15.0*D2R;
   
   rsinp = r1 * sin(p);
   x[0] = rsinp * cos(t);
   y[0] = rsinp * sin(t);
   z[0] = r1 * cos(p); 

   rsinp = r2 * sin(p);
   x[1] = rsinp * cos(t);
   y[1] = rsinp * sin(t); 
   z[1] = r2 * cos(p);
   
   s2sci(ci);
   s2sch(sz);
   s2arro(x[0],y[0],z[0],x[1],y[1],z[1]);
   s2sch(1);
}

XYZ cross(XYZ a, XYZ b)
{
   XYZ c;
   c.x = a.y*b.z - a.z*b.y;
   c.y = a.z*b.x - a.x*b.z;
   c.z = a.x*b.y - a.y*b.x;

   return c;
}

#define REPS 1.0E-6
XYZ unit(XYZ a)
{
   XYZ b;
   float r;
   r = length(a);
   if (fabs(r) < REPS) {
      b.x = b.y = b.z = 0;
   } else {
      b.x = a.x/r;
      b.y = a.y/r;
      b.z = a.z/r;
   }
   return b;
}

float dotp(XYZ a, XYZ b)
{
   return (a.x*b.x + a.y*b.y + a.z*b.z);
}

float length(XYZ a)
{
   return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

float dota(XYZ a, XYZ b)
{
   XYZ a1 = unit(a);
   XYZ b1 = unit(b);
   float d = dotp(a1, b1);
   return acos(d);
}


int countLines(FILE *fp)
{
   int N = 0;
   long int ptr;
   char string[1024];

   ptr = ftell(fp);

   fgets(string,1024,fp);
   while (!feof(fp)) { fgets(string,1024,fp); N++; }
   fseek(fp, ptr, SEEK_SET);            /* Back to where file was */

   return N;
}



float **initSlice(int nx, int ny)
/* Allocate memory and initialise a data cube */
{
   float **slice;
   int i, j;

   slice = (float **)malloc(nx * sizeof(float *));
   if (slice == NULL) {
      fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float *));
      exit(-1);
   }
   for (i=0;i<nx;i++) {
      slice[i] = (float *)malloc(ny * sizeof(float));
      if (slice[i] == NULL) {
         fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float));
         exit(-1);
      }
      for (j=0;j<ny;j++) {
         slice[i][j] = 0.0;
      }
   }

   return slice;
}

void equalizeVolume(float ***volume, int M, float vmin, float vmax)
{  
   int i, j, k;
   float *pdf, *cdf;
   int maxn; 
   int idx;

   maxn = (int)(vmax+1.0);
   pdf = (float *)calloc(maxn, sizeof(float));
   cdf = (float *)calloc(maxn, sizeof(float));
   for (i=0;i<maxn;i++) {
      pdf[i] = 0; 
      cdf[i] = 0;
   }        
            
   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            idx = (int)round(volume[i][j][k]);
            pdf[idx]+=1.0;
         }
      }
   }
   cdf[0] = pdf[0];
   for (i=1;i<maxn;i++) {
      cdf[i] = cdf[i-1] + pdf[i];
   }
   float alpha = maxn/(float)(M*M*M);

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            idx = (int)round(volume[i][j][k]);
            volume[i][j][k] = cdf[idx]*alpha;
         }
      }
   }
}

void equalizeSlice(float **slice, int M, float smin, float smax)
{  
   int i, j;
   float *pdf, *cdf;
   int maxn; 
   int idx;

   maxn = (int)(smax+1.0);
   pdf = (float *)calloc(maxn, sizeof(float));
   cdf = (float *)calloc(maxn, sizeof(float));
   for (i=0;i<maxn;i++) {
      pdf[i] = 0; 
      cdf[i] = 0;
   }        
            
   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         idx = (int)round(slice[i][j]);
         pdf[idx]+=1.0;
      }
   }
   cdf[0] = pdf[0];
   for (i=1;i<maxn;i++) {
      cdf[i] = cdf[i-1] + pdf[i];
   }
   float alpha = maxn/(float)(M*M);

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         idx = (int)round(slice[i][j]);
         slice[i][j] = cdf[idx]*alpha;
      }
   }
}

void maskVolume(float ***volume, int M, float clip, float value)
{
   int i, j, k;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            if (volume[i][j][k] < clip) volume[i][j][k] = value;
         }
      }
   }
}

void maskSlice(float **slice, int M, float clip, float value)
{
   int i, j;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         if (slice[i][j] < clip) slice[i][j] = value;
      }
   }
}

void normaliseSlice(float **slice, int M, float smin, float smax, float scale)
{
   int i, j;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         slice[i][j] = scale*(slice[i][j]-smin)/(smax-smin);
      }
   }
}

void normaliseVolume(float ***volume, int M, float vmin, float vmax, 
		float scale)
{
   int i, j, k;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            volume[i][j][k] = scale*(volume[i][j][k]-vmin)/(vmax-vmin);
         }
      }
   }
}

void normaliseVolume3(float ***volume, int Mx, int My, int Mz, 
		float vmin, float vmax, float scale)
{
   int i, j, k;

   for (i=0;i<Mx;i++) {
      for (j=0;j<My;j++) {
         for (k=0;k<Mz;k++) {
            volume[i][j][k] = scale*((volume[i][j][k]-vmin)/(vmax-vmin));
         }
      }
   }
}

void sqrtVolume(float ***volume, int M)
{
   int i, j, k;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            volume[i][j][k] = sqrt(fabs(volume[i][j][k]));
         }
      }
   }
}

void log10Volume3(float ***volume, int Mx, int My, int Mz)
{
   int i, j, k;

   for (i=0;i<Mx;i++) {
      for (j=0;j<My;j++) {
         for (k=0;k<Mz;k++) {
            volume[i][j][k] = log10(volume[i][j][k]+1.0);
         }
      }
   }
}

void log10Volume(float ***volume, int M)
{
   int i, j, k;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            volume[i][j][k] = log10(volume[i][j][k]+1.0);
         }
      }
   }
}

void log10Slice(float **slice, int M)
{
   int i, j;

   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         slice[i][j] = log10(slice[i][j]+1.0);
      }
   }
}


XYZ minmaxVolumeCell(float ***volume, int M, float *rvmin, float *rvmax)
{
   int i, j, k;
   float vmin, vmax;
   XYZ cell;

   vmin = volume[0][0][0];
   vmax = volume[0][0][0];
   cell.x = cell.y = cell.z = 0;
   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            if (volume[i][j][k] < vmin) { 
               vmin = volume[i][j][k];
            }
            if (volume[i][j][k] > vmax) {
               vmax = volume[i][j][k];
	       cell.x = i;
	       cell.y = j;
	       cell.z = k;
            }
         }
      }
   }
   *rvmin = vmin;
   *rvmax = vmax;
   return cell;
}

void minmaxVolume3(float ***volume, int Mx, int My, int Mz, float *rvmin, float *rvmax)
{
   int i, j, k;
   float vmin, vmax;

   vmin = volume[0][0][0];
   vmax = volume[0][0][0];
   for (i=0;i<Mx;i++) {
      for (j=0;j<My;j++) {
         for (k=0;k<Mz;k++) {
            vmin = (volume[i][j][k] < vmin) ? volume[i][j][k] : vmin;
            vmax = (volume[i][j][k] > vmax) ? volume[i][j][k] : vmax;
         }
      }
   }
   *rvmin = vmin;
   *rvmax = vmax;
}

void minmaxVolume(float ***volume, int M, float *rvmin, float *rvmax)
{
   int i, j, k;
   float vmin, vmax;

   vmin = volume[0][0][0];
   vmax = volume[0][0][0];
   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         for (k=0;k<M;k++) {
            vmin = (volume[i][j][k] < vmin) ? volume[i][j][k] : vmin;
            vmax = (volume[i][j][k] > vmax) ? volume[i][j][k] : vmax;
         }
      }
   }
   *rvmin = vmin;
   *rvmax = vmax;
}

void minmaxSlice(float **slice, int M, float *rsmin, float *rsmax)
{
   int i, j;
   float smin, smax;

   smin = slice[0][0];
   smax = slice[0][0];
   for (i=0;i<M;i++) {
      for (j=0;j<M;j++) {
         smin = (slice[i][j] < smin) ? slice[i][j] : smin;
         smax = (slice[i][j] > smax) ? slice[i][j] : smax;
      }
   }
   *rsmin = smin;
   *rsmax = smax;
}



float ***initVolume(int nx, int ny, int nz)
/* Allocate memory and initialise a data cube */
{
   float ***volume;
   int i, j;

   float *zero;
   zero = (float *)malloc(nz *sizeof(float));
   for (i=0;i<nz;i++) {
      zero[i] = -20.0;
   }

   volume = (float ***)malloc(nx * sizeof(float **));
   if (volume == NULL) {
      fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float **));
      exit(-1);
   }
   for (i=0;i<nx;i++) {
      volume[i] = (float **)malloc(ny * sizeof(float *));
      if (volume[i] == NULL) {
         fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float *));
         exit(-1);
      }
      for (j=0;j<ny;j++) {
         volume[i][j] = (float *)malloc(nz * sizeof(float));
         if (volume[i][j] == NULL) {
            fprintf(stderr,"Failed to allocate %ld bytes\n",nx*sizeof(float));
            exit(-1);
         }
	 memcpy(volume[i][j], zero, nz*sizeof(float)); 
		/* Fill with zeroes! */
      }
   }

   if (zero != NULL) { free(zero); zero = NULL; }

   return volume;
}

float hsl2rgb(float T, float S, float L)
/* 
   Hue is between 0.0 and 360.0 
   Call as:   
      R = hsl2rgb(hue/360.0 + 1.0/3.0, S, L);
      G = hsl2rgb(hue/360.0          , S, L);
      B = hsl2rgb(hue/360.0 - 1.0/3.0, S, L);
*/
{
   float Q, P;

   if (L< 0.5) Q = L * (1.0 + S);
   else Q = L + S - (L*S);
   P = 2.0*L - Q;
   if (T < 0) T = T + 1.0;
   if (T < (1.0/6.0)) { return (P + ((Q-P)*6.0*T)); }
   if (T < 0.5) { return Q; }
   if (T < (2.0/3.0)) { return (P + ((Q-P)*(2.0/3.0 - T) * 6.0)); }
   return P;
}



