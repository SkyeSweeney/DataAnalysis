
all: hub video cmd minimal img

hub: hub.o nodes.o
	g++ -Wall -g -o hub -pthread -lreadline hub.o nodes.o -lreadline

video: video.o nodes.o hub_if.o
	g++ -Wall -o video -g -pthread video.o nodes.o hub_if.o

cmd: cmd.o nodes.o hub_if.o
	g++ -Wall -o cmd -g  cmd.c nodes.o hub_if.o -lreadline -pthread


hub.o: hub.c msgs.h nodes.h
	g++ -Wall -pthread -c -g hub.c

video.o: video.c msgs.h nodes.h
	g++ -Wall -pthread -c -g video.c

cmd.o: cmd.c msgs.h nodes.h
	g++ -Wall -pthread -c -g cmd.c

nodes.o: nodes.c nodes.h
	g++ -Wall -c -g nodes.c

hub_if.o: hub_if.c msgs.h nodes.h
	g++ -Wall -c -g hub_if.c

clean:
	rm -f *.o
	rm -f hub
	rm -f video
	rm -f cmd


minimal: minimal.o
	g++ -o minimal minimal.o `wx-config --libs`


minimal.o: minimal.cpp
	g++ -c -Wall `wx-config --cxxflags` minimal.cpp


img: img.o
	g++ -o img img.o `wx-config --libs`


img.o: img.cpp
	g++ -c -Wall `wx-config --cxxflags` img.cpp
