include ../config.mk
.PHONY: clean

common.o:	common.cpp common.h
	c++ $(CFLAGS) -O3 -c common.cpp -I../../src

clean:
	-rm -f common.o
