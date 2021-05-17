
all: 
	cd Utils && make
	cd Hub && make
	cd Cmd && make
	cd Playback && make
	cd Img && make
	cd Tbl && make
	ctags *


install: 
	cd Utils && make install
	cd Hub && make install
	cd Cmd && make install
	cd Playback && make install
	cd Img && make install
	cd Tbl && make install



clean:
	cd Utils && make clean
	cd Hub && make clean
	cd Cmd && make clean
	cd Playback && make clean
	cd Img && make clean
	cd Tbl && make clean
