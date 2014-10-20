
####################
# --- CONTENTS --- #
####################

* CONTENTS
* LICENSE
* ABOUT
* REQUIREMENTS
  - Building
    - Compilers
    - Libraries
  - Running
* BUILDING
  - GNU Compiler Collection (GCC)
    - Compiling Manually
  - Minimalist GNU for Windows (MinGW)
  - Microsoft Visual Studio
  - Compiling Manually
    - GNU Compiler Collection (GCC)
  - Apple Systems
  - BSD Systems
  - GNU/Linux Systems
  - Win32 Systems
    - Minimals GNU for Windows (MinGW)
    - Visual Studio


###################
# --- LICENSE --- #
###################

MyABCs & it's source code are copyright Jordan Irwin 2010-2014 & are
distributed under the terms & conditions of the MIT open source license. See
"license-MIT.txt" for more information.


#################
# --- ABOUT --- #
#################

MyABCs is an educational tool/game targeted at children. It is designed to
aid in learning the English alphabet through visual & audio samples. As a side
function it can also help to familiarize one with a keyboard QWERTY-style
keyboard.


########################
# --- REQUIREMENTS --- #
########################

-- Building --

  Compilers:
  The source code is written in C++ syntax so a C++ compiler is required for
  building. The project has been built & tested with the GNU Compiler
  Collection (GCC) & GNU/BSD make. Other compilers may be compatible but have
  not been tested so included instructions will mostly be limited to GCC.

  Libraries:  
  MyABCs is written with the aid of the wxWidgets toolkit. A version of
  wxWidgets must be installed or be present in the build environment for the
  software to be successfully compiled. Different systems will use a variant
  of wxWigets. On Win32 systems it is commonly called wxMSW & calls functions
  from the the Win32 GUI libraries. GNU/BSD systems using an X11 desktop/window
  systems generally use a version of wxWidgets called wxGTK which makes use of
  the Gtk+ GUI libraries.
  
  The project may possible switch from wxWidgets to SDL in the future.


####################
# --- BUILDING --- #
####################

-- GNU Compiler Collection (GCC) --
  
  Compiling Manually:
    To compile manually with GCC (g++) use the follow command as a guide:
    
      g++ -s -Wall -O2 -Iinclude src/* `wx-config --cxxflags --libs` -o myabcs
    
    If wx-config is not installed on your system's PATH environment you will
    need to explicitly link to the wxWidgets libraries & possible tell the
    compiler where to search for the headers & libraries using the "-I" & "-L"
    directives.
    
  BSD Make:
    The file "Makefile.BSD" is provide for systems using BSD make. The following
    command can be issued to compile & install on these systems:
    
      make -f Makefile.BSD install
    
  GNU Make:
    The file "Makefile.GNU" is provided for systems such as Linux which use GNU
    make. The following command can be issued to compile & install on these
    systems:
    
      make -f Makefile.GNU install
    
    BSD systems may also offer an executable for GNU make. To compile on BSD
    with GNU make replace "make" with "gmake".
  
-- Minimalist GNU for Windows (MinGW) --
  
  MinGW is basically GCC ported to the Win32 platform. So, the instructions
  under the section labeled "GNU Compiler Collection (GCC)" should work here
  as well. MinGW may offer different versions of the "make" command, but they
  generally should be compatible with Makefile.GNU. You can optionally specify
  Makefile.MinGW as the Makefile, but it simply imports the contents of
  Makefile.GNU.
  
  The Win32 CMD terminal will not parse the "`" character correctly. If you
  wish to compile with the `wx-config --cxxflags --libs` option you must do
  so from within a POSIX compliant shell (sh, bash, etc). Environments like
  Cygwin or MSYS come with one or more of these shells, but can be downloaded
  independently & accessed from the windows CMD prompt.
  
  To compile from the Windows CMD prompt use the following command:
  
    bash make -f Makefile.MinGW
  
-- Microsoft Visual Studio --
  
  Sorry, no instructions yet.
