
all: hub video cmd minimal img playback

hub: hub.o nodes.o
	g++ -Wall -g -o hub -pthread -lreadline hub.o nodes.o -lreadline

video: video.o nodes.o hub_if.o
	g++ -Wall -g -o video -pthread video.o nodes.o hub_if.o

cmd: cmd.o nodes.o hub_if.o
	g++ -Wall -g -o cmd cmd.c nodes.o hub_if.o -lreadline -pthread

playback: playback.o nodes.o hub_if.o
	g++ -Wall -g -o playback -pthread playback.o nodes.o hub_if.o


hub.o: hub.c msgs.h nodes.h
	g++ -Wall -g -pthread -c hub.c

video.o: video.c msgs.h nodes.h
	g++ -Wall -g -pthread -c video.c

cmd.o: cmd.c msgs.h nodes.h
	g++ -Wall -g -pthread -c cmd.c

playback.o: playback.c msgs.h nodes.h
	g++ -Wall -g -pthread -c playback.c

nodes.o: nodes.c nodes.h
	g++ -Wall -g -c nodes.c

hub_if.o: hub_if.c msgs.h nodes.h
	g++ -Wall -g -c hub_if.c

clean:
	rm -f *.o
	rm -f hub
	rm -f video
	rm -f cmd


minimal: minimal.o
	g++ -o minimal minimal.o `wx-config --libs`


minimal.o: minimal.cpp
	g++ -c -Wall `wx-config --cxxflags` minimal.cpp


img: img.o img.h
	g++ -o img img.o `wx-config --libs`


img.o: img.cpp
	g++ -c -Wall `wx-config --cxxflags` img.cpp
