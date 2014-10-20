# MyABCs Makefile for GNU systems.
# Created by Jordan Irwin.

# Environment
PROJECT=MyABCs
V_MAJ=0
V_MIN=5
V_REV=x
VERSION=${V_MAJ}.${V_MIN}.${V_REV}
PREFIX=/usr/local
SDL2_TEST=sdl2-config --version

# Directories
DIR_DATA=data
DIR_DOC=doc
DIR_INCL=include
DIR_SRC=src
SUBDIRS=${DIR_DATA} ${DIR_DOC} ${DIR_INCL} ${DIR_SRC}

# Compilation
CC=g++
ifdef DEBUG
CFLAGS=-c -Wall -g -I${DIR_INCL}
else
CFLAGS=-c -Wall -O2 -I${DIR_INCL}
endif
CXXFLAGS=${CFLAGS}
SDLFLAGS=`sdl2-config --cflags`

# Linking
ifdef __MINGW32__
LIBS=-lmingw -lSDLmain -lSDL
else
LIBS=-lSDLmain -lSDL
endif
SDLLIBS=`sdl-config --libs`
LDFLAGS=-s ${SDLLIBS}

# Files
EXE_NAME=myabcs
ifdef WIN32
EXE_SUF=.exe
else
EXE_SUF=
endif
EXE=${EXE_NAME}${EXE_SUF}
OBJ_SUF=.o
FILES_OBJ=main${OBJ_SUF}

all: tests ${FILES_OBJ}
	@echo "Making project ${PROJECT} version ${VERSION}."
	${CC} ${LDFLAGS} ${FILES_OBJ} -o ${EXE} ${LIBS}
main${OBJ_SUF}: ${DIR_SRC}/main.cpp ${DIR_INCL}/main.h
	${CC} ${CXXFLAGS} ${SDLFLAGS} ${DIR_SRC}/main.cpp
tests:
	@echo "Testing for SDL2..."
	@#sdl2-config --version || (echo "SDL2 not found... aborting"; exit 1)
	@${SDL2_TEST} && (echo "Found SDL version `${SDL2_TEST}`") || (echo "SDL2 not found... aborting"; exit 1)
install: data/* doc/* all
	@echo "No instructions yet."
clean:
	rm -rf *.o ${EXE}
help:
	@echo "Help instructions"
