
###############################################################################
# - S1 - CONTENTS ----------------------------------------------------------- #
###############################################################################

‣ CONTENTS ................................................ S1
‣ LICENSE ................................................. S2
‣ ABOUT ................................................... S3
‣ REQUIREMENTS ............................................ S4
  • Building ............................................ S4.1
    ◦ Compilers ....................................... S4.1.1
    ◦ Libraries ....................................... S4.1.2
  • Running ............................................. S4.2
    ◦ Shared Libraries ................................ S4.2.1
‣ BUILDING ................................................ S5
  • GNU Compiler Collection (GCC) ....................... S5.1
    ◦ Compiling Manually .............................. S5.1.1
  • Makefile Systems .................................... S5.2
    ◦ BSD make ........................................ S5.2.1
    ◦ GNU/Linux make .................................. S5.2.2
  • Minimalist GNU for Windows (MinGW) .................. S5.3
    ◦ Compiling Manually .............................. S5.3.1
    ◦ Makefiles ....................................... S5.3.2
  • Microsoft Visual Studio (MSVC) ...................... S5.4
  • Apple Xcode ......................................... S5.5
  
  
###############################################################################
# - S2 - LICENSE ------------------------------------------------------------ #
###############################################################################
  
  MyABCs & it's source code are copyright Jordan Irwin 2010-2014 & are
  distributed under the terms & conditions of the MIT open source license. See
  "license-MIT.txt" for more information.
  
  
###############################################################################
# - S3 - ABOUT -------------------------------------------------------------- #
###############################################################################
  
  MyABCs is an educational tool/game targeted at children. It is designed to
  aid in learning the English alphabet through visual & audio samples. As a
  side function it can also help to familiarize one with a QWERTY-style
  keyboard.
  
  
###############################################################################
# - S4 - REQUIREMENTS ------------------------------------------------------- #
###############################################################################
  
_______________________________________________________________________________
# S4.1 - Building --
  
  
- S4.1.1 - Compilers:
  
  The source code is written in C++ syntax so a C++ compiler is required for
  building. The project has been built & tested with the GNU Compiler
  Collection (GCC) & GNU/BSD make. Other compilers may be compatible but have
  not been tested so included instructions will mostly be limited to GCC.
  
  
- S4.1.2 - Libraries:
  
  MyABCs is written with the aid of the wxWidgets toolkit. A version of
  wxWidgets must be installed or be present in the build environment for the
  software to be successfully compiled. Different systems will use a variant
  of wxWigets. On Win32 systems it is commonly called wxMSW & calls functions
  from the the Win32 GUI libraries. GNU/BSD systems using an X11 desktop/window
  systems generally use a version of wxWidgets called wxGTK which makes use of
  the Gtk+ GUI libraries.
  
  Currently MyABCs uses the version 2.8.x wxWidgets line but will be switching
  to version 3.0.x.
  
  The project may possibly switch from wxWidgets to SDL in the future.
  
  
_______________________________________________________________________________
# S4.2 - Running --
  
- S4.2.1 - Shared Libraries:
  
  Unless MyABCs is built against a static wxWidgets library it will need to
  link to the shared libraries which should be installed on the system's PATH.
  The PATH environment variable defines the locations for where to search for
  files such as executables & shared libraries.
  
  POSIX systems (UNIX/Linux/BSD) use a PATH where executables are generally
  stored in a location such as "/usr/share/bin" or "/usr/local/share/bin",
  & shared libraries (.so files) in a dirctory such as "/usr/lib" or
  "/usr/local/lib".
  
  Win32 systems (Windows XP, Vista, 7, 8, etc.) use much different paths.
  Generally executables & shared libraries (.dll files) are stored in either
  "C:\Windows", "C:\Windows\System32", or "C:\Windows\SysWOW64". Optionally,
  shared libraries can be stored in the same directory as the executable or
  it's working directory.
  
  
###############################################################################
# - S5 - BUILDING ----------------------------------------------------------- #
###############################################################################
  
_______________________________________________________________________________
# S5.1 - The GNU Compiler Collection (GCC) --
  
- S5.1.1 - Compiling Manually:
  
  To compile manually with GCC (g++) use the following command as a guide:
  
      g++ -s -Wall -O2 -Iinclude src/*.cpp `wx-config --cxxflags --libs` \
          -o myabcs
  
  KEY:
    ‣ g++:
          is the C++ compiler provided by GCC.
    ‣ -s (strip):
          Remove extra debug symbols from executable (optional)
    ‣ -W:
          Warning level (treat warnings as errors)
          OPTIONS: all
    ‣ -O:
          Optimization level
          OPTIONS: 
    ‣ -I:
          Search specified directory for required header files
    ‣ src/*.cpp:
          Input files (source) to be compiled (can be specified individually)
    ‣ wx-config:
          Configurations for compiling with & linking to wxWidgets
          OPTIONS: --prefix=<DIR>, --exec-preifx=<DIR>, --host=<HOST>,
                   --cppflags, --cflags, --cxxflags, --cc, --cxx, --ld,
                   --rescomp, --linkdeps
          OPTIONS: (for selecting different installed wxWidgets builds)
                   --toolkit=<TOOLKIT>, --universal[=yes|no],
                   --unicode[=yes|no], --debug[=yes|no], --static[=yes|no],
                   --version=<VERSION>
          NOTE:    The command "wx-config" & it's options must be enclosed in
                   "`" accent marks. These are not the same as "'" apostrophes.
                   Non-POSIX compliant shells, such as Windows CMD, will not
                   parse these correctly & execution will fail. POSIX shells,
                   such as "sh" & "bash" can be executed from within the
                   Windows command prompt. See section entitled "Minimalist GNU
                   for Win32 (MinGW)".
    ‣ -o <filename>:
          Determines the filename of the output file
  
  If wx-config is not installed on your system's PATH environment you will
  need to explicitly link to the wxWidgets libraries & possible tell the
  compiler where to search for the headers & libraries using the "-I" & "-L"
  directives.
  
  
_______________________________________________________________________________
- S5.2 - Makefile Systems --
  
- S5.2.1 - BSD make:
  
  The files "BSDmakefile" & "Makefile.BSD" are provide for systems using BSD's
  version of "make". Executing "make" or "make install" on BSD systems should
  load the file "BSDmakefile" which will import instructions from
  "Makefile.BSD". Optionally you can explicitly specify the use of
  "Makefile.BSD":
    
      make -f Makefile.BSD
      or
      make -f Makefile.BSD install
  
  
- S5.2.2 - GNU/Linux make:
  
  The GNU version of "make" works similarly to the BSD version but has a
  slightly different syntax. For this reason the files "GNUmakefile" &
  "Makefile.GNU" are provided. Executing "make" or "make install on GNU/Linux
  systems should load the file "GNUmakefile" which will import instructions
  from "Makefile.GNU". Again, "Makefile.GNU" can be manually specified using
  the following command:
  
      make -f Makefile.GNU
      or
      make -f Makefile.GNU install
  
  GNU "make" is the default version for GNU/Linux systems. It is may also
  be available on BSD systems as "gmake".
  
  
_______________________________________________________________________________
- S5.3 - Minimalist GNU for Win32 (MinGW) --
  
  MinGW is basically GCC ported to the Win32 platform. So, the instructions
  under the section labeled "GNU Compiler Collection (GCC)" should work here
  as well. MinGW may offer different versions of the "make" command, but they
  generally should be compatible with Makefile.GNU. You can optionally specify
  Makefile.MinGW as the Makefile, but it simply imports the contents of
  Makefile.GNU.
  
  
  S5.3.1 - Compiling Manually:
  
  The Win32 CMD terminal will not parse the "`" character correctly. If you
  wish to compile with the `wx-config --cxxflags --libs` option you must do
  so from within a POSIX compliant shell (sh, bash, etc). Environments like
  Cygwin or MSYS come with one or more of these shells, but can be downloaded
  independently & accessed from the windows CMD prompt.
  
  To compile from the Windows CMD prompt use the following command for
  reference:
  
      bash make -f Makefile.MinGW
  
  
  S5.3.2 - Makefiles:
  
  If you are using a UNIX-like build environment for Windows, such as Cygwin
  or Minimal System (MSYS), in conjunction with MinGW, you should be able to
  follow the instructions outlined in section 5.2.2 entitled "GNU/Linux make".
  These environments generally use a GNU-compatible version of "make".
  

_______________________________________________________________________________
- S5.4 - Microsoft Visual Studio (MSVC) -

  
  Sorry, no instructions yet.
  
  
_______________________________________________________________________________
- S5.5 - Apple Xcode --
  
  I do not have experience with Apple's Xcode development environment, but,
  from what I have read on the internet it makes available a version of GCC.
  If that is so, you should (theoretically) be able to use the instructions
  from section 5.1, entitled "The GNU Compiler Collection (GCC)", to compile
  manually from a terminal.
