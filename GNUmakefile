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
