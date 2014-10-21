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
LIBS=-lSDL2main -lSDL2
endif
SDLLIBS=`sdl2-config --libs`
LDFLAGS=-s ${SDLLIBS}

# Files
EXE_NAME=myabcs
ifdef WIN32
EXE_SUF=.exe
else
EXE_SUF=
endif

include make_functions
