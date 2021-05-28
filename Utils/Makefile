
CXXFLAGS = -g -Wall -std=c++11


all: mkSample mkIndex hub_if.o nodes.o


mkSample: mkSample.cpp msgs.h
	g++ ${CXXFLAGS} -o $@ mkSample.cpp

mkIndex: mkIndex.cpp msgs.h
	g++ ${CXXFLAGS} -o $@ mkIndex.cpp

hub_if.o: hub_if.cpp msgs.h nodes.h
	g++ ${CXXFLAGS} -c hub_if.cpp

nodes.o: nodes.cpp nodes.h
	g++ ${CXXFLAGS} -c nodes.cpp


clean:
	rm -f *.o
	rm -f mkIndex
	rm -f mkSample
	rm -f sample.log
	rm -f sample.index

install:
	cp mkSample ../bin/.
	cp mkIndex ../bin/.
	cp start ../bin/.
	cp terms ../bin/.
