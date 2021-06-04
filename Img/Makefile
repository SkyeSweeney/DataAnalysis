
CXXFLAGS = -g -Wall -std=c++11 -I../Utils


all: img


img: img.o img.h ../Utils/nodes.o  ../Utils/CommonStatus.o
	g++ ${CXXFLAGS} -o $@ -pthread img.o `wx-config --libs` ../Utils/nodes.o ../Utils/hub_if.o ../Utils/CommonStatus.o


img.o: img.cpp
	g++ ${CXXFLAGS} -c `wx-config --cxxflags` img.cpp


clean:
	rm -f *.o
	rm -f img

install:
	cp img ../bin/.
