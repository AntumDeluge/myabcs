
CC := g++

all: play_sound

play_sound: play_sound.cpp
	$(CC) play_sound.cpp `wx-config --cxxflags --libs` `sdl2-config --cflags --libs` -lSDL2_mixer -lpthread -o play_sound

clean:
	rm -f *.exe
