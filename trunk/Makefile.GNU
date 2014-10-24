# MyABCs Makefile for GNU/Linux systems.
# Created by Jordan Irwin.

include make_definitions

ifdef DEBUG
CFLAGS=-c -Wall -g -I${DIR_INCL}
CXXFLAGS=${CFLAGS}
LDFLAGS=${SDLLIBS}
endif

ifdef WIN32
LIBS=-lmingw -lSDLmain -lSDL
EXE_SUF=.exe
EXE=${PNAME}${EXE_SUF}
else
  ifdef __WIN32__
  LIBS=-lmingw -lSDLmain -lSDL
  EXE_SUF=.exe
  EXE=${PNAME}${EXE_SUF}
  endif
endif

include make_directives
