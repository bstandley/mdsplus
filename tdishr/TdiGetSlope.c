/*      TdiGetSlope
        Evaluate a SLOPE type axis.
        Typically it is a time base for a recorder.
        WARNING: usually first BEGIN and last END are not defined.
        Assume monotonic axis, ascending or descending.
        Internal routine:
                status = TdiGetSlope(&window_dsc, &slope_dsc, &out_dsc)

        Ken Klare, LANL CTR-7   (c)1989,1990
        ASSUMES all slopes same sign and (end-start)*slope >= 0.
        NEED to watch for rounding/precision problems.
*/

#include <STATICdef.h>
#include <stdlib.h>
#include <tdishr_messages.h>
#include "tdirefstandard.h"

#include <mdsshr.h>

#define HUGE 0x1FFFFFFF
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))



extern int TdiData();
extern int TdiGetLong();
extern int TdiRange();
extern int TdiAdd();
extern int TdiSubtract();
extern int TdiMultiply();
extern int TdiDivide();
extern int TdiEvaluate();
extern int TdiFloat();
extern int TdiFloor();
extern int TdiNint();
extern int TdiLe();
extern int TdiLt();
extern int Tdi1Vector();
extern int TdiSetRange();

int TdiGetSlope(struct descriptor_window *window_ptr,
		struct descriptor_slope *slope_ptr, struct descriptor_xd *out_ptr)
{
  struct descriptor_xd xat0 = EMPTY_XD;
  int ndesc = slope_ptr->ndesc;
  int nseg = (ndesc + 2) / 3;
  int status = 0, jseg, kseg;
  int k0, k1, left, right;
  struct descriptor k0_dsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
  struct descriptor k1_dsc = { sizeof(int), DTYPE_L, CLASS_S, 0 };
  k0_dsc.pointer = (char *)&k0;
  k1_dsc.pointer = (char *)&k1;
	/*****************
        Get window limits.
        *****************/
  if (ndesc <= 0)
    status = TdiMISS_ARG;
  else if (window_ptr && window_ptr->startidx)
    status = TdiGetLong(window_ptr->startidx, &k0);
  else
    k0 = -HUGE;
  if (status & 1 && window_ptr && window_ptr->endingidx)
    status = TdiGetLong(window_ptr->endingidx, &k1);
  else
    k1 = HUGE;
  if (status & 1 && window_ptr)
    status = TdiData(window_ptr->value_at_idx0, &xat0);
  else if (status & 1)
    status = TdiData(window_ptr, &xat0);

	/********************************************
        Single slope with no begin or end.
        X[i] = (dX/di)*RANGE(startidx,endidx) + X[0].
        ********************************************/
  if ((ndesc < 2 || !slope_ptr->segment[0].begin) && (ndesc < 3 || !slope_ptr->segment[0].ending)) {
    if (status & 1)
      status = TdiRange(&k0_dsc, &k1_dsc, out_ptr);
    if (status & 1)
      status = TdiMultiply(slope_ptr->segment[0].slope, out_ptr, out_ptr);
    if (status & 1 && xat0.pointer)
      status = TdiAdd(&xat0, out_ptr, out_ptr);
  }

	/*********************************
        Multiple segments.
        Get each slope, begin, and end.
        May omit first begin and last end.
        *********************************/
  else if (status & 1) {
    struct descriptor left_dsc = { sizeof(left), DTYPE_L, CLASS_S, 0 };
    struct descriptor right_dsc = { sizeof(right), DTYPE_L, CLASS_S, 0 };
    DESCRIPTOR_RANGE(ramp, 0, 0, 0);
    struct descriptor_xd (*pbegin)[], (*pend)[] = 0, (*pslope)[] = 0, *(*pvlist)[] = 0, tmp1;
    int (*pcnt)[] = 0;
    int virt =
	(sizeof(struct descriptor_xd) * 3 +
	 sizeof(struct descriptor_xd *)) * nseg + sizeof(int) * (nseg + 1);

    left_dsc.pointer = (char *)&left;
    right_dsc.pointer = (char *)&right;
    ramp.begin = &left_dsc;
    ramp.ending = &right_dsc;
		/**************************************
                Dynamic allocation of descriptors.
                Memory for begin[nseg], end[nseg],
                slope[nseg], *vlist[nseg], cnt[nseg+1].
                **************************************/
    status = (pbegin = malloc(virt)) != NULL;
    if (status & 1) {
      pend = (struct descriptor_xd(*)[])&(*pbegin)[nseg];
      pslope = (struct descriptor_xd(*)[])&(*pend)[nseg];
      pvlist = (struct descriptor_xd * (*)[])&(*pslope)[nseg];
      pcnt = (int (*)[])&(*pvlist)[nseg];
    }

		/*************************************************************************************
                Find the number of points in interior segments, if any.
                For 1ms sampling, begin=0s, 1000 points, end=.999s, no sample is taken at 1s.
                Ideal expression: FLOOR((end - begin)/slope + 1) but not less than zero.
                WARNING: round-off may change by +-1, could change next by -+1 if end(j)==begin(j+1).
                WARNING: to prevent swings, ASSUME that divisor is close and round. Must be exact int.
                Expression: NINT((end - begin)/slope + 1)
                *************************************************************************************/
    tmp1 = EMPTY_XD;
    for (jseg = 0; jseg < nseg; ++jseg) {
      (*pslope)[jseg] = (*pbegin)[jseg] = (*pend)[jseg] = EMPTY_XD;
      if (status & 1)
	status = TdiEvaluate(slope_ptr->segment[jseg].slope, &(*pslope)[jseg]);
      if (status & 1 && jseg * 3 + 1 < ndesc)
	status = TdiEvaluate(slope_ptr->segment[jseg].begin, &(*pbegin)[jseg]);
      if (status & 1 && jseg * 3 + 2 < ndesc)
	status = TdiEvaluate(slope_ptr->segment[jseg].ending, &(*pend)[jseg]);

      if (jseg == 0 && !slope_ptr->segment[0].begin) {
	(*pcnt)[1] = HUGE;
	continue;
      }
      if (jseg == nseg - 1 && !slope_ptr->segment[jseg].ending) {
	(*pcnt)[nseg] = HUGE;
	continue;
      }

      if (status & 1)
	status = TdiSubtract((*pend)[jseg].pointer, (*pbegin)[jseg].pointer, &tmp1);
      if (status & 1)
	status = TdiDivide(&tmp1, (*pslope)[jseg].pointer, &tmp1);
      if (status & 1)
	status = TdiNint(&tmp1, &tmp1);
      if (status & 1)
	status = TdiGetLong(&tmp1, &left);
      (*pcnt)[jseg + 1] = MAX(left + 1, 0);
    }

		/********************************************
                Find the segment with value at index 0.
                Use first segment where expression is true:
                (0 LE slope[0]) EQV (x[0] LT begin[kseg+1]]).
                WARNING use .pointer to not free it.
                ********************************************/
    if (status & 1)
      status = TdiLe(0, (*pslope)[0].pointer, &tmp1);
    if (status & 1)
      status = TdiGetLong(&tmp1, &left);
    for (kseg = 0; status & 1 && kseg < nseg - 1; ++kseg) {
      status = TdiLt(xat0.pointer, (*pbegin)[kseg + 1].pointer, &tmp1);
      if (status & 1)
	status = TdiGetLong(&tmp1, &right);
      if (left == right)
	break;
    }

		/********************************************
                With index 0 in kseg, what is index at begin?
                left = (begin[kseg] - xat0)/slope[kseg]
                right = (end[0] - xat0)/slope[0]+1 for kseg=0.
                Trigger time xat0 implies next clock is at
                index 0, thus round down, more negative.
                Then adjust and accumulate counts.
                ********************************************/
    if (kseg == 0 && !slope_ptr->segment[0].begin) {
      if (status & 1)
	status = TdiSubtract((*pend)[0].pointer, &xat0, &tmp1);
      if (status & 1)
	status = TdiDivide(&tmp1, (*pslope)[0].pointer, &tmp1);
      if (status & 1)
	status = TdiGetLong(&tmp1, &right);
      (*pcnt)[0] = MAX(right + 1, 0) - (*pcnt)[1];
    } else {
			/*******************************************
                        To make FLOOR work, must be floating divide.
                        Generally, result is a negative number.
                        *******************************************/
      if (status & 1)
	status = TdiSubtract((*pbegin)[kseg].pointer, &xat0, &tmp1);
      if (status & 1)
	status = TdiFloat(&tmp1, &tmp1);
      if (status & 1)
	status = TdiDivide(&tmp1, (*pslope)[kseg].pointer, &tmp1);
      if (status & 1)
	status = TdiFloor(&tmp1, &tmp1);
      if (status & 1)
	status = TdiGetLong(&tmp1, &left);

			/***************************************
                        Definition of kseg gives cnt[kseg] <= 0.
                        Check that cnt[kseg+1] >= 0. For xat0
                        near start[kseg+1], get cnt[kseg+1] = 0.
                        ***************************************/
      (*pcnt)[0] = MAX(left, -(*pcnt)[kseg + 1]);
      for (jseg = kseg; --jseg >= 0;)
	(*pcnt)[0] -= (*pcnt)[jseg + 1];
    }
    for (jseg = 0; jseg < nseg; ++jseg)
      (*pcnt)[jseg + 1] += (*pcnt)[jseg];
    if (k0 < (*pcnt)[0])
      k0 = (*pcnt)[0];
    if (k1 >= (*pcnt)[nseg])
      k1 = (*pcnt)[nseg] - 1;

		/********************************
                For each segment generate result.
                ASSUMES ramp < 0 is 0 elements.
                [*:0] * slope[0] + end[0] or
                [0:*] * slope[jseg] + begin[jseg]
                Reuse slope as segment dsc.
                ********************************/
    for (jseg = nseg; --jseg >= 0;) {
      left = MAX(k0 - (*pcnt)[jseg], 0);
      right = MIN((*pcnt)[jseg + 1] - 1, k1) - (*pcnt)[jseg];
      if (left > right)
	(*pvlist)[jseg] = 0;
      else if (jseg == 0 && !slope_ptr->segment[0].begin) {
	left -= (*pcnt)[1] - (*pcnt)[0] - 1;
	right -= (*pcnt)[1] - (*pcnt)[0] - 1;
	if (status & 1)
	  status = TdiMultiply(&ramp, &(*pslope)[0], &tmp1);
	if (status & 1)
	  status = TdiAdd(&tmp1, &(*pend)[0], &(*pslope)[0]);
	(*pvlist)[0] = &(*pslope)[0];
      } else {
	if (status & 1)
	  status = TdiMultiply(&ramp, &(*pslope)[jseg], &tmp1);
	if (status & 1)
	  status = TdiAdd(&tmp1, &(*pbegin)[jseg], &(*pslope)[jseg]);
	(*pvlist)[jseg] = &(*pslope)[jseg];
      }
    }

		/************************
                Make segments into whole.
                ************************/
    if (status & 1)
      status = Tdi1Vector(0, nseg, (*pvlist), out_ptr);

    MdsFree1Dx(&tmp1, NULL);
    for (jseg = nseg; --jseg >= 0;) {
      MdsFree1Dx(&(*pslope)[jseg], NULL);
      MdsFree1Dx(&(*pbegin)[jseg], NULL);
      MdsFree1Dx(&(*pend)[jseg], NULL);
    }
    if (pbegin != NULL)
      free(pbegin);
  }
  MdsFree1Dx(&xat0, NULL);

	/*********************************************
        When first index is not 0, we must set bounds.
        *********************************************/
  if (window_ptr && status & 1 && k0 != 0) {
    DESCRIPTOR_RANGE(range, 0, 0, 0);
    range.begin = &k0_dsc;
    range.ending = &k1_dsc;
    status = TdiSetRange(&range, out_ptr, out_ptr);
  }

  return status;
}
