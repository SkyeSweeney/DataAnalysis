


all: master master.dwarf vars.ini

master: master.c ../msgs.h
	gcc -g -Wall -I.. -I/usr/local/include/libxml2 master.c -L/usr/local/lib -lxml2 -lz -llzma -lm -ldl -o master

master.dwarf: master
	dwarfdump -i master > master.dwarf

vars.ini: master.dwarf
	./midget.py master.dwarf


clean:
	rm -f *.o
	rm -f master
	rm -f *.dwarf
