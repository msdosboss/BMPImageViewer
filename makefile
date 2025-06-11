CC = gcc
CFLAGS = `sdl2-config --cflags --libs`
all: imageViewer.o bitpack.o display.o
	$(CC) display.o bitpack.o imageViewer.o -o main $(CFLAGS)

imageViewer.o: imageViewer.c
	gcc -c imageViewer.c -o imageViewer.o

display.o: display.c
	gcc -c display.c -o display.o

bitpack.o: bitpack.c
	gcc -c bitpack.c -o bitpack.o

clean:
	rm -f main
	rm -f *.o
