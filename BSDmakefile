# A convenience Makefile that simply imports instructions
# for BSD systems.
# Created by Jordan Irwin

.include "make_definitions"

.if defined(DEBUG)
CFLAGS=-c -O0 -g -Wall -Werror -DDEBUG -I${DIR_INCL}
CXXFLAGS=${CFLAGS} ${WXFLAGS}
LDFLAGS=${WXLIBS}
.endif

.if defined(WIN32) || defined(__WIN32__)
LIBS=-lmingw
EXE_SUF=.exe
EXE=${PNAME}${EXE_SUF}
.endif

.include "make_directives"
