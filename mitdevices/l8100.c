#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include "l8100_gen.h"
#include <libroutines.h>
#include <strroutines.h>
#include <treeshr.h>
#include <mdsshr.h>

struct l8100_ctl {
  unsigned gain:3;
  unsigned mult:3;
  unsigned ground:1;
  unsigned filter:1;
};

static int one = 1;
static int zero = 0;

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) < (b)) ? (b) : (a))
#define SignalError(call) status = call; if (!(status & 1)) { return status; }
#define pio(f,a,d) {\
 if (!((status = DevCamChk(CamPiow(setup->name,a,f,d,16,0),&one,0)) & 1)) return status;}
#define IsOn(nid) (((status = TreeIsOn(nid)) == TreeON) || (status == TreePARENT_OFF))

extern int DevCamChk();
extern int CamPiow();
extern int TdiCompile();

static int InitChannel(InInitStruct * setup, int chan, int gain, int mult, float offset)
{
  int status;
  int input_nid = setup->head_nid + L8100_N_INPUT_1 + chan * (L8100_N_INPUT_2 - L8100_N_INPUT_1);
  int filter_nid = input_nid + L8100_N_INPUT_1_FILTER - L8100_N_INPUT_1;
  int ground_nid = input_nid + L8100_N_INPUT_1_GROUND - L8100_N_INPUT_1;
  if (IsOn(input_nid)) {
    unsigned short off;
    struct l8100_ctl ctl;
    unsigned short *ctl_w;
    ctl.gain = gain;
    ctl.mult = mult;
    ctl.filter = IsOn(filter_nid);
    ctl.ground = IsOn(ground_nid);
    pio(16, chan, (short *)&ctl)
	off = 4096 * (5.5 - offset) / 11. + .4999;
    off = min(off, 4095);
    pio(17, chan, &off)
  }
  return status;
}

int l8100___init(struct descriptor *niddsc, InInitStruct * setup)
{
  int status;

  status = InitChannel(setup, 0,
		       setup->input_1_gain_convert,
		       setup->input_1_mult_convert, setup->input_1_offset);
  status = InitChannel(setup, 1,
		       setup->input_2_gain_convert,
		       setup->input_2_mult_convert, setup->input_2_offset);
  return status;
}

static int StoreChannel(InStoreStruct * setup, int chan)
{
  int status;
  int input_nid = setup->head_nid + L8100_N_INPUT_1 + chan * (L8100_N_INPUT_2 - L8100_N_INPUT_1);
  int filter_on_nid = input_nid + L8100_N_INPUT_1_FILTER_ON - L8100_N_INPUT_1;
  int output_nid = input_nid + L8100_N_INPUT_1_OUTPUT - L8100_N_INPUT_1;
  if (IsOn(input_nid)) {
    unsigned short gn, mu;
    struct l8100_ctl ctl;
    static unsigned short off;
    static float mult;
    static float gain;
    static float offset;
    static float gains[] = { 1., 10., 100. };
    static float mults[] = { .2, .5, 1. };

    static DESCRIPTOR(expression, "build_with_units(($ - $) /($ * $),\"volts\")");
    static DESCRIPTOR_FLOAT(mult_d, &mult);
    static DESCRIPTOR_FLOAT(gain_d, &gain);
    static DESCRIPTOR_FLOAT(offset_d, &offset);
    static EMPTYXD(value);
    static DESCRIPTOR_NID(out_nid_d, 0);
    static DESCRIPTOR(y, "Y");
    static DESCRIPTOR(n, "N");
    struct descriptor *filter;
    pio(0, chan, (short *)&ctl)
	filter = (struct descriptor *)((ctl.filter) ? &y : &n);
    gn = (ctl.gain & 1) ? 2 : ((ctl.gain & 2) ? 1 : 0);
    gain = gains[gn];
    mu = (ctl.mult & 1) ? 2 : ((ctl.mult & 2) ? 1 : 0);
    mult = mults[mu];
    pio(1, chan, &off)
	offset = 5.5 - off / 4096. * 11.;
    out_nid_d.pointer = (char *)&output_nid;
    SignalError(TdiCompile
		(&expression, &out_nid_d, &offset_d, &gain_d, &mult_d, &value MDS_END_ARG));
    SignalError(TreePutRecord(input_nid, (struct descriptor *)&value, 0));
    if (IsOn(filter_on_nid))
      SignalError(TreePutRecord(filter_on_nid, filter, 0));
  }
  return status;
}

int l8100___store(struct descriptor *niddsc, InStoreStruct * setup)
{
  int status;
  status = StoreChannel(setup, 0);
  status = StoreChannel(setup, 1);
  return status;
}
