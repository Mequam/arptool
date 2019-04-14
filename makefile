arptool-1.1.o: arptool-1.1.cc strmac.h version.h help.h
	g++ -c arptool-1.1.cc
build: arptool-1.1.o
	g++ arptool-1.1.o -lparse -o arptool
install: arptool
	cp arptool $(DESTDIR)/usr/bin
clean:
	rm arptool-1.1.o
	rm arptool
