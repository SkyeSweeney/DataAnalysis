


all: hub

hub: hub.o
	gcc -o hub -lpthread hub.c

hub.o: hub.c msgs.h nodes.h
	gcc -c hub.c
