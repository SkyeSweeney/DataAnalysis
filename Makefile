
CXXFLAGS = -g -Wall -std=c++11


all: hub video cmd minimal img playback
	ctags *

hub: hub.o nodes.o
	g++ ${CXXFLAGS} -o $@ -pthread -lreadline hub.o nodes.o -lreadline

video: video.o nodes.o hub_if.o
	g++ ${CXXFLAGS} -o $@ -pthread video.o nodes.o hub_if.o

cmd: cmd.o nodes.o hub_if.o
	g++ ${CXXFLAGS} -o $@ cmd.cpp nodes.o hub_if.o -lreadline -pthread

playback: playback.o nodes.o hub_if.o
	g++ ${CXXFLAGS} -o $@ -pthread playback.o nodes.o hub_if.o

hub.o: hub.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -pthread -c hub.cpp

video.o: video.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -pthread -c video.cpp

cmd.o: cmd.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -pthread -c cmd.cpp

playback.o: playback.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -pthread -c playback.cpp

nodes.o: nodes.cpp nodes.h
	g++ ${CXXFLAGS} -c nodes.cpp

hub_if.o: hub_if.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -c hub_if.cpp


minimal: minimal.o
	g++ ${CXXFLAGS} -o $@ minimal.o `wx-config --libs`


minimal.o: minimal.cpp
	g++ ${CXXFLAGS} -c `wx-config --cxxflags` minimal.cpp


img: img.o img.h nodes.o 
	g++ ${CXXFLAGS} -o $@ -pthread img.o `wx-config --libs` nodes.o hub_if.o


img.o: img.cpp
	g++ ${CXXFLAGS} -c `wx-config --cxxflags` img.cpp


clean:
	rm -f *.o
	rm -f hub
	rm -f video
	rm -f cmd
	rm -f img minimal playback
