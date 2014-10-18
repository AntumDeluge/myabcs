# Makefile for MyABCS. Created by Jordan Irwin.

# Installation
PREFIX=/usr/local
.if !defined(DESTDIR)
  DESTDIR=${PREFIX}
.endif
DATADIR=${DESTDIR}/share/myabcs
APPSDIR=${DESTDIR}/share/applications
INSTALL=install -cS
INSTALLDATA=install -cS -m 0644

# Directories
SRC=src
INCLDIR=include

# Files
BIN=myabcs
EXESUFFIX=
ICON=myabcs.xpm
.if defined(WIN32) || defined(__WIN32__)
  EXESUFFIX=.exe
  ICON=myabcs.ico
.endif
EXE=${BIN}${EXESUFFIX}
DATA_FILES=${ICON} license-MIT.txt
DATA_PIX=pic
PIX_SUBDIRS=${DATA_PIX}/animals ${DATA_PIX}/food ${DATA_PIX}/misc ${DATA_PIX}/music ${DATA_PIX}/toys
DATA_SND=sound
SND_SUBDIRS=${DATA_SND}/alpha
DATA_DESKTOP=myabcs.desktop
OBJSUFFIX=.o
OBJ=abc${OBJSUFFIX} main${OBJSUFFIX} gnrcabt${OBJSUFFIX}

.if defined(unix) || defined(__unix__)
  INST_ICON=${INSTALLDATA} ${ICON} ${DESTDIR}/share/pixmaps
  RM_ICON=rm ${DESTDIR}/share/pixmaps/${ICON}
.endif

# Compilation
CC=g++
INCLUDE=-I${INCLDIR}
WXFLAGS=`wx-config --cxxflags`
CFLAGS=${WXFLAGS} ${INCLUDE}
CXXFLAGS=${CFLAGS}

# Linking
LIBS=-lpthread
WXLIBFLAGS=`wx-config --libs`
LDFLAGS=${WXLIBFLAGS}

# Because "myabcs" executable requires res.h add include for INCLUDE directory
all: libtests ${OBJ} ${INCLDIR}/res.h
	${CC} -s ${OBJ} $(LDFLAGS) -I${INCLDIR} -o myabcs $(LIBS)
${EXE}: all
	@# This just redirects to "all" when "make install" is called
abc${OBJSUFFIX}: ${SRC}/abc.cpp ${INCLDIR}/abc.h
	${CC} -c ${SRC}/abc.cpp ${CXXFLAGS} -o abc.o
main${OBJSUFFIX}: ${SRC}/main.cpp ${INCLDIR}/main.h
	${CC} -c ${SRC}/main.cpp ${CXXFLAGS} -o main.o
gnrcabt${OBJSUFFIX}: ${SRC}/gnrcabt.cpp ${INCLDIR}/gnrcabt.h
	${CC} -c ${SRC}/gnrcabt.cpp ${CXXFLAGS} -o gnrcabt.o
install: ${EXE} ${DATA_FILES} ${DATA_DESKTOP} ${PIX_SUBDIRS} $(SND_SUBDIRS)
	@echo "Creating directory structure..."
	@for subdir in ${PIX_SUBDIRS}; do mkdir -p ${DATADIR}/$${subdir}; done
	@for subdir in ${SND_SUBDIRS}; do mkdir -p ${DATADIR}/$${subdir}; done
	@mkdir -p ${DESTDIR}/bin ${APPSDIR} ${DESTDIR}/share/pixmaps
	@echo "Installing data..."
	@${INSTALLDATA} ${DATA_FILES} ${DATADIR}
	@echo "Installing pixmaps..."
	@${INSTALLDATA} ${DATA_PIX}/*.png ${DATADIR}/${DATA_PIX}
	@for subdir in ${PIX_SUBDIRS}; do ${INSTALLDATA} $${subdir}/*.png ${DATADIR}/$${subdir}; done
	@echo "Installing audio files..."
	@${INSTALLDATA} ${DATA_SND}/*.wav ${DATADIR}/${DATA_SND}
	@for subdir in ${SND_SUBDIRS}; do ${INSTALLDATA} $${subdir}/*.wav ${DATADIR}/$${subdir}; done
	@echo "Installing executable..."
	@${INSTALL} ${EXE} ${DATADIR}
	@echo "Installing menu file & icon..."
	@${INST_ICON}
	@${INSTALL} ${DATA_DESKTOP} ${APPSDIR}
	@echo "Creating symbolic link to executable..."
	@ln -s ${DATADIR}/${EXE} ${DESTDIR}/bin/${EXE}
uninstall:
	@echo "Uninstalling..."
	@rm ${DESTDIR}/bin/${EXE}
	@rm -r ${DATADIR}
	@rm ${APPSDIR}/myabcs.desktop
	@${RM_ICON}
clean:
	@echo "Cleaning MyABCs"
	@rm -rf *.o myabcs
libtests:
	@echo "Checking if wxWidgets is installed..."
	@echo "Checking if PThreads is installed..."
