
CXXFLAGS = -g -Wall -std=c++11 -I../Utils/


all: cmd

cmd: cmd.o ../Utils/nodes.o ../Utils/hub_if.o
	g++ ${CXXFLAGS} -o $@ cmd.cpp ../Utils/nodes.o ../Utils/hub_if.o -lreadline -pthread

cmd.o: cmd.cpp ../Utils/msgs.h ../Utils/nodes.h
	g++ ${CXXFLAGS} -pthread -c cmd.cpp

clean:
	rm -f *.o
	rm -f cmd
