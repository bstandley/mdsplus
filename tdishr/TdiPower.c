/*      Tdi3Power.C
        Raise number to a power (integral, real, or complex).

        WARNING can destroy inputs, which must differ from output.
        WARNING 0^0 is undefined, for r^r and c^c ROPRAND, for i^i 1. (OTS$POWJJ gives 0.)
        WARNING accuracy of float base not as good as OTS$ routines, which use double.
        Ken Klare, LANL P-4     (c)1990,1991
*/
#include <STATICdef.h>
#include <string.h>
#define _MOVC3(a,b,c) memcpy(c,b,a)
#include <mdsdescrip.h>
#include "tdinelements.h"
#include <tdishr_messages.h>



STATIC_CONSTANT int one = 1;
STATIC_CONSTANT struct descriptor one_dsc = { sizeof(one), DTYPE_L, CLASS_S, (char *)&one };

extern int Tdi3Log();
extern int TdiConvert();
extern int Tdi3Multiply();
extern int Tdi3Exp();
extern int Tdi3Divide();

int Tdi3Power(struct descriptor *x, struct descriptor *y, struct descriptor_a *z)
{
  int status = 1;
  int yy;
  char uno[32];
  struct descriptor duno;

	/*******************************************
        For real/complex exponent, z = EXP(LOG(x)*y)
        Need HC routines, not available on VAX.
        *******************************************/
  if (y->dtype != DTYPE_L) {
    if (x->class != CLASS_A && z->class == CLASS_A) {
      status = TdiConvert(x, z);
      status = Tdi3Log(z, z);
    } else
      status = Tdi3Log(x, z);
    if (status & 1)
      status = Tdi3Multiply(y, z, z);
    if (status & 1)
      status = Tdi3Exp(z, z);
  }
	/******************************************
        Scalar integer exponent, look once at bits.
        Clobbers x and x&z must be different.
        ******************************************/
  else if (y->class != CLASS_A) {
    if ((yy = *(int *)y->pointer) <= 0) {
      if (yy == 0)
	return TdiConvert(&one_dsc, z);
      yy = -yy;
      duno = *x;
      duno.pointer = uno;
      status = TdiConvert(&one_dsc, &duno);
      if (status & 1)
	status = Tdi3Divide(&duno, x, x);
    }
    for (; !(yy & 1) && status & 1; yy >>= 1)
      status = Tdi3Multiply(x, x, x);
    if (x->class != CLASS_A)
      _MOVC3(z->length, x->pointer, z->pointer);
    else
      _MOVC3(z->arsize, x->pointer, z->pointer);
    for (; (yy >>= 1) > 0 && status & 1;) {
      status = Tdi3Multiply(x, x, x);
      if (yy & 1 && status & 1)
	status = Tdi3Multiply(x, z, z);
    }
  }
	/************************
        Must do it the hard way.
        Copy to keep scalar base.
        ************************/
  else {
    char xx[32];
    struct descriptor dx = { 0, 0, CLASS_S, 0 };
    char *px = x->pointer;
    int incx = x->class == CLASS_A ? x->length : 0;
    int *py = (int *)y->pointer;
    struct descriptor dz = *(struct descriptor *)z;
    int incz = dz.length;
    int n;

    dx.length = x->length;
    dx.dtype = x->dtype;
    dx.pointer = xx;
    N_ELEMENTS(z, n);
    dz.class = CLASS_S;
    duno = dx;
    duno.pointer = uno;
    if (status & 1)
      status = TdiConvert(&one_dsc, &duno);
    for (; --n >= 0 && status & 1; px += incx, dz.pointer += incz) {
      _MOVC3(dx.length, px, xx);
      if ((int)(yy = *py++) <= 0) {
	if (yy == 0) {
	  _MOVC3(dz.length, uno, dz.pointer);
	  continue;
	}
	yy = -yy;
	status = Tdi3Divide(&duno, &dx, &dx);
      }
      for (; !(yy & 1) && status & 1; yy >>= 1)
	status = Tdi3Multiply(&dx, &dx, &dx);
      _MOVC3(dz.length, dx.pointer, dz.pointer);
      for (; (yy >>= 1) > 0 && status & 1;) {
	status = Tdi3Multiply(&dx, &dx, &dx);
	if (yy & 1 && status & 1)
	  status = Tdi3Multiply(&dx, &dz, &dz);
      }
    }
  }
  return status;
}

/*-------------------------------------------------------------
        Tdi3Merge.C
        Assign value from either true or false source according to mask.
*/
typedef struct {
  int q0, q1;
} lquad;

int Tdi3Merge(struct descriptor_a *pdtrue, struct descriptor_a *pdfalse,
	      struct descriptor_a *pdmask, struct descriptor_a *pdout)
{
  int len = pdout->length;
  char *po = pdout->pointer;
  int n = 0, status = 1;

  int stept = pdtrue->class == CLASS_A ? len : 0;
  char *pt = pdtrue->pointer;
  int stepf = pdfalse->class == CLASS_A ? len : 0;
  char *pf = pdfalse->pointer;

  int stepm = pdmask->class == CLASS_A ? pdmask->length : 0;
#ifdef WORDS_BIGENDIAN
  char *pm = pdmask->pointer + pdmask->length - 1;
#else
  char *pm = pdmask->pointer;
#endif
  char *pc;
  short *ps;
  int *pl;
  lquad *pq;

  N_ELEMENTS(pdout, n);
  if (status & 1)
    switch (len) {
    case 1:
      for (pc = (char *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pc++ = *(char *)pt;
	else
	  *pc++ = *(char *)pf;
      break;
    case 2:
      for (ps = (short *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *ps++ = *(short *)pt;
	else
	  *ps++ = *(short *)pf;
      break;
    case 4:
      for (pl = (int *)po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pl++ = *(int *)pt;
	else
	  *pl++ = *(int *)pf;
      break;
    case 8:
      for (pq = (lquad *) po; --n >= 0; pm += stepm, pt += stept, pf += stepf)
	if (*pm & 1)
	  *pq++ = *(lquad *) pt;
	else
	  *pq++ = *(lquad *) pf;
      break;
    default:
      {
	int sm = stepm, st = stept, sf = stepf;
	for (; --n >= 0; pm += sm, pt += st, pf += sf, po += len)
	  if (*pm & 1)
	    _MOVC3(len, pt, po);
	  else
	    _MOVC3(len, pf, po);
      }
      break;
    }
  return status;
}
