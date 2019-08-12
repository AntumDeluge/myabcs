
# MyABCs Educational Software for Children

![Icon](https://antumdeluge.github.io/myabcs/data/icon.svg)

## Description

MyABCs is a simple alphabet game. It has two modes with multiple categories.

## How to Play

Press one of the icons at the top of the window to select a category.

Categories:
- Alphabet mode:
    - Select the ABC blocks icon at the top of the window.
    - In this mode, the player must press the letter key corresponding to the picture displayed on the screen.
    - Work your way through the English alphabet to finish the game.
- All other categories:
    - Simply press any letter on the keyboard to display an associated image.

While an image is displayed on the screen, press the spacebar to hear the pronunciation of the object & an associated sound effect if available.

## Building 

### Requirements
---

<pre>
GNU Make        BSD Make was previously supported but may no
                longer work.
wxWidgets 3.x   <a href="https://wxwidgets.org/">https://wxwidgets.org/</a>
wxSVG           <a href="http://wxsvg.sourceforge.net/">http://wxsvg.sourceforge.net/</a>
SDL2            <a href="https://libsdl.org/">https://libsdl.org/</a>
PThreads        Linux: Check your package manager for a library
                implementation.
                Windows: If your compiler does not have an
                implementation you can download <a href="http://sourceware.org/pthreads-win32/">Pthreads-w32</a>.
</pre>

**NOTE:** I plan to switch to [CMake](https://cmake.org/) build generator eventually.


### Compiling with GNU Make (GNU G++, Clang, MSYS/MinGW, etc.)
---

1. Navigate to the source folder from the command line/terminal.
2. To compile execute:
    - `make`
3. To install execute:
    - `make install` (on Windows this puts all files into "build/stage/myabcs" directory)
4. To zip files into release archive execute:
    - `make pack`

If you want to compile & use the "built-in" static libraries, execute the following
from the source folder:

```sh
$ make build-libs
$ make USE_BUILTIN=1
```

Make command usage:
<pre>
Targets:
    help            Show this help information.
    all (default)   Compile & link executable.
    install         Install or stage files for release.
    stage           Same as install.
    uninstall       Remove files from system or stage directory.
    pack            Zip contentns of stage directory into archive
                    for release.
    build-libs      Compile "built-in" libraries.

Environment variables:
    STATIC          Define to create static build (note: SDL
                    libraries are still linked dynamically).
    WXVERSION       Use a version of wxWidgets other than the default.
    EXTRA_LIBS      Use this variable to manually link to libraries
                    (example: EXTRA_LIBS="-lpng -lz"). This should only
                    be used if linker errors are encountered.
    REL_SUFFIX      Appends text to filename of release archive when
                    "make pack" is called.
    BUILD_LIBS      Only build specified built-in libs with build-libs target.
    USE_BUILTIN     Set to non-zero to link to built-in libs. Only works if
                    libraries previously built with build-libs target called.
    WXCONFIG        Use a custom wx-config
    SDLCONFIG       Use a custom sdl2-config
</pre>


### Compiling with Microsoft Visual C++
---

I don't know


### Compiling with Apple Xcode
---

I am not familiar with Apple's Xcode environment, but I understand that it
comes with GNU GCC & possibly Clang. The instructions above should work here as
well.
