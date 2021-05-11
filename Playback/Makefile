
CXXFLAGS = -g -Wall -std=c++11 -I../Utils


all: playback

playback: playback.o ../Utils/nodes.o ../Utils/hub_if.o
	g++ ${CXXFLAGS} -o $@ -pthread playback.o ../Utils/nodes.o ../Utils/hub_if.o

playback.o: playback.cpp ../Utils/msgs.h ../Utils/nodes.h
	g++ ${CXXFLAGS} -pthread -c playback.cpp

clean:
	rm -f *.o
	rm -f playback
