#include        "tclsysdef.h"
#include <string.h>
#include <dcl.h>

/**********************************************************************
* TCL_CLEAN_DATAFILE.C --
*
* TclCleanDatafile:  Clean a datafile (delete unused space, etc???)
*
* History:
*  14-Apr-1998  TRG  Create.  Ported from original mds code.
*
************************************************************************/

extern int TdiExecute();

	/****************************************************************
	 * TclCleanDatafile:
	 ****************************************************************/
int TclCleanDatafile(void *ctx, char **error, char **output)
{
  int sts;
  int shot;
  char *filnam=0;
  char *asciiShot=0;
  cli_get_value(ctx, "FILE", &filnam);
  cli_get_value(ctx, "SHOTID", &asciiShot);
  if (asciiShot && strlen(asciiShot) > 0) {
    char *endptr;
    shot = strtol(asciiShot, &endptr, 0);
    if (*endptr != 0) {
      *error=malloc(strlen(asciiShot)+100);
      sprintf(*error,"Error: Invalid shot specified '%s'. Please use an integer value.\n");
      sts = CLI_STS_IVVERB;
    }
    else {
      sts = TreeCleanDatafile(filnam, shot);
      if (!(sts & 1)) {
	char *msg=MdsGetMsg(sts);
	*error=malloc(strlen(msg)+strlen(filnam)+100);
	sprintf(*error,"Error: Problem cleaning tree '%s' shot '%d'\nError message was: %s\n",
		filnam,shot,msg);
      }
    }
  } else {
    *error=strdup("Error getting shot number from command.\n");
    sts = CLI_STS_IVVERB;
  }
  if (filnam)
    free(filnam);
  if (asciiShot)
    free(asciiShot);
  return sts;
}
