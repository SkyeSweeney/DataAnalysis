
all: tbl

tbl: tbl.o tbl.h ../Utils/nodes.o ../Utils/CommonStatus.o ../Utils/hub_if.o
	g++ ${CXXFLAGS} -o $@ -pthread tbl.o `wx-config --libs` ../Utils/nodes.o ../Utils/hub_if.o ../Utils/CommonStatus.o

tbl.o: tbl.cpp
	g++ ${CXXFLAGS} -c -I../Utils `wx-config --cxxflags` tbl.cpp


clean:
	rm -f *.o
	rm -f tbl

install:
	cp tbl ../bin/.

