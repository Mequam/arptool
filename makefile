arptool.o: arptool.cc strmac.h version.h help.h
	g++ -c arptool.cc
build: arptool.o
	g++ arptool.o -lparse -o arptool
install: arptool
	cp arptool $(DESTDIR)/usr/bin
clean:
	rm arptool.o
	rm arptool
