
CXXFLAGS = -g -Wall -std=c++11 -I../Utils/


all: hub


hub: hub.o ../Utils/nodes.o
	g++ ${CXXFLAGS} -o $@ -pthread -lreadline hub.o ../Utils/nodes.o -lreadline

hub.o: hub.cpp ../Utils/msgs.h ../Utils/nodes.h
	g++ ${CXXFLAGS} -pthread -c hub.cpp

clean:
	rm -f *.o
	rm -f hub
