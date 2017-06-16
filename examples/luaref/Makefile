include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=-llua ../common/common.o
EXEC=luaref
SRC=luaref.cpp
DEBUGOUTPUT ?=-D_DEBUGOUTPUT

.PHONY: clean

$(EXEC):	$(SRC) Makefile ../../src/LuaRef.h
	c++ $(DEBUGOUTPUT) $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -O0 -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

