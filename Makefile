
all: 
	cd Utils && make
	cd Hub && make
	cd Cmd && make
	cd Playback && make
	cd Img && make
	ctags *




clean:
	cd Utils && make clean
	cd Hub && make clean
	cd Cmd && make clean
	cd Playback && make clean
	cd Img && make clean
