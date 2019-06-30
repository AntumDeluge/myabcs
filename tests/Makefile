
CC := g++
WXFLAGS = `wx-config --cxxflags --debug`
WXLIBS = `wx-config --libs --debug | sed -e 's|subsystem,windows|subsystem,console|'`
SDLFLAGS = `sdl2-config --cflags`
SDLLIBS = `sdl2-config --libs` -lSDL2_mixer
MISC_LIBS = -lpthread


all: play_sound sound_test alpha_only

play_sound: play_sound.cpp
	$(CC) play_sound.cpp $(WXFLAGS) $(WXLIBS) $(SDLFLAGS) $(SDLLIBS) $(MISC_LIBS) -o play_sound

sound_test: sound_test.cpp
	$(CC) sound_test.cpp $(WXFLAGS) $(WXLIBS) -o sound_test

alpha_only: alpha_only.cpp
	$(CC) alpha_only.cpp $(WXFLAGS) $(WXLIBS) -o alpha_only

clean:
	rm -f *.exe
