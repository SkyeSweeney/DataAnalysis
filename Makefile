


all: hub video

hub: hub.o nodes.o
	gcc -Wall -o hub -g -lpthread hub.c nodes.o

video: video.o nodes.o hub_if.o
	gcc -Wall -o video -g -lpthread video.c nodes.o hub_if.o

hub.o: hub.c msgs.h nodes.h
	gcc -Wall -c -g hub.c

nodes.o: nodes.c nodes.h
	gcc -Wall -c -g nodes.c

hub_if.o: hub_if.c msgs.h nodes.h
	gcc -Wall -c -g hub_if.c

clean:
	rm *.o
	rm hub
	rm video
