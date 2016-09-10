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


help:
	@echo
	@echo "Makefile for BSD systems."
	@echo
	@echo "By default BSD 'make' will invoke this file"
	@echo "& import the contentents of 'Makefile.GNU'."
	@echo "To manually specify a platform-dependent"
	@echo "Makefile use the following:"
	@echo
	@echo "	make -f Makefile.<platform>"
	@echo
	@echo "	Available platforms:"
	@echo "		BSD GNU MinGW"
	@echo
	