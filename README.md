
# MyABCs Educational Software for Children

<img src="https://antumdeluge.github.io/myabcs/data/icon.svg" style="display:block;margin-left:auto;margin-right:auto;" />

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
CMake           <a href="https://cmake.org/">https://cmake.org/</a>
pkg-config      <a href="https://www.freedesktop.org/wiki/Software/pkg-config/">https://www.freedesktop.org/wiki/Software/pkg-config/</a>
wxWidgets 3.1+  <a href="https://wxwidgets.org/">https://wxwidgets.org/</a>
wxSVG           <a href="http://wxsvg.sourceforge.net/">http://wxsvg.sourceforge.net/</a>
SDL2            <a href="https://libsdl.org/">https://libsdl.org/</a>
SDL2_mixer      <a href="https://www.libsdl.org/projects/SDL_mixer/">https://www.libsdl.org/projects/SDL_mixer/</a>
PThreads        Linux: Check your package manager for a library
                  implementation.
                Windows: If your compiler does not have an
                  implementation you can download <a href="http://sourceware.org/pthreads-win32/">Pthreads-w32</a>.
                  The <a href="https://www.msys2.org/">MSYS2</a> & <a href="http://mingw-w64.org/doku.php">MinGW-w64</a> projects also supply a
                  more-actively developed version known as
                  <i>winpthreads</i>.
</pre>


### Using CMake (recommended)

#### Command line

Create an empty directory where you would like build files to be generated. Navigate to that directory from a command line/terminal. For default settings invoke the following command (**NOTE:** The *&lt;path_to_source&gt;* argument should be the location of the MyABCs source directory):

```sh
cmake <path_to_source>
```

If you need to manually set the Makefile generator, execute `cmake --help` to see which generators are available. Then use the `-G` switch to set the generator. For example, if you are on Windows using the [MSYS2/MinGW-w64 environment](https://www.msys2.org/), then you may want to do the following:

```sh
cmake -G "MSYS Makefile" <path_to_source>
```

**Customizing settings:**

The following are the main values that can be defined using the `-D` switch:

- ***CMAKE_BUILD_TYPE***
  - Determines if the MyABCs executable is built with debugging symbols.
  - options: Release, Debug
  - default: Release
- ***CMAKE_INSTALL_PREFIX***
  - The directory under which the application's files will be installed.
- ***AUDIO_FORMAT***
  - The output audio file format.
  - Requires [FFmpeg](https://ffmpeg.org/)
  - options:
    - ***copy*** *(default if FFmpeg not available)*: keep uncompressed [FLAC](https://xiph.org/flac/) (no conversion done)
	- ***vorbis*** *(default)*: convert to [Vorbis/OGG](https://xiph.org/vorbis/) (recommended)
	- ***mp3***: convert to [MPEG Audio Layer III (MP3)](https://en.wikipedia.org/wiki/MP3)
	- ***pcm***: convert to uncompressed [PCM/WAV](https://en.wikipedia.org/wiki/WAV)
- ***USE_BUILTIN***
  - Set to ***ON*** to prioritize using "built-in" libraries.
  - default: ***OFF***
  - see section *[Building Internal Libraries](#building-internal-libraries)*
- ***BUILTIN_LIBPREFIX***
  - Defines the root directory under which the "built-in" libraries are stored.
  - Automatically detected.

Example:

```sh
cmake -G "GNU Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local \
	-DAUDIO_FORMAT=vorbis -DUSE_BUILTIN=ON ../
```

Once the build files have been generated using the previous command, the app can be built with appropriate "make" command. The aformentioned generates makefiles for the GNU make system (***NOTE:*** *on some systems, such as [FreeBSD](https://www.freebsd.org/), the GNU make executable may be `gmake`*). So to build the app, execute the following:

```sh
make
```

Then it can be installed to the system using:

```sh
make install
```

If you do not want to install the app's files to the system, you can package the app in a .zip archive with the following command:

```sh
make package
```

#### GUI

CMake also offers a GUI frontend for generating the Makefiles. For more information, see [this page](https://cmake.org/runningcmake/).

<img src="https://cmake.org/wp-content/uploads/2018/10/cmake-gui.png" style="display:block;margin-left:auto;margin-right:auto;" />


### Compiling with GNU Make (GNU G++, Clang, MSYS/MinGW, etc.) (DEPRECATED)
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

## Building Internal Libraries

The source includes a [shell script](https://en.wikipedia.org/wiki/Shell_script) for compiling internal libraries. From the top-level of the source directory, it is invoked like this:

```sh
./libraries/build.sh <build_string>
```

The `&lt;build_string&gt;` parameter symbolizes the platform on which the app/libraries are being built. This parameter is set automatically if invoked from the Makefile with the `make` command (functionality may be removed in future versions):

```sh
make build-libs
```

You can also specify which libraries to build using the `BUILD_LIBS` variable:

```sh
BUILD_LIBS="wxWidgets wxSVG SDL2 SDL2_mixer" ./libraries/build.sh <build_string>
# or
make build-libs BUILD_LIBS="wxWidgets wxSVG SDL2 SDL2_mixer"
```

If the `BUILD_LIBS` variable is not used, the script will build the wxWidgets, wxSVG, SDL2, SDL2_mixer libraries & any required dependencies.

The script will attempt to download the sources for each library, compile, & install them under the directory `libraries/libprefix-<build_string>`. Compiling against & linking to these libraries will take priority when building with one of the following methods.

- CMake: `cmake -DUSE_BUILTIN=ON`
- generic GNU Makefile: `make USE_BUILTIN=1`

For configuration & build settings of individual libraries, see the [libraries/CONFIG directory](libraries/CONFIG).

***NOTE:*** *In the future, the shell script may be replaced with another type of script for multi-platform use*
