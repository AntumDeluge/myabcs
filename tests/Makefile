
CC := g++
WXFLAGS = `wx-config --cxxflags`
WXLIBS = `wx-config --libs`
SDLFLAGS = `sdl2-config --cflags`
SDLLIBS = `sdl2-config --libs` -lSDL2_mixer
MISC_LIBS = -lpthread

all: play_sound sound_test

play_sound: play_sound.cpp
	$(CC) play_sound.cpp $(WXFLAGS) $(WXLIBS) $(SDLFLAGS) $(SDLLIBS) $(MISC_LIBS) -o play_sound

sound_test: sound_test.cpp
	$(CC) sound_test.cpp $(WXFLAGS) $(WXLIBS) -o sound_test

clean:
	rm -f *.exe
