
MyABCs 0.4-4


------------------------------------
Dependencies                        |
------------------------------------
wxWidgets 2.8.11  (http://wxwidgets.org)
PThreads          (http://sourceforge.net/projects/mingw)


--------------------------------------------
Compiling with GNU Make/GNU G++/MSYS/MinGW  |
--------------------------------------------

1) Unzip MyABCs_0.4-4_src.zip file
2) Navigate to the source folder from the command line/terminal

== GNU Make/MSYS/MinGW ==
   For GNU/Linux or Windows
   Execute:
     make
   For Windows (executable will have icon)
   Execute:
     make -f Makefile-Windows

== GNU G++/MinGW ==
   For GNU/Linux or Windows:
     g++ src/main.cpp src/abc.cpp src/gnrcabt.cpp `wx-config --cxxflags --libs` -o MyABCs -lpthread
   For Windows: (executable will have icon)
     windres resources.rc -o resources.o
     g++ src/main.cpp src/abc.cpp src/gnrcabt.cpp resources.o `wx-config --cxxflags --libs` -o MyABCs -lpthread

Notes for compiling with MinGW:
To use ready-made Makefiles, make sure that the development libraries for wxWidgets
and PThreads have been installed to the MinGW directory. Else you will have to edit
the Makefiles to tell the compiler where the libraries and headers are located with
the -L and -I flags. For example: If wxWidgets is installed to C:\wxWidgets, you
will have to add "-LC:/wxWidgets/lib -IC:/wxWidgets/include" to the lines and change
"`wx-config --cxxflags --libs`" to "`C:/wxWidgets/bin/wx-config --cxxflags --libs`".


-------------------------------------
Compiling with Microsoft Visual C++  |
-------------------------------------
I don't know


-------------------
Compiling for Mac  |
-------------------
I don't know much about Macs. If GNU G++ is available, follow same directions as GNU/Linux.