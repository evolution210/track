###Makefile for track
###writer Team six

.PHONY: clean

RM = rm -f

track: track.o header.o
	gcc -o track track.o header.o -lcurses -lpthread

track.o: track.c track.h
	gcc -w -g -c track.c

header.o: header.c header.h 
	gcc -w -g -c header.c 

clean:
	$(RM) *.o
