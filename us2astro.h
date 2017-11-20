/*****************************************************************************/
/* us2astro.h                                               November 2017    */
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
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifndef __S2ASTRO_H__
#define __S2ASTRO_H__

#include "s2plot.h"


/* CONSTANTS */
#define D2R  0.017453292519943295       /* Conversion of degrees to radians */
#define RA2R 0.261799387799149408	/* Conversion of RA (hrs) to radians */
#define R2D 57.295779513082322865       /* Conversion of radians to degrees */

#define DEMIN			-90.0		/* Maximum Declination in degrees */
#define DEMAX                   +90.0   	/* Maximum Declination in degrees */
#define RAMIN                     0.0   	/* Minimum RA value in hours */
#define RAMAX                    24.0   	/* Maximum RA value in hours */

/* FUNCTION PROTOTYPES */

/* Coordinate functions */

/* Convert RA (hours) and Declination (degrees) into XYZ coordinates */
/*   for a sphere of given radius */
XYZ us2rade2xyz(float ra, float dec, float radius);



/*** s2pt-style functions using colour and size set by s2sci and s2sch ***/

void us2radept1(float ra, float dec, float radius, int symbol);
void us2radept(int n, float *ra, float *dec, float radius, int symbol);
void us2radeptrs(int n, float *ra, float *dec, float *radius, int *symbol);

/**** ns2point-style functions ****/

void us2radepoint(float ra, float dec, float radius, float red, float green, float blue);
void us2radethpoint(float ra, float dec, float radius, float red, float green, float blue, float size);
void us2radevpoint(XYZ rade, COLOUR col);
void us2radevthpoint(XYZ rade, COLOUR col, float size);
void us2radevnpoint(XYZ *rade, COLOUR col, int n);
void us2radevnthpoint(XYZ *rade, COLOUR col, float size, int n);
void us2radevnpointc(XYZ *rade, COLOUR *col, int n);
void us2radevnthpointc(XYZ *rade, COLOUR *col, float size, int n);
void us2radevnthpointct(XYZ *rade, COLOUR *col, float *size, int n);





/* Draw a line at constant RA (hours) between Declinations (degrees) */
/*   demin and demax. Sphere has radius r */
void us2raline(float ra, float demin, float demax, float r);

/* Draw a line at constant Declination (degrees) between RAs (hours) */
/*   ramin and ramax. Sphere has radius r */
void us2deline(float de, float ramin, float ramax, float r);

/* Draw a RA (hour) and Declination (degrees) grid with integer steps in */
/*  RA and Declination.  Sphere has radius r */
void  us2radegrid(int rastep, int destep, float radius);

/* Draw an arrow between radius r1 and radius r2 for constant RA (hour) */
/*   and Declination (degrees). Arrow has colour ci and size sz  */
void us2radearrow(double ra, double de, double r1, double r2, int ci, int sz);

XYZ unit(XYZ a);
float dotp(XYZ a, XYZ b);
float length(XYZ a);
float dota(XYZ a, XYZ b);
XYZ cross(XYZ a, XYZ b);
void us2radeline(float ra1, float de1, float ra2, float de2, float radius);

int countLines(FILE *fp);

float ***initVolume(int nx, int ny, int nz);
void equalizeVolume(float ***volume, int M, float vmin, float vmax);
void log10Volume(float ***volume, int M);
void log10Volume3(float ***volume, int Mx, int My, int Mz);
void sqrtVolume(float ***volume, int M);
void normaliseVolume(float ***volume, int M, float vmin, float vmax,
	float scale);
void normaliseVolume3(float ***volume, int Mx, int My, int Mz, 
	float vmin, float vmax, float scale);
void minmaxVolume(float ***volume, int M, float *rvmin, float *rvmax);
void minmaxVolume3(float ***volume, int Mx, int My, int Mz, float *rvmin, float *rvmax);
XYZ minmaxVolumeCell(float ***volume, int M, float *rvmin, float *rvmax);
void maskVolume(float ***volume, int M, float clip, float value);

void maskSlice(float **slice, int M, float clip, float value);
void minmaxSlice(float **slice, int M, float *rsmin, float *rsmax);
void normaliseSlice(float **slice, int M, float smin, float smax, float scale);
void log10Slice(float **slice, int M);
float **initSlice(int nx, int ny);
void equalizeSlice(float **slice, int M, float smin, float smax);
float hsl2rgb(float T, float S, float L);

void us2alphatex(unsigned int id);
void us2radevtexture(XYZ rade, float size, COLOUR col, unsigned int tid,
                        float alpha, char trans);

#endif
