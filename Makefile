OS:=$(shell uname)

ifeq ($(OS), Darwin)
    CXXFLAGS = -Iext/gl3w -std=c++14 -Wall -pedantic -DENABLE_SOUND
    LIB = -ldl -framework OpenGL -framework Cocoa -framework CoreFoundation -lglfw -lSDL -lSDL_mixer -lpthread -lSDLmain
else ifeq ($(OS), Linux)
    CXXFLAGS = -Iext/gl3w -std=c++14 -Wall -pedantic -DENABLE_SOUND
    LIB = -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lSDL -lSDL_mixer -lSDLmain
else
    $(error Your OS $(OS) is not supported.)
    exit 1
endif

BIN=main
OBJ=$(BIN).o common.o world.o camera.o dungeon.o floor.o gameobject.o janitor.o door.o room.o roomtextparser.o wall.o hero.o boss.o $(CURDIR)/src/Cleanable/puddle.o ValueIteration.o $(CURDIR)/src/Cleanable/artifact.o pathfinder.o pathfindernode.o emitter.o particle.o

default: build

build: $(BIN)

test: build
	./$(BIN)

%.o: src/%.cpp
	g++ -c -g $(CXXFLAGS) -o $@ $<

$(BIN): $(OBJ)
	g++ -o $@ $(OBJ) $(LIB)

clean:
	- rm -f $(BIN) $(OBJ)