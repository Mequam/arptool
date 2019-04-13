arptool.o: arptool-1.0.cc strmac.h
	g++ -c arptool-1.0.cc
build: arptool.o
	g++ arptool-1.0.o -lparse -o arptool
install:
	mv arptool /usr/bin
