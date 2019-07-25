
MyABCs 0.4.6


 --------------
| Requirements |
 --------------
GNU Make        BSD Make was previously supported but may no
                longer work.
wxWidgets 3.x   https://wxwidgets.org/
wxSVG           http://wxsvg.sourceforge.net/
SDL2            https://libsdl.org/
PThreads        Linux: Check your package manager for a library
                implementation.
                Windows: If your compiler does not have an
                implementation you can download Pthreads-w32
                ( http://sourceware.org/pthreads-win32/ ).

NOTE: I plan to switch to CMake ( https://cmake.org/ ) build generator eventually.


 ------------------------------------------------------------
| Compiling with GNU Make (GNU G++, Clang, MSYS/MinGW, etc.) |
 ------------------------------------------------------------

1) Navigate to the source folder from the command line/terminal.
2) To compile execute:
    ‣ make
3) To install execute:
    ‣  make install (on Windows this puts all files into "build/stage/myabcs" directory)
4) To zip files into release archive execute:
    ‣  make pack

Make targets:
    help            Show this help information.
    all (default)   Compile & link executable.
    install         Install or stage files for release.
    stage           Same as install.
    uninstall       Remove files from system or stage directory.
    pack            Zip contentns of stage directory into archive
                    for release.

Environment variables:
    STATIC          Define to create static build (note: SDL
                    libraries are still linked dynamically).
    WXVERSION       Use a version of wxWidgets other than the default.
    EXTRA_LIBS      Use this variable to manually link to libraries
                    (example: EXTRA_LIBS="-lpng -lz"). This should only
                    be used if linker errors are encountered.
    REL_SUFFIX      Appends text to filename of release archive when
                    "make pack" is called.


 -------------------------------------
| Compiling with Microsoft Visual C++ |
 -------------------------------------

I don't know


 ----------------------------
| Compiling with Apple Xcode |
 ----------------------------

I am not familiar with Apple's Xcode environment, but I understand that it
comes with GNU GCC & possibly Clang. The instructions above should work here as
well.
