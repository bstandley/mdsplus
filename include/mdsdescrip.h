#ifndef MDSDESCRIP_H_DEFINED
#define MDSDESCRIP_H_DEFINED 1

#include <config.h>

#ifdef _WIN32
#define __fill_name__ fill
#define __fill_value__ 0,
#define __char_align__ char
#else				/* _WINDOWS */
#define __char_align__
#define __fill_name__
#define __fill_value__
#endif				/* _WINDOWS */

#ifdef _DESCRIPTOR_PREFIXES
#define _DSCA_(field) dscA_##field
#define _DSCB_(field) dscB_##field
#define _DSCL_(field) dscL_##field
#define _DSCW_(field) dscW_##field
#define _DSCV_(field) dscV_##field
#else
#define _DSCA_(field) field
#define _DSCB_(field) field
#define _DSCL_(field) field
#define _DSCW_(field) field
#define _DSCV_(field) field
#endif

#define DTYPE_LIST 214
#define DTYPE_TUPLE 215
#define DTYPE_DICTIONARY 216
#define DTYPE_POINTER 51
/*
 *	Descriptor Prototype - each class of descriptor consists of at least the following fields:
 */
struct descriptor {
  unsigned short _DSCW_(length);	/* specific to descriptor class;  typically a 16-bit (unsigned) length */
  unsigned char _DSCB_(dtype);	/* data type code */
  unsigned char _DSCB_(class);	/* descriptor class code */
  char *_DSCA_(pointer);	/* address of first byte of data element */
};

/*
 *	Fixed-Length Descriptor:
 */
struct descriptor_s {
  unsigned short _DSCW_(length);	/* length of data item in bytes,
					   or if dtype is K_DTYPE_V, bits,
					   or if dtype is K_DTYPE_P, digits (4 bits each) */
  unsigned char _DSCB_(dtype);	/* data type code */
  unsigned char _DSCB_(class);	/* descriptor class code = K_CLASS_S */
  char *_DSCA_(pointer);	/* address of first byte of data storage */
};

/*
 *	Dynamic String Descriptor:
 */
struct descriptor_d {
  unsigned short _DSCW_(length);	/* length of data item in bytes,
					   or if dtype is K_DTYPE_V, bits,
					   or if dtype is K_DTYPE_P, digits (4 bits each) */
  unsigned char _DSCB_(dtype);	/* data type code */
  unsigned char _DSCB_(class);	/* descriptor class code = K_CLASS_D */
  char *_DSCA_(pointer);	/* address of first byte of data storage */
};

/*
 *	Array Descriptor:
 */

typedef struct _aflags {
  unsigned __char_align__:3;	/* reserved;  must be zero */
  unsigned __char_align__ _DSCV_(binscale):1;	/* if set, scale is a power-of-two, otherwise, -ten */
  unsigned __char_align__ _DSCV_(redim):1;	/* if set, indicates the array can be redimensioned */
  unsigned __char_align__ _DSCV_(column):1;	/* if set, indicates column-major order (FORTRAN) */
  unsigned __char_align__ _DSCV_(coeff):1;	/* if set, indicates the multipliers block is present */
  unsigned __char_align__ _DSCV_(bounds):1;	/* if set, indicates the bounds block is present */
} aflags;			/* array flag bits */

struct descriptor_a {
  unsigned short _DSCW_(length);	/* length of an array element in bytes,
					   or if dtype is K_DTYPE_V, bits,
					   or if dtype is K_DTYPE_P, digits (4 bits each) */
  unsigned char _DSCB_(dtype);	/* data type code */
  unsigned char _DSCB_(class);	/* descriptor class code = K_CLASS_A */
  char *_DSCA_(pointer);	/* address of first actual byte of data storage */
  char _DSCB_(scale);		/* signed power-of-two or -ten multiplier, as specified by
				   binscale, to convert from internal to external form */
  unsigned char _DSCB_(digits);	/* if nonzero, number of decimal digits in internal representation */
  aflags aflags;
  unsigned char _DSCB_(dimct);	/* number of dimensions */
  unsigned int _DSCL_(arsize);	/* total size of array in bytes,
				   or if dtype is K_DTYPE_P, digits (4 bits each) */
  /*
   * One or two optional blocks of information may follow contiguously at this point;
   * the first block contains information about the dimension multipliers (if present,
   * aflags.coeff is set), the second block contains information about
   * the dimension bounds (if present, aflags.bounds is set).  If the
   * bounds information is present, the multipliers information must also be present.
   *
   * The multipliers block has the following format:
   *      char    *_DSCA_(a0);            Address of the element whose subscripts are all zero
   *      int     _DSCL_(m) [DIMCT];      Addressing coefficients (multipliers)
   *
   * The bounds block has the following format:
   *      struct
   *      {
   *              unsigned int    _DSCL_(l);      Lower bound
   *              unsigned int    _DSCL_(u);      Upper bound
   *      } bounds [DIMCT];
   *
   * (DIMCT represents the value contained in dimct.)
   */
};

/************************************************
  Supplementary definitons for array classes.
  Useful for classes A, CA, and APD.
*************************************************/

#define ARRAY_HEAD(ptr_type)		\
		unsigned short	_DSCW_(length);	\
		unsigned char	_DSCB_(dtype);	\
		unsigned char	_DSCB_(class);	\
		ptr_type	*_DSCA_(pointer);	\
		char		_DSCB_(scale);	\
		unsigned char	_DSCB_(digits);	\
		aflags    aflags;   \
		unsigned char	_DSCB_(dimct);	\
		unsigned int	_DSCL_(arsize);

#define ARRAY(ptr_type)		struct {	ARRAY_HEAD(ptr_type)}

#define ARRAY_COEFF(ptr_type, dimct)	struct {	ARRAY_HEAD(ptr_type)	\
							ptr_type	*_DSCA_(a0);	\
							unsigned int		_DSCL_(m)[dimct];	\
					}

#define ARRAY_BOUNDS(ptr_type, dimct)	struct {	ARRAY_HEAD(ptr_type)	\
							ptr_type	*_DSCA_(a0);	\
							unsigned int		_DSCL_(m)[dimct];	\
							struct {			\
								int	_DSCL_(l);	\
								int	_DSCL_(u);	\
							} bounds[dimct];		\
						}

#define DESCRIPTOR_A(name, len, type, ptr, arsize) \
	ARRAY(char) name = {len, type, CLASS_A, (char *)ptr, 0, 0, {0,1,1,0,0}, 1, arsize}

#define DESCRIPTOR_A_COEFF(name, len, type, ptr, dimct, arsize) \
	ARRAY_COEFF(char, dimct) name = {len, type, CLASS_A, (char *)ptr, 0, 0, {0,1,1,1,0}, dimct, arsize}

#define DESCRIPTOR_A_COEFF_2(name, len, type, ptr, arsize, rows, columns) \
	ARRAY_COEFF(char, 2) name = {len, type, CLASS_A, (char *)ptr, 0, 0, {0,1,1,1,0}, 2, arsize, (char *)ptr,\
            {rows, columns}}

#define DESCRIPTOR_A_BOUNDS(name, len, type, ptr, dimct, arsize) \
	ARRAY_BOUNDS(char, dimct) name = {len, type, CLASS_A, (char *)ptr, 0, 0, {0,1,1,1,1}, dimct, arsize}

/************************************************
  CLASS_XD extended dynamic descriptor definition.
	Dynamic memory pointed to must be freed
	using the pointer and length.
	The descriptor is usually on the stack.
*************************************************/

#define CLASS_XD 192

struct descriptor_xd {
  unsigned short _DSCW_(length);
  unsigned char _DSCB_(dtype);
  unsigned char _DSCB_(class);
  struct descriptor *_DSCA_(pointer);
  unsigned int _DSCL_(l_length);
};

#define EMPTYXD(name) struct descriptor_xd name = {0, DTYPE_DSC, CLASS_XD, 0, 0}

/************************************************
  CLASS_XS extended static descriptor definition.
*************************************************/

#define CLASS_XS 193

struct descriptor_xs {
  unsigned short _DSCW_(length);
  unsigned char _DSCB_(dtype);
  unsigned char _DSCB_(class);
  struct descriptor *_DSCA_(pointer);
  unsigned int _DSCL_(l_length);
};

/************************************************
  CLASS_R Record descriptor definition.
*************************************************/

#define CLASS_R 194

#define RECORD_HEAD				\
		unsigned short	_DSCW_(length);	\
		unsigned char	_DSCB_(dtype);	\
		unsigned char	_DSCB_(class);	\
		unsigned char	*_DSCA_(pointer);	\
		unsigned char	_DSCB_(ndesc);	\
		unsigned __fill_name__ : 24;

struct descriptor_r {
  RECORD_HEAD struct descriptor *_DSCA_(dscptrs)[1];
};

#define RECORD(ndesc)	struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(dscptrs)[ndesc];	\
				}

#define DESCRIPTOR_R(name, type, ndesc) \
	RECORD(ndesc) name = {0, type, CLASS_R, 0, ndesc, __fill_value__ {0}}

/************************************************
  CLASS_CA Compressed array descriptor definition.
	This describes an array's shape and type.
	The evaluated class is CLASS_A.
	The length, dtype, multipliers, and
	bounds are imposed on the result.

	The data is reconstructed (decompressed) by
	the evaluation of the pointed descriptor.
	The reconstruction must be self-contained.
	It must have all length and type info.
*************************************************/

#define CLASS_CA 195

#define DESCRIPTOR_CA(name, len, type, ptr, arsize) \
	ARRAY(struct descriptor) name = {len, type, CLASS_CA, (struct descriptor *)ptr, 0, 0, \
            {0,1,1,0,0}, 1, arsize}

#define DESCRIPTOR_CA_COEFF(name, len, type, ptr, dimct, arsize) \
	ARRAY_COEFF(struct descriptor, dimct) name = {len, type, CLASS_CA, (struct descriptor *)ptr, 0, 0, \
            {0,1,1,1,0}, dimct, arsize}

#define DESCRIPTOR_CA_BOUNDS(name, len, type, ptr, dimct, arsize) \
	ARRAY_BOUNDS(struct descriptor, dimct) name = {len, type, CLASS_CA, (struct descriptor *)ptr, 0, 0, \
	    {0,1,1,1,1}, dimct, arsize}

/************************************************
  CLASS_APD Array of pointers to descriptors.
	This describes an array's shape and type.
	The evaluated class is CLASS_A.
	The multipliers and
	bounds are imposed on the result.
	The final dtype, length, and arsize are
	from evaluation.

	The pointer points to a list of addresses
	of descriptors.
	However, the length and arsize describe
	the list of pointers to descriptors.
*************************************************/

#define CLASS_APD 196

#define DESCRIPTOR_APD(name, type, ptr, ndesc) \
	ARRAY(struct descriptor *) name = {sizeof(struct descriptor *), type, CLASS_APD, \
		(struct descriptor **)ptr, 0, 0, {0,1,1,0,0}, 1, ndesc*sizeof(struct descriptor *)}

#define DESCRIPTOR_APD_COEFF(name, type, ptr, dimct, ndesc) \
	ARRAY_COEFF(struct descriptor *, dimct) name = {sizeof(struct descriptor *), type, CLASS_APD, \
		(struct descriptor **)ptr, 0, 0, {0,1,1,1,0}, dimct, ndesc*sizeof(struct descriptor *)}

#define DESCRIPTOR_APD_BOUNDS(name, type, ptr, dimct, ndesc) \
	ARRAY_BOUNDS(struct descriptor *, dimct) name = {sizeof(struct descriptor *), type, CLASS_APD, \
		(struct descriptor **)ptr, 0, 0, {0,1,1,1,1}, dimct, ndesc*sizeof(struct descriptor *)}

/*****************************************************
  MISSING marks omitted argument, converts to default.
  IDENT is text-like variable name for compiler.
  NID is 4-bytes describing a node identifier.
  PATH is text-like tree location.
  Others are supported record data types.
*****************************************************/

#define DTYPE_MISSING	0

#define DTYPE_IDENT	191

#define DTYPE_NID		192

#define DTYPE_PATH	193

#define DTYPE_PARAM	194

struct descriptor_param {
  RECORD_HEAD struct descriptor *_DSCA_(value);
  struct descriptor *_DSCA_(help);
  struct descriptor *_DSCA_(validation);
};

#define DESCRIPTOR_PARAM(name, value, help, validation) \
	struct descriptor_param name = {0, DTYPE_PARAM, CLASS_R, 0, 3, __fill_value__\
          (struct descriptor *)value, (struct descriptor *)help, (struct descriptor *)validation}

#define DTYPE_SIGNAL	195

struct descriptor_signal {
  RECORD_HEAD struct descriptor *_DSCA_(data);
  struct descriptor *_DSCA_(raw);
  struct descriptor *_DSCA_(dimensions)[1];
};

#define SIGNAL(ndims)	struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(data);		\
					struct descriptor *_DSCA_(raw);		\
					struct descriptor *_DSCA_(dimensions)[ndims];	\
				}

#define DESCRIPTOR_SIGNAL(name, ndims, data, raw) \
	SIGNAL(ndims) name = {0, DTYPE_SIGNAL, CLASS_R, 0, ndims + 2, __fill_value__\
        (struct descriptor *)data, (struct descriptor *)raw}

#define DESCRIPTOR_SIGNAL_1(name, data, raw, dimension) \
	SIGNAL(1) name = {0, DTYPE_SIGNAL, CLASS_R, 0, 3, __fill_value__\
	(struct descriptor *)data, (struct descriptor *)raw, {(struct descriptor *)dimension}}

#define DESCRIPTOR_SIGNAL_2(name, data, raw, dim_1, dim_2) \
	SIGNAL(2) name = {0, DTYPE_SIGNAL, CLASS_R, 0, 4, __fill_value__\
	(struct descriptor *)data, (struct descriptor *)raw, {(struct descriptor *)dim_1, (struct descriptor *)dim_2}}

#define DTYPE_DIMENSION	196

struct descriptor_dimension {
  RECORD_HEAD struct descriptor *_DSCA_(window);
  struct descriptor *_DSCA_(axis);
};

#define DESCRIPTOR_DIMENSION(name, window, axis) \
	struct descriptor_dimension name = {0, DTYPE_DIMENSION, CLASS_R, 0, 2, __fill_value__\
	(struct descriptor *)window, (struct descriptor *)axis}

#define DTYPE_WINDOW	197

struct descriptor_window {
  RECORD_HEAD struct descriptor *_DSCA_(startidx);
  struct descriptor *_DSCA_(endingidx);
  struct descriptor *_DSCA_(value_at_idx0);
};

#define DESCRIPTOR_WINDOW(name, start, iend, xref) \
	struct descriptor_window name = {0, DTYPE_WINDOW, CLASS_R, 0, 3, __fill_value__\
	(struct descriptor *)start, (struct descriptor *)iend, (struct descriptor *)xref}

struct descriptor_axis {
  RECORD_HEAD struct descriptor *_DSCA_(dscptrs)[3];
};

#define DTYPE_SLOPE	198	/* Do not use this deprecated type */
/* This is to be replaced by RANGEs of scalars or vectors */

struct descriptor_slope {
  RECORD_HEAD struct {
    struct descriptor *_DSCA_(slope);
    struct descriptor *_DSCA_(begin);
    struct descriptor *_DSCA_(ending);
  } segment[1];
};

#define SLOPE(nsegs)	struct	{	RECORD_HEAD \
					struct {\
						struct descriptor *_DSCA_(slope);	\
						struct descriptor *_DSCA_(begin);	\
						struct descriptor *_DSCA_(ending);	\
					} segment[nsegs];\
				}

#define DTYPE_FUNCTION	199

struct descriptor_function {
  RECORD_HEAD struct descriptor *_DSCA_(arguments)[1];
};

#define FUNCTION(nargs) struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(arguments)[nargs];	\
				}

#define DESCRIPTOR_FUNCTION(name, op_code_ptr, nargs) \
	FUNCTION(nargs) name = {sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)op_code_ptr, nargs}

#define DESCRIPTOR_FUNCTION_0(name, op_code_ptr) \
	struct descriptor_function name = {sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, \
						(unsigned char *)op_code_ptr, 0, __fill_value__ 0}

#define DESCRIPTOR_FUNCTION_1(name, op_code_ptr, arg) \
	struct descriptor_function name = {sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, \
						(unsigned char *)op_code_ptr, 1, __fill_value__	{(struct descriptor *)arg}}

#define DESCRIPTOR_FUNCTION_2(name, op_code_ptr, arg_1, arg_2) \
	FUNCTION(2) name = {sizeof(unsigned short), DTYPE_FUNCTION, CLASS_R, (unsigned char *)op_code_ptr, 2, __fill_value__\
	{(struct descriptor *)arg_1, (struct descriptor *)arg_2}}

#define DTYPE_CONGLOM	200

struct descriptor_conglom {
  RECORD_HEAD struct descriptor *_DSCA_(image);
  struct descriptor *_DSCA_(model);
  struct descriptor *_DSCA_(name);
  struct descriptor *_DSCA_(qualifiers);
};

#define DESCRIPTOR_CONGLOM(sname, image, model, name, qualifiers) \
    struct descriptor_conglom sname = {0, DTYPE_CONGLOM, CLASS_R, 0, 4, __fill_value__\
	(struct descriptor *)image, (struct descriptor *)model, (struct descriptor *)name,\
	(struct descriptor *)qualifiers}

#define DTYPE_RANGE	201

struct descriptor_range {
  RECORD_HEAD struct descriptor *_DSCA_(begin);
  struct descriptor *_DSCA_(ending);
  struct descriptor *_DSCA_(deltaval);
};

#define DESCRIPTOR_RANGE(name, begin, ending, delta) \
	struct descriptor_range name = {0, DTYPE_RANGE, CLASS_R, 0, 3, __fill_value__\
	(struct descriptor *)begin, (struct descriptor *)ending, (struct descriptor *)delta}

#define DTYPE_ACTION	202

struct descriptor_action {
  RECORD_HEAD struct descriptor *_DSCA_(dispatch);
  struct descriptor *_DSCA_(task);
  struct descriptor *_DSCA_(errorlogs);
  struct descriptor *_DSCA_(completion_message);
  struct descriptor_a *_DSCA_(performance);
};

#define DESCRIPTOR_ACTION(name, dispatch, task, errorlogs) \
	struct descriptor_action name = {0, DTYPE_ACTION, CLASS_R, 0, 5, __fill_value__\
	(struct descriptor *)dispatch, (struct descriptor *)task, (struct descriptor *)errorlogs, 0, 0}

#define DTYPE_DISPATCH	203

struct descriptor_dispatch {
  RECORD_HEAD struct descriptor *_DSCA_(ident);
  struct descriptor *_DSCA_(phase);
  struct descriptor *_DSCA_(when);
  struct descriptor *_DSCA_(completion);
};

/*****************************************************
  So far three types of scheduling are supported.
*****************************************************/
#define TreeSCHED_NONE	0
#define TreeSCHED_ASYNC	1
#define TreeSCHED_SEQ	2
#define TreeSCHED_COND	3

#define DESCRIPTOR_DISPATCH(name, type, ident, phase, when, completion) \
	struct descriptor_dispatch name = {sizeof(unsigned char), DTYPE_DISPATCH, CLASS_R, (unsigned char *)type, 4, __fill_value__\
	(struct descriptor *)ident, (struct descriptor *)phase, (struct descriptor *)when, \
	(struct descriptor *)completion}

#define DTYPE_PROGRAM	204

struct descriptor_program {
  RECORD_HEAD struct descriptor *_DSCA_(time_out);
  struct descriptor *_DSCA_(program);
};

#define DESCRIPTOR_PROGRAM(name, program, timeout) \
	struct descriptor_program name = {0, DTYPE_PROGRAM, CLASS_R, 0, 2, __fill_value__\
	(struct descriptor *)timeout, (struct descriptor *)program}

#define DTYPE_ROUTINE	205

struct descriptor_routine {
  RECORD_HEAD struct descriptor *_DSCA_(time_out);
  struct descriptor *_DSCA_(image);
  struct descriptor *_DSCA_(routine);
  struct descriptor *_DSCA_(arguments)[1];
};

#define ROUTINE(nargs)	struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(time_out);		\
					struct descriptor *_DSCA_(image);		\
					struct descriptor *_DSCA_(routine);		\
					struct descriptor *_DSCA_(arguments)[nargs];	\
				}

#define DESCRIPTOR_ROUTINE(name, image, routine, timeout, nargs) \
	ROUTINE(nargs) name = {0, DTYPE_ROUTINE, CLASS_R, 0, nargs + 3, __fill_value__\
	(struct descriptor *)timeout, (struct descriptor *)image, (struct descriptor *)routine}

#define DTYPE_PROCEDURE	206

struct descriptor_procedure {
  RECORD_HEAD struct descriptor *_DSCA_(time_out);
  struct descriptor *_DSCA_(language);
  struct descriptor *_DSCA_(procedure);
  struct descriptor *_DSCA_(arguments)[1];
};

#define PROCEDURE(nargs) struct {	RECORD_HEAD \
					struct descriptor *_DSCA_(time_out);		\
					struct descriptor *_DSCA_(language);		\
					struct descriptor *_DSCA_(procedure);		\
					struct descriptor *_DSCA_(arguments)[nargs];	\
				}

#define DESCRIPTOR_PROCEDURE(name, language, procedure, timeout, nargs) \
	PROCEDURE(nargs) name = {0, DTYPE_PROCEDURE, CLASS_R, 0, nargs + 3, __fill_value__\
	(struct descriptor *)timeout, (struct descriptor *)language, (struct descriptor *)procedure}

#define DTYPE_METHOD	207

struct descriptor_method {
  RECORD_HEAD struct descriptor *_DSCA_(time_out);
  struct descriptor *_DSCA_(method);
  struct descriptor *_DSCA_(object);
  struct descriptor *_DSCA_(arguments)[1];
};

#define METHOD(nargs)	struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(time_out);		\
					struct descriptor *_DSCA_(method);		\
					struct descriptor *_DSCA_(object);		\
					struct descriptor *_DSCA_(arguments)[nargs];	\
				}

#define METHOD_0	struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(time_out);		\
					struct descriptor *_DSCA_(method);		\
					struct descriptor *_DSCA_(object);		\
				}

#define DESCRIPTOR_METHOD(name, method, object, timeout, nargs) \
	METHOD(nargs) name = {0, DTYPE_METHOD, CLASS_R, 0, nargs + 3, __fill_value__\
	(struct descriptor *)timeout, (struct descriptor *)method, (struct descriptor *)object}

#define DESCRIPTOR_METHOD_0(name, method, object, timeout) \
	METHOD_0 name = {0, DTYPE_METHOD, CLASS_R, 0, 3, __fill_value__\
	(struct descriptor *)timeout, (struct descriptor *)method, (struct descriptor *)object}

#define DTYPE_DEPENDENCY	208

struct descriptor_dependency {
  RECORD_HEAD struct descriptor *_DSCA_(arguments)[2];
};

#define DESCRIPTOR_DEPENDENCY(name, op_code_ptr, arg_1, arg_2) \
	struct descriptor_dependency name = {sizeof(unsigned char), DTYPE_DEPENDENCY, CLASS_R, (unsigned char *)op_code_ptr, 2, __fill_value__\
	(struct descriptor *)arg_1, (struct descriptor *)arg_2}

#define DTYPE_CONDITION	209

struct descriptor_condition {
  RECORD_HEAD struct descriptor *_DSCA_(condition);
};

#define DESCRIPTOR_CONDITION(name, modifier, condition) \
	struct descriptor_condition name = {sizeof(unsigned char), DTYPE_CONDITION, CLASS_R, (unsigned char *)modifier, 1, __fill_value__\
	(struct descriptor *)condition}

#ifdef DTYPE_EVENT
#undef DTYPE_EVENT
#endif
#define DTYPE_EVENT	210

#define TreeNEGATE_CONDITION 	7
#define TreeIGNORE_UNDEFINED 	8
#define TreeIGNORE_STATUS	9
#define TreeDEPENDENCY_AND	10
#define TreeDEPENDENCY_OR	11

#define DTYPE_WITH_UNITS	211

struct descriptor_with_units {
  RECORD_HEAD struct descriptor *_DSCA_(data);
  struct descriptor *_DSCA_(units);
};

#define DESCRIPTOR_WITH_UNITS(name, value, units) \
	struct descriptor_with_units name = {0,DTYPE_WITH_UNITS,CLASS_R,0,2, __fill_value__\
	(struct descriptor *)value, (struct descriptor *)units}

/*********************************************
  A CALL invokes a routine in a shared image.
  TDI syntax is image->routine:type(arg0,...).
  The result (R0,R1) is given dtype, def=L.
*********************************************/

#define DTYPE_CALL	212

struct descriptor_call {
  RECORD_HEAD struct descriptor *_DSCA_(image);
  struct descriptor *_DSCA_(routine);
  struct descriptor *_DSCA_(arguments)[1];
};

#define CALL(nargs) struct	{	RECORD_HEAD \
					struct descriptor *_DSCA_(image);		\
					struct descriptor *_DSCA_(routine);		\
					struct descriptor *_DSCA_(arguments)[nargs];	\
				}

#define DESCRIPTOR_CALL(name, dtype_ptr, nargs, image, routine) \
	CALL(nargs) name = {sizeof(unsigned short), DTYPE_CALL, CLASS_R, (unsigned char *)dtype_ptr, nargs+2, __fill_value__\
	(struct descriptor *)image, (struct descriptor *)routine}

#define DTYPE_WITH_ERROR	213

struct descriptor_with_error {
  RECORD_HEAD struct descriptor *_DSCA_(data);
  struct descriptor *_DSCA_(error);
};

#define DESCRIPTOR_WITH_ERROR(name, data, error) \
	struct descriptor_with_error name = {0,DTYPE_WITH_ERROR,CLASS_R,0,2, __fill_value__\
	(struct descriptor *)data, (struct descriptor *)error}

#define DTYPE_OPAQUE 217

struct descriptor_opaque {
  RECORD_HEAD struct descriptor *_DSCA_(data);
  struct descriptor *_DSCA_(opaque_type);
};

#define DESCRIPTOR_OPAQUE(name, data, opaque_type) \
        struct descriptor_opaque name = {0,DTYPE_OPAQUE,CLASS_R,0,2, __fill_value__\
        (struct descriptor *)data, (struct descriptor *)opaque_type}

#define	 DESCRIPTOR_FLOAT(name, _float) struct descriptor name = {sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, (char *)_float}

#define	 DESCRIPTOR_LONG(name, _long) struct descriptor name = {sizeof(int), DTYPE_L, CLASS_S, (char *)_long}

#define	 DESCRIPTOR_NID(name, _nid) struct descriptor name = {sizeof(int), DTYPE_NID, CLASS_S, (char *)_nid}
/*
 *	Atomic data types:
 */
#define DTYPE_Z	0		/* unspecified */
#define DTYPE_BU	2	/* byte (unsigned);  8-bit unsigned quantity */
#define DTYPE_WU	3	/* word (unsigned);  16-bit unsigned quantity */
#define DTYPE_LU	4	/* longword (unsigned);  32-bit unsigned quantity */
#define DTYPE_QU	5	/* quadword (unsigned);  64-bit unsigned quantity */
#define DTYPE_OU	25	/* octaword (unsigned);  128-bit unsigned quantity */
#define DTYPE_B	6		/* byte integer (signed);  8-bit signed 2's-complement integer */
#define DTYPE_W	7		/* word integer (signed);  16-bit signed 2's-complement integer */
#define DTYPE_L	8		/* longword integer (signed);  32-bit signed 2's-complement integer */
#define DTYPE_Q	9		/* quadword integer (signed);  64-bit signed 2's-complement integer */
#define DTYPE_O	26		/* octaword integer (signed);  128-bit signed 2's-complement integer */
#define DTYPE_F	10		/* F_floating;  32-bit single-precision floating point */
#define DTYPE_D	11		/* D_floating;  64-bit double-precision floating point */
#define DTYPE_G	27		/* G_floating;  64-bit double-precision floating point */
#define DTYPE_H	28		/* H_floating;  128-bit quadruple-precision floating point */
#define DTYPE_FC	12	/* F_floating complex */
#define DTYPE_DC	13	/* D_floating complex */
#define DTYPE_GC	29	/* G_floating complex */
#define DTYPE_HC	30	/* H_floating complex */
#define DTYPE_CIT	31	/* COBOL Intermediate Temporary */
/*
 *	String data types:
 */
#define DTYPE_T	14		/* character string;  a single 8-bit character or a sequence of characters */
#define DTYPE_VT	37	/* varying character string;  16-bit count, followed by a string */
#define DTYPE_NU	15	/* numeric string, unsigned */
#define DTYPE_NL	16	/* numeric string, left separate sign */
#define DTYPE_NLO	17	/* numeric string, left overpunched sign */
#define DTYPE_NR	18	/* numeric string, right separate sign */
#define DTYPE_NRO	19	/* numeric string, right overpunched sign */
#define DTYPE_NZ	20	/* numeric string, zoned sign */
#define DTYPE_P	21		/* packed decimal string */
#define DTYPE_V	1		/* aligned bit string */
#define DTYPE_VU	34	/* unaligned bit string */
/*
 *	IEEE data types:
 */
#define DTYPE_FS  52		/* IEEE float basic single S */
#define DTYPE_FT  53		/* IEEE float basic double T */
#define DTYPE_FSC 54		/* IEEE float basic single S complex */
#define DTYPE_FTC 55		/* IEEE float basic double T complex */
/*
 *	Miscellaneous data types:
 */
#define DTYPE_ZI	22	/* sequence of instructions */
#define DTYPE_ZEM	23	/* procedure entry mask */
#define DTYPE_DSC	24	/* descriptor */
#define DTYPE_BPV	32	/* bound procedure value */
#define DTYPE_BLV	33	/* bound label value */
#define DTYPE_ADT	35	/* absolute date and time */
/*
 *	Reserved data type codes:
 *	codes 38-191 are reserved to DIGITAL;
 *	codes 160-191 are reserved to DIGITAL facilities for facility-specific purposes;
 *	codes 192-255 are reserved for DIGITAL's Computer Special Systems Group
 *	  and for customers for their own use.
 */

/*
 *	Codes for dsc$b_class:
 */
#define CLASS_S	1		/* fixed-length descriptor */
#define CLASS_D	2		/* dynamic string descriptor */
/*	CLASS_V			** variable buffer descriptor;  reserved for use by DIGITAL */
#define CLASS_A	4		/* array descriptor */
#define CLASS_P	5		/* procedure descriptor */
/*	CLASS_PI			** procedure incarnation descriptor;  obsolete */
/*	CLASS_J			** label descriptor;  reserved for use by the VMS Debugger */
/*	CLASS_JI			** label incarnation descriptor;  obsolete */
#define CLASS_SD	9	/* decimal string descriptor */
#define CLASS_NCA	10	/* noncontiguous array descriptor */
#define CLASS_VS	11	/* varying string descriptor */
#define CLASS_VSA	12	/* varying string array descriptor */
#define CLASS_UBS	13	/* unaligned bit string descriptor */
#define CLASS_UBA	14	/* unaligned bit array descriptor */
#define CLASS_SB	15	/* string with bounds descriptor */
#define CLASS_UBSB 16		/* unaligned bit string with bounds descriptor */
/*
 *	Reserved descriptor class codes:
 *	codes 15-191 are reserved to DIGITAL;
 *	codes 160-191 are reserved to DIGITAL facilities for facility-specific purposes;
 *	codes 192-255 are reserved for DIGITAL's Computer Special Systems Group
 *	  and for customers for their own use.
 */

/*
 *	A simple macro to construct a 32-bit string descriptor:
 */
#define DESCRIPTOR(name,string)	struct descriptor_s name = { sizeof(string)-1, DTYPE_T, CLASS_S, string }
#define DESCRIPTOR_FROM_CSTRING(d,cstring) \
  struct descriptor d = {0,DTYPE_T,CLASS_S,0}; \
  d.length=strlen(cstring); \
  d.pointer=cstring;

/*
 * array typedefs
 */

#define MAXDIM 8
typedef ARRAY_COEFF(char, MAXDIM) array_coeff;
typedef ARRAY_BOUNDS(char, MAXDIM) array_bounds;
typedef ARRAY_BOUNDS(struct descriptor *, MAXDIM) array_bounds_desc;
typedef ARRAY(char) array;
typedef ARRAY(int) array_int;
typedef ARRAY(struct descriptor *) array_desc;
typedef SIGNAL(MAXDIM) signal_maxdim;

#ifdef __VMS
#pragma member_alignment restore
#endif				/* __VMS */

#ifdef __VMS
#define DTYPE_NATIVE_FLOAT DTYPE_F
#define DTYPE_FLOAT_COMPLEX DTYPE_FC

#if __G_FLOAT
#define DTYPE_NATIVE_DOUBLE DTYPE_G
#define DTYPE_DOUBLE_COMPLEX DTYPE_GC
#else				/* __G_FLOAT */
#define DTYPE_NATIVE_DOUBLE DTYPE_D
#define DTYPE_DOUBLE_COMPLEX DTYPE_DC
#endif				/* __G_FLOAT */

#else				/* __VMS */
#define DTYPE_NATIVE_FLOAT DTYPE_FS
#define DTYPE_NATIVE_DOUBLE DTYPE_FT
#define DTYPE_FLOAT_COMPLEX DTYPE_FSC
#define DTYPE_DOUBLE_COMPLEX DTYPE_FTC
#endif				/* __VMS */

#ifndef DTYPE_FLOAT
#define DTYPE_FLOAT DTYPE_NATIVE_FLOAT
#endif

#ifndef DTYPE_DOUBLE
#define DTYPE_DOUBLE DTYPE_NATIVE_DOUBLE
#endif

#endif
