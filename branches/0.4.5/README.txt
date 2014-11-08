
MyABCs 0.4.5


------------------------------------
Dependencies                        |
------------------------------------
wxWidgets 2.8   (http://wxwidgets.org)
PThreads        (http://sourceforge.net/projects/mingw)


-----------------------------------------------------------------
Compiling with GNU Make, GNU G++, MSYS/MinGW, BSD Make, & Clang  |
-----------------------------------------------------------------

Navigate to the source folder from the command line/terminal


== GNU Make/MSYS/MinGW ==

    ‣ For GNU/Linux or Windows
      Execute:
         make
    ‣ For Windows (executable will have icon)
      Execute:
        make -f Makefile-Windows
    

== GNU G++/MinGW ==

    ‣ For GNU/Linux or Windows:
    
        g++ src/main.cpp src/abc.cpp src/gnrcabt.cpp `wx-config --cxxflags \
            --libs` -o MyABCs -lpthread
    
    ‣ For Windows: (executable will have icon)
    
	    windres resources.rc -o resources.o
	    g++ src/main.cpp src/abc.cpp src/gnrcabt.cpp resources.o `wx-config \
            --cxxflags --libs` -o MyABCs -lpthread
    
== Clang ==
    
    Clang's syntax is compatible with GCC. Use the instructions above but
    replace "g++" with "clang++".

Notes for compiling with MinGW:
To use ready-made Makefiles, make sure that the development libraries for
wxWidgets and PThreads have been installed to the MinGW directory. Else you
will have to edit the Makefiles to tell the compiler where the libraries and
headers are located with the -L and -I flags. For example: If wxWidgets is
installed to C:\wxWidgets, you will have to add "-LC:/wxWidgets/lib
-IC:/wxWidgets/include" to the lines and change "`wx-config --cxxflags --libs`"
to "`C:/wxWidgets/bin/wx-config --cxxflags --libs`".

Notes for Debian based linux systems:
The wxSound module has not worked for me on Debian systems that I have used for
testing. If this is the case you may want to try compiling wxWidgets yourself,
or try using the clang++ compiler.


-------------------------------------
Compiling with Microsoft Visual C++  |
-------------------------------------

I don't know


----------------------------
Compiling with Apple Xcode  |
----------------------------

I am not familiar with Apple's Xcode environment, but I understand that it
comes with GNU GCC & possibly Clang. The instructions above should work here as
well.
