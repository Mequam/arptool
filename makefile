arptool-1.0.o: arptool-1.0.cc strmac.h
	g++ -c arptool-1.0.cc
build: arptool-1.0.o
	g++ arptool-1.0.o -lparse -o arptool
install: arptool
	mv arptool /usr/bin
