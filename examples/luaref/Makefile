include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=-llua ../common/common.o
EXEC=luaref
SRC=luaref.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile ../../src/LuaRef.h
	c++ -D_DEBUGOUTPUT $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -O3 -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

