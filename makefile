CC=g++
CFLAGS=
OBJS = ./World.o \
	./MyB2Draw.o \
	./BoxCar.o \
	./Car.o \
	./GUI/Forms/Credits.o \
  ./GUI/Forms/TimeControl.o



INCS = -I. -IMath -IGeneric -IGUI -IGUI/Forms
CFLAGS = $(INCS) -g -std=c++0x -O2
LDFLAGS =-lSDL -lGL -lSDL_image -lengine -L.

headers:
	$(CC) $(CFLAGS) -c StdAfx.h -o StdAfx.h.gch

all: headers $(OBJS)
	$(CC) -o BlindAM $(OBJS) libBox2D.a $(LDFLAGS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f BlindAM
	find ./ -name "*.o" -exec rm -rf {} \;
	rm -f StdAfx.h.gch

