
PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -framework OpenGL
EXT =
DEFS = `pkg-config --cflags glfw3`

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

LINK += shader.o

.PHONY:  clean

all : assign1$(EXT)

assign1$(EXT) : main.o $(LINK)
	g++ $(DEFS) -o assign1 main.o $(LINK) $(GL_LIBS)

main.o : main.cpp $(LINK)
	g++ $(DEFS) -c main.cpp

shader.o : shader.cpp shader.hpp
	g++ $(DEFS) -c shader.cpp

clean:
	rm -f *.o assign1$(EXT)
