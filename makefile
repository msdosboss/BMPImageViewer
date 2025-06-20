CC = gcc
LINKERFLAGS = `sdl2-config --cflags --libs`
CFLAGS = -Wall
OBJS = imageViewer.o bitpack.o display.o

all: $(OBJS)
	$(CC) $(OBJS) -o main $(LINKERFLAGS) -lm

imageViewer.o: imageViewer.c
	gcc -c imageViewer.c $(CFLAGS) -o imageViewer.o

display.o: display.c
	gcc -c display.c $(CFLAGS) -o display.o

bitpack.o: bitpack.c
	gcc -c bitpack.c $(CFLAGS) -o bitpack.o

clean:
	rm -f main
	rm -f *.o

debug: $(OBJS)
	$(CC) $(OBJS) -g -o main $(LINKERFLAGS) -lm -DDEBUG