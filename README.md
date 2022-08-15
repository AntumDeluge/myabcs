
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


### Building with CMake (recommended)
---

See [CMake Manual](https://cmake.org/cmake/help/latest/manual/cmake.1.html) for in-depth details on how to use CMake from the command line.

#### Basic Usage

To configure with default options execute: `cmake path/to/source`

After configuration is complete run any of the following:

- `cmake --build ./` (compile)
- `cmake --build ./ --target install` or `cmake --install` (compile & install)

#### Options

*To get a detailed list of available CMake configuration options run `cmake -LA path/to/source`.*

The following are the main options that can be configured using the `-D` switch:

- *CMAKE_BUILD_TYPE*
  - Determines if the MyABCs executable is built with debugging symbols.
  - type: string
  - options: *Release*, *Debug*
  - default: *Release*
- *CMAKE_INSTALL_PREFIX*
  - The directory under which the application's files will be installed.
  - type: path
- *PORTABLE*
  - Builds app for portability.
  - type: bool
  - default: *ON*
- *DATAROOT*
  - Directory where app will search data files. Only available when `PORTABLE=OFF`.
  - type: path
  - default: *CMAKE_INSTALL_PREFIX/share/myabc*
- *AUDIO_FORMAT*
  - Audio files format.
  - Requires [FFmpeg](https://ffmpeg.org/)
  - type: string
  - options:
    - *copy*:   keep uncompressed [FLAC](https://xiph.org/flac/) (no conversion done)
    - *vorbis*: convert to [Vorbis/OGG](https://xiph.org/vorbis/) (recommended)
    - *mp3*:    convert to [MPEG Audio Layer III (MP3)](https://en.wikipedia.org/wiki/MP3)
    - *pcm*:    convert to uncompressed [PCM/WAV](https://en.wikipedia.org/wiki/WAV)
  - default: *vorbis* if FFmpeg is available, *copy* otherwise
- *USE_BUNDLED*
  - Build using bundled libraries instead of those found on system.
  - type: bool
  - default: *OFF*
  - see section *[Building Internal Libraries](#building-internal-libraries)*
- *BUNDLED_LIBPREFIX*
  - Defines the root directory under which the bundled libraries are stored.
  - Automatically detected.

#### Examples

Portable configuration using MP3 audio:

```sh
cmake -DAUDIO_FORMAT=mp3 -DCMAKE_INSTALL_PREFIX="$(pwd)/install" path/to/source
```

Portable configuration using non-system libraries:

```sh
cmake -DUSE_BUNDLED=ON -DCMAKE_INSTALL_PREFIX="$(pwd)/install" path/to/source
```

Non-portable configuration:

```sh
cmake -DPORTABLE=OFF path/to/source
```

Non-portable configuration with a custom data directory:

```sh
cmake -DPORTABLE=OFF -DDATAROOT="/home/username/myabcs" path/to/source
```

#### GUI

CMake also offers a GUI frontend for generating the Makefiles. For more information, see [this page](https://cmake.org/runningcmake/).

<img src="https://cmake.org/wp-content/uploads/2018/10/cmake-gui.png" style="display:block;margin-left:auto;margin-right:auto;" />


### Building with GNU Make (GNU G++, Clang, MSYS/MinGW, etc.) (DEPRECATED)
---

1. Navigate to the source folder from the command line/terminal.
2. To compile execute:
    - `make`
3. To install execute:
    - `make install` (on Windows this puts all files into "build/stage/myabcs" directory)
4. To zip files into release archive execute:
    - `make pack`

If you want to compile & use the bundled static libraries, execute the following
from the source folder:

```sh
$ make build-libs
$ make USE_BUNDLED=1
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
    build-libs      Compile bundled libraries.

Environment variables:
    STATIC          Define to create static build (note: SDL
                    libraries are still linked dynamically).
    WXVERSION       Use a version of wxWidgets other than the default.
    EXTRA_LIBS      Use this variable to manually link to libraries
                    (example: EXTRA_LIBS="-lpng -lz"). This should only
                    be used if linker errors are encountered.
    REL_SUFFIX      Appends text to filename of release archive when
                    "make pack" is called.
    BUILD_LIBS      Only build specified bundled libs with build-libs target.
    USE_BUNDLED     Set to non-zero to link to bundled libs. Only works if
                    libraries previously built with build-libs target called.
    WXCONFIG        Use a custom wx-config
    SDLCONFIG       Use a custom sdl2-config
</pre>


### Building with Microsoft Visual C++
---

I don't know


### Building with Apple Xcode
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

- CMake: `cmake -DUSE_BUNDLED=ON`
- generic GNU Makefile: `make USE_BUNDLED=1`

For configuration & build settings of individual libraries, see the [libraries/CONFIG directory](libraries/CONFIG).

***NOTE:*** *In the future, the shell script may be replaced with another type of script for multi-platform use*
