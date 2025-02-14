VERSION = 1.0
CXX = g++

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

LIBS = -lSDL2 -lSDL2_image

SRC = mouse_sun.cpp
TARGET = mouse_sun

mouse_sun: $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(LIBS)

clean :
	rm $(TARGET)

install: mouse_sun
