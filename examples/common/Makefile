include ../config.mk
.PHONY: clean

# No debug info, so we don't have to step through common code.
common.o:	common.cpp common.h
	c++ $(CFLAGS) -c common.cpp -I../../src

clean:
	-rm -f common.o
