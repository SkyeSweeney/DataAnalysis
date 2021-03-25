


all: hub

hub: hub.o
	gcc -o hub -lpthread hub.c

hub.o: hub.c
	gcc -c hub.c
