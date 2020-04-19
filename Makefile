CC=g++
SOURCES=$(wildcard src/*.cc)
OBJECTS=$(patsubst src/%.cc, obj/%.o, $(SOURCES))
LIBS=-pthread -lmosquitto -lrtmidi
FLAGS=-Wall -g
EXE=net-midid

all : $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $(EXE)

$(OBJECTS) : $(SOURCES)
	$(CC) $(FLAGS) -c $< -o $@ 

.phony clean :
	rm $(OBJECTS)
	rm $(EXE)
