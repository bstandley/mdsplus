

include ${top_srcdir}/Makefile.inc

if NOT_MINGW
else
EXE=.exe
WIN=.win
DEF=mdsipshr.def
endif

lib = @LIBPRE@


_SOURCES = adler32.c \
           compress.c \
           deflate.c \
           infblock.c \
           infcodes.c \
           inffast.c \
           inflate.c \
           inftrees.c \
           infutil.c \
           trees.c \
           uncompr.c \
           zutil.c
                      

library_includedir = $(includedir)/mdsplus/mdstcpip
library_include_HEADERS = 

noinst_LIBRARIES = libMdsIpzlib.a
libMdsIpzlib_a_SOURCES = ${_SOURCES}


