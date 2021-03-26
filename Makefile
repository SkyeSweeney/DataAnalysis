


all: hub

hub: hub.o nodes.o
	gcc -o hub -g -lpthread hub.c nodes.o

hub.o: hub.c msgs.h nodes.h
	gcc -c -g hub.c

nodes.o: nodes.c nodes.h
	gcc -c -g nodes.c

clean:
	rm *.o
	rm hub
