/*  DEC/CMS REPLACEMENT HISTORY, Element FIR.C */
/*  *14   15-MAR-1995 11:20:29 MDSPLUS "Compatibility" */
/*  *13   14-MAR-1995 16:59:45 MDSPLUS "ANSI C" */
/*  *12   13-MAR-1995 17:32:31 MDSPLUS "New Version" */
/*  *11   16-NOV-1994 12:44:29 MDSPLUS "OSF/1 compatibile" */
/*  *10   15-NOV-1994 16:32:25 MDSPLUS "Ported to OSF1" */
/*  *9    15-NOV-1994 15:16:57 MDSPLUS "Move to OSF1" */
/*  *8    11-NOV-1994 13:43:07 MDSPLUS "Proceede" */
/*  *7    10-NOV-1994 15:35:40 MDSPLUS "Proceede" */
/*  *6     3-NOV-1994 10:06:24 MDSPLUS "Fix" */
/*  *5     3-NOV-1994 09:17:54 MDSPLUS "Fix" */
/*  *4     3-NOV-1994 08:54:31 MDSPLUS "Fix" */
/*  *3     3-NOV-1994 08:38:34 MDSPLUS "Fix" */
/*  *2     3-NOV-1994 08:35:44 MDSPLUS "Fix" */
/*  *1     2-NOV-1994 18:03:24 MDSPLUS "FIR management routines" */
/*  DEC/CMS REPLACEMENT HISTORY, Element FIR.C */
/*------------------------------------------------------------------------------

	Name:	FIR   

	Type:   C function

	Author:	Gabriele Manduchi
		Istituto Gas Ionizzati del CNR - Padova (Italy)

	Date:    2-NOV-1994

	Purpose: Compute FIR coefficients using the following windows:

	Rectangular;
	Bartlett;
	Hanning;
	Hamming;
	Blackmann.
	
------------------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include "filter.h"
#include "complex.h"

static Filter *Fir(double fc, double s_f, int n, void (*Window) (double *, int));
static void Rectangular(double *w, int n);
static void Bartlett(double *w, int n);
static void Hanning(double *w, int n);
static void Hamming(double *w, int n);
static void Blackmann(double *w, int n);

Filter *FirRectangular(float *fc, float *s_f, int *n)
{
  return Fir(*fc, *s_f, *n, Rectangular);
}

Filter *FirBartlett(float *fc, float *s_f, int *n)
{
  return Fir(*fc, *s_f, *n, Bartlett);
}

Filter *FirHanning(float *fc, float *s_f, int *n)
{
  return Fir(*fc, *s_f, *n, Hanning);
}

Filter *FirHamming(float *fc, float *s_f, int *n)
{
  return Fir(*fc, *s_f, *n, Hamming);
}

Filter *FirBlackmann(float *fc, float *s_f, int *n)
{
  return Fir(*fc, *s_f, *n, Blackmann);
}

static Filter *Fir(double fc, double s_f, int n, void (*Window) (double *, int))
{
  int i;
  Filter *filter;
  double *w, wc, alpha;

  wc = 2 * PI * fc / s_f;

  filter = (Filter *) malloc(sizeof(Filter));
  filter->num_parallels = 1;
  filter->units = (FilterUnit *) malloc(sizeof(FilterUnit));
  filter->units[0].num_degree = n;
  filter->units[0].den_degree = 0;
  filter->units[0].num = (double *)malloc(n * sizeof(double));
  w = (double *)malloc(n * sizeof(double));
  Window(w, n);
  alpha = (n - 1) * 0.5;
  for (i = 0; i < n; i++)
    if (fabs(i - alpha) > 1E-6)
      filter->units[0].num[i] = w[i] * sin(wc * (i - alpha)) / (PI * (i - alpha));
    else
      filter->units[0].num[i] = wc / PI;
  free((char *)w);
  return filter;
}

static void Rectangular(double *w, int n)
{
  int i;
  for (i = 0; i < n; i++)
    w[i] = 1;
}

static void Bartlett(double *w, int n)
{
  int i;
  for (i = 0; i <= (n - 1) / 2; i++)
    w[i] = 2 * i / (double)(n - 1);
  for (; i < n; i++)
    w[i] = 2 - 2 * i / (double)(n - 1);
}

static void Hanning(double *w, int n)
{
  int i;
  for (i = 0; i < n; i++)
    w[i] = 0.5 * (1 - cos(2 * PI * i / (double)(n - 1)));
}

static void Hamming(double *w, int n)
{
  int i;
  for (i = 0; i < n; i++)
    w[i] = 0.54 - 0.46 * cos(2 * PI * i / (double)(n - 1));
}

static void Blackmann(double *w, int n)
{
  int i;
  for (i = 0; i < n; i++)
    w[i] =
	0.42 - 0.5 * cos(2 * PI * i / (double)(n - 1)) + 0.08 * cos(4 * PI * i / (double)(n - 1));
}
