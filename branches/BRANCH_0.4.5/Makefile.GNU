# MyABCs Makefile for GNU/Linux systems.
# Created by Jordan Irwin.

include make_definitions

ifdef DEBUG
CFLAGS=-c -O0 -g -Wall -Werror -DDEBUG -I${DIR_INCL}
CXXFLAGS=${CFLAGS} ${WXFLAGS}
LDFLAGS=${WXLIBS}
endif

ifdef WIN32
LIBS=-lmingw
EXE_SUF=.exe
EXE=${PNAME}${EXE_SUF}
else
  ifdef __WIN32__
  LIBS=-lmingw
  EXE_SUF=.exe
  EXE=${PNAME}${EXE_SUF}
  endif
endif

include make_directives
