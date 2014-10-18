MyABCs: abc.o main.o gnrcabt.o src/res.h
	g++ abc.o main.o gnrcabt.o `wx-config --libs` -o MyABCs -lpthread
abc.o: src/abc.cpp src/abc.h
	g++ -c src/abc.cpp `wx-config --cxxflags` -o abc.o
main.o: src/main.cpp src/main.h
	g++ -c src/main.cpp `wx-config --cxxflags` -o main.o
gnrcabt.o: src/gnrcabt.cpp src/gnrcabt.h
	g++ -c src/gnrcabt.cpp `wx-config --cxxflags` -o gnrcabt.o