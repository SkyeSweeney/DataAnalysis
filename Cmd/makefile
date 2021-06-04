
all: cmd

cmd: cmd.o cmd.h ../Utils/nodes.o ../Utils/CommonStatus.o ../Utils/hub_if.o
	g++ ${CXXFLAGS} -o $@ -pthread cmd.o `wx-config --libs` ../Utils/nodes.o ../Utils/hub_if.o ../Utils/CommonStatus.o

cmd.o: cmd.cpp
	g++ ${CXXFLAGS} -c -I../Utils `wx-config --cxxflags` cmd.cpp


clean:
	rm -f *.o
	rm -f cmd

install:
	cp cmd ../bin/.

