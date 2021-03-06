/*      TdiIntrinsic.C
        Dispatch internal functions to their routines.
        These are extensions of the basic types.
        All operation must be defined in our table.
        Each standard FUNCTION is called:
                status = Tdi1name(opcode, narg, &list, &out)
        NOTE first and second arguments are passed by value.
        The returned expression is not re-evaluated.
        This is used by Tdi1Evaluate for expression evaluation
        and by TDISHR$SHARE.MAR for external calls.
        If output is class XD, free it when we can and make it the result.
        If output is class D, then try to grab S or D class. Type is set by data.
        If output is class S or A, then try to convert into it. Only VMS result types.
        Can use this to get scalar text of a long by:
                DESCRIPTOR(output_dsc, "12345678");
                status = TdiLong(&input_dsc, &output_dsc);
        If input_dsc describes -123.4, then output_dsc will describe "bbbb-123".
        WARNING, this depends on string writable.
        WARNING, STR$ routines signal rather that return the error.

        Ken Klare, LANL P-4     (c)1989,1990,1991,1992
*/
#define PREC_COMMA 92
#define MAXLINE 120
#define MAXFRAC 40
#define MINMAX(min, test, max) ((min) >= (test) ? (min) : (test) < (max) ? (test) : (max))

#if defined(_WIN32)
#include <windows.h>
#endif

#include <STATICdef.h>
#include "tdithreadsafe.h"
#include "tdirefcat.h"
#include "tdireffunction.h"
#include "tdirefstandard.h"
#include "tdirefzone.h"
#include <string.h>
#include <strroutines.h>
#include <libroutines.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tdishr_messages.h>
#include <mdsshr.h>
#include <mds_stdarg.h>


typedef struct _bounds {
  int l;
  int u;
} BOUNDS;

#define _MOVC3(a,b,c) memcpy(c,b,a)
extern int TdiDECOMPILE_MAX;
extern unsigned short OpcCompile;
extern int TdiFaultHandlerNoFixup();
extern int Tdi0Decompile();
extern int TdiConvert();
extern int TdiGetLong();
#ifdef __VMS
#define SysGetMsg sys##$##getmsg
#endif
extern int SysGetMsg();
STATIC_ROUTINE struct descriptor *FixedArray();
STATIC_CONSTANT char *VMS_CLASS[] = { "CLASS0", "S", "D", "CLASS3", "A" };
STATIC_CONSTANT char *MDS_CLASS[] = { "XD", "XS", "R", "CA", "APD" };

STATIC_CONSTANT char *MDS_DTYPE[] = {
  "Ident-",			/*191 */
  "Nid",			/*192 */
  "Path-",			/*193 */
  "Param",			/*194 */
  "Signal",			/*195 */
  "Dimension",			/*196 */
  "Window",			/*197 */
  "Slope",			/*198 */
  "Function-",			/*199 */
  "Conglom",			/*200 */
  "Range",			/*201 */
  "Action",			/*202 */
  "Dispatch",			/*203 */
  "Program",			/*204 */
  "Routine",			/*205 */
  "Procedure",			/*206 */
  "Method",			/*207 */
  "Dependency",			/*208 */
  "Condition",			/*209 */
  "Event-",			/*210 */
  "With_Units",			/*211 */
  "Call",			/*212 */
  "With_Error",			/*213 */
};

STATIC_CONSTANT DESCRIPTOR(compile_err, "%TDI Syntax error near # marked region\n");
STATIC_CONSTANT DESCRIPTOR(hilite, "##");
STATIC_CONSTANT DESCRIPTOR(newline, "\n");
STATIC_THREADSAFE pthread_mutex_t lock;
STATIC_THREADSAFE int lock_initialized = 0;
STATIC_CONSTANT struct descriptor miss_dsc = { 0, DTYPE_MISSING, CLASS_S, 0 };

/****************************
Explain in 300 words or less.
****************************/
#define MAXMESS 1800
STATIC_ROUTINE void add(char *text)
{
  struct descriptor_d new = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_d *message = &((TdiThreadStatic())->TdiIntrinsic_message);
  new.length = (unsigned short)strlen(text);
  new.pointer = text;
  if (message->length + new.length < MAXMESS)
    StrAppend(message, &new);
}

STATIC_ROUTINE void numb(int count)
{
  STATIC_CONSTANT char blanks[] = "            ";
  char val[sizeof(blanks)], *pval;
  strcpy(val, blanks);
  pval = &val[sizeof(val) - 1];
  if (count < 0)
    *--pval = '-';
  else if (count == 0)
    *--pval = '0';
  else
    for (; count > 0 && pval > val; count /= 10)
      *--pval = (char)(count % 10 + '0');
  if (pval > val)
    *--pval = ' ';
  add(pval);
}

/***************************************************
Danger: this routine is used by DECOMPILE to report.
***************************************************/
int TdiTrace(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  struct descriptor_d *message = &((TdiThreadStatic())->TdiIntrinsic_message);
  if (message->length > MAXMESS)
    return 0;
  add("%TDI Decompile text_length");
  numb(out_ptr->length);
  add(" partial text: ");
  if (out_ptr->length < MAXLINE - 70)
    StrAppend(message, out_ptr);
  else {
    *((char *)out_ptr->pointer + MAXLINE - 70) = '\0';
    add((char *)out_ptr->pointer);
  }
  return 1;
}

int TRACE(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int j;
  struct descriptor_d text = { 0, DTYPE_T, CLASS_D, 0 };
  struct descriptor_d *message = &((TdiThreadStatic())->TdiIntrinsic_message);
  unsigned short now = message->length;

  if (now > MAXMESS)
    return 0;
  if (opcode >= 0 && opcode <= TdiFUNCTION_MAX) {
    struct TdiFunctionStruct *pfun = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];

    if (narg < pfun->m1 || narg > pfun->m2) {
      add("%TDI Requires");
      numb(pfun->m1);
      if (pfun->m1 != pfun->m2) {
	add(" to");
	numb(pfun->m2);
      }
      add(" input arguments for ");
    } else
      add("%TDI Error in ");
    add(pfun->name);
  } else
    add("%TDI Unknown opcode ");
  add("(");

  TdiDECOMPILE_MAX = 5;
  for (j = 0; j < narg;) {
    if (Tdi0Decompile(list[j], PREC_COMMA, &text) & 1) {
      if (message->length - now + text.length < MAXLINE - 2)
	StrAppend(message, &text);
      else {
	*(text.pointer + MAXFRAC) = '\0';
	add(text.pointer);
	add(" ...");
      }
    } else
      add("BAD_INPUT");
    StrFree1Dx(&text);
    if (++j < narg) {
      if (message->length - now < MAXLINE - MAXFRAC - 7)
	add(", ");
      else {
	add(",\n");
	now = message->length;
	add("%- ");
      }
    }
  }
  add(")\n");
  return 1;
}

/**********************************
Useful for access violation errors.
**********************************/
STATIC_ROUTINE int interlude(int (*f1) (),
			     int opcode,
			     int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status;
#if defined(_WIN32) && !defined(HAVE_PTHREAD_H)
  __try {
#endif
    status = (*f1) (opcode, narg, list, out_ptr);
#if defined(_WIN32) && !defined(HAVE_PTHREAD_H)
  }
  __except(EXCEPTION_EXECUTE_HANDLER) {
    status = TdiBOMB;
  }
#endif
  return status;
}

STATIC_CONSTANT EMPTYXD(emptyxd);

int TdiIntrinsic(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  struct TdiFunctionStruct *fun_ptr = (struct TdiFunctionStruct *)&TdiRefFunction[opcode];
  struct descriptor_xd tmp;
  int stat1 = 1;
  struct descriptor *dsc_ptr;
  struct descriptor_d *message = &((TdiThreadStatic())->TdiIntrinsic_message);
  tmp = emptyxd;
  TdiThreadStatic()->TdiIntrinsic_recursion_count =
      TdiThreadStatic()->TdiIntrinsic_recursion_count + 1;
  if (narg < fun_ptr->m1)
    status = TdiMISS_ARG;
  else if (narg > fun_ptr->m2)
    status = TdiEXTRA_ARG;
  else if (TdiThreadStatic()->TdiIntrinsic_recursion_count > 1800)
    status = TdiRECURSIVE;
  else {
    int list_size = narg * sizeof(struct descriptor *);
    struct descriptor *fixed_list[256];
    char fixed[256];
    int i;
    for (i = 0; i < narg; i++)
      if (list[i] != NULL && list[i]->class == CLASS_NCA) {
	fixed[i] = 1;
	fixed_list[i] = FixedArray(list[i]);
      } else {
	fixed[i] = 0;
	fixed_list[i] = list[i];
      }
    status = interlude(fun_ptr->f1, opcode, narg, fixed_list, &tmp);
    for (i = 0; i < narg; i++)
      if (fixed[i])
	free(fixed_list[i]);
  }
  if (status & 1
      || status == TdiBREAK || status == TdiCONTINUE || status == TdiGOTO || status == TdiRETURN) {

    if (!out_ptr)
      goto notmp;
    switch (out_ptr->class) {
    default:
      status = TdiINVCLADSC;
      break;

		/*****************************************
                To XD, rename it if from XD, else make it.
                *****************************************/
    case CLASS_XD:
		/*************************************************************************
                WARNING could be tricky if an input is same as output and propagated thru.
                If out overlaps tmp, then some input is the same as the output
                and the input was freed, so don't free output.
                *************************************************************************/
      if ((char *)out_ptr->pointer + out_ptr->l_length <=
	  (char *)tmp.pointer || (char *)out_ptr->pointer >= (char *)tmp.pointer + tmp.l_length)
	MdsFree1Dx(out_ptr, NULL);
      else if (out_ptr->l_length) {
	add("%TDI DANGER, part of old output descriptor was input to below.\n");
	TRACE(opcode, narg, list, out_ptr);
      }
      if (tmp.class == CLASS_XD)
	*out_ptr = tmp;
      else {
	stat1 = MdsCopyDxXd((struct descriptor *)&tmp, out_ptr);
	MdsFree1Dx(&tmp, NULL);
      }
      break;

		/************************************
                To D, allocate and copy it (free XD).
                ************************************/
    case CLASS_D:
      if (tmp.class == CLASS_XD)
	dsc_ptr = (struct descriptor *)tmp.pointer;
      else
	dsc_ptr = (struct descriptor *)&tmp;
      if (dsc_ptr == 0)
	stat1 = StrFree1Dx(out_ptr);
      else
	switch (dsc_ptr->class) {
	case CLASS_S:
	case CLASS_D:
	  if (out_ptr->length != dsc_ptr->length) {
	    stat1 = StrGet1Dx(&dsc_ptr->length, out_ptr);
	  }
	  if (stat1 & 1) {
	    out_ptr->dtype = dsc_ptr->dtype;
	    _MOVC3(out_ptr->length, dsc_ptr->pointer, (char *)out_ptr->pointer);
	  }
	  break;
	default:
	  stat1 = TdiINVCLADSC;
	  break;
	}
      MdsFree1Dx(&tmp, NULL);
      break;

		/*******************************
                To S, convert (free if from XD).
                *******************************/
    case CLASS_S:
    case CLASS_A:
      if (tmp.class == CLASS_XD)
	dsc_ptr = tmp.pointer;
      else
	dsc_ptr = (struct descriptor *)&tmp;
      if (dsc_ptr)
	stat1 = TdiConvert(dsc_ptr, out_ptr);
      else
	stat1 = TdiConvert(&miss_dsc, out_ptr);
      MdsFree1Dx(&tmp, NULL);
      break;
    case CLASS_NCA:
      {
	struct descriptor *fixed_out_ptr = FixedArray(out_ptr);
	if (tmp.class == CLASS_XD)
	  dsc_ptr = tmp.pointer;
	else
	  dsc_ptr = (struct descriptor *)&tmp;
	if (dsc_ptr)
	  stat1 = TdiConvert(dsc_ptr, out_ptr);
	else
	  stat1 = TdiConvert(&miss_dsc, out_ptr);
	MdsFree1Dx(&tmp, NULL);
	free(fixed_out_ptr);
      }
      break;
    }
    if (stat1 & 1)
      goto done;
    status = stat1;
  }
  TRACE(opcode, narg, list, out_ptr);

	/********************************
        Compiler errors get special help.
        ********************************/
  if (opcode == OpcCompile && message->length < MAXMESS) {
    char *cur = MINMAX(TdiRefZone.a_begin, TdiRefZone.a_cur, TdiRefZone.a_end);
    int npre = MINMAX(0, TdiRefZone.l_ok, MAXLINE);
    int nbody = cur - TdiRefZone.a_begin - npre;
    int npost = TdiRefZone.a_end - cur;
    nbody = MINMAX(0, nbody, MAXLINE);
    npost = MINMAX(0, npost, MAXLINE);
    if (npre + nbody + npost > MAXLINE)
      npre = 0;
    if (nbody + npost > MAXLINE) {
      npre = MAXLINE - nbody - npost;
      if ((int)npre < 0)
	npre = 0;
      if (nbody + npost > MAXLINE)
	npost = MINMAX(0, npost, MAXFRAC);
      if (nbody + npost > MAXLINE)
	nbody = MAXLINE - npost;
    }
    {
      struct descriptor pre = { 0, DTYPE_T, CLASS_S, 0 };
      struct descriptor body = { 0, DTYPE_T, CLASS_S, 0 };
      struct descriptor post = { 0, DTYPE_T, CLASS_S, 0 };
      pre.length = (unsigned short)npre;
      pre.pointer = cur - nbody - npre;
      body.length = (unsigned short)nbody;
      body.pointer = cur - nbody;
      post.length = (unsigned short)npost;
      post.pointer = cur;
      StrConcat((struct descriptor *)message,
		(struct descriptor *)message, &compile_err, &pre, &hilite,
		&body, &hilite, &post, &newline MDS_END_ARG);
    }
  }

  if (out_ptr)
    MdsFree1Dx(out_ptr, NULL);
 notmp:MdsFree1Dx(&tmp, NULL);
 done:TdiThreadStatic()->TdiIntrinsic_recursion_count = 0;
  TdiThreadStatic()->TdiIntrinsic_mess_stat = status;
  return status;
}

/*--------------------------------------------------------------
        Set debugging printout.
                message = DEBUG([option])
        Where option is bitwise combination of:
                1 to prepend first error message
                2 to print the current message
                4 to clear the message buffer
*/
int Tdi1Debug(int opcode, int narg, struct descriptor *list[], struct descriptor_xd *out_ptr)
{
  int status = 1;
  int option = -1;
  int mess_stat = (TdiThreadStatic())->TdiIntrinsic_mess_stat;
  struct descriptor_d *message = &((TdiThreadStatic())->TdiIntrinsic_message);
  if (narg > 0 && list[0])
    status = TdiGetLong(list[0], &option);
  if (option & 1 && mess_stat != 1) {
    struct descriptor dmsg = { 0, DTYPE_T, CLASS_S, 0 };
    dmsg.pointer = MdsGetMsg(mess_stat);
    dmsg.length = strlen(dmsg.pointer);
    StrConcat((struct descriptor *)message, &dmsg, &newline, message MDS_END_ARG);
  }
  if (message->length) {
    if (option & 2)
      printf("%.*s", message->length, message->pointer);
    if (option & 4)
      (TdiThreadStatic())->TdiIntrinsic_mess_stat = StrFree1Dx(message);
  }
  status = MdsCopyDxXd((struct descriptor *)message, out_ptr);
  return status;
}

STATIC_ROUTINE struct descriptor *FixedArray(struct descriptor *in)
{

  array_coeff *a = (array_coeff *) in;
  int dsize = sizeof(struct descriptor_a) + sizeof(int) + 3 * sizeof(int) * a->dimct;
  int i;
  BOUNDS *bounds = (BOUNDS *) & a->m[a->dimct];
  array_coeff *answer = (array_coeff *) memcpy(malloc(dsize), a, dsize);
  answer->class = CLASS_A;
  answer->aflags.column = 1;
  answer->aflags.coeff = 1;
  answer->aflags.bounds = 1;
  for (i = 0; i < a->dimct; i++)
    answer->m[i] = bounds[i].u - bounds[i].l + 1;
  return (struct descriptor *)answer;
}
