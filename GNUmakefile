# A convenience Makefile that simply imports instructions
# for GNU/Linux systems.
# Created by Jordan Irwin

include make_definitions

ifdef WIN32
LIBS=-lmingw
EXE_SUF=.exe
EXE=$(PNAME)$(EXE_SUF)
else
  ifdef __WIN32__
  LIBS=-lmingw
  EXE_SUF=.exe
  EXE=$(PNAME)$(EXE_SUF)
  endif
endif

include make_directives


help:
	@echo
	@echo "Makefile for GNU/Linux systems."
	@echo
	@echo "By default GNU 'make' ('gmake' on BSD"
	@echo "systems) will invoke this file & import"
	@echo "the contentents of 'Makefile.GNU'. To"
	@echo "manually specify a platform-dependent"
	@echo "Makefile use the following:"
	@echo
	@echo "	make -f Makefile.<platform>"
	@echo
	@echo "	Available platforms:"
	@echo "		BSD GNU MinGW"
	@echo
	