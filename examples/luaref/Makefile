include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=../common/common.o ../common/LuaBinding.o
EXEC=luaref
SRC=luaref.cpp
DEBUGOUTPUT ?=-D_DEBUGOUTPUT

.PHONY: clean

$(EXEC):	$(SRC) Makefile ../../src/LuaRef.h
	c++ $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -g $(SRC) -o $(EXEC) $(DEBUGOUTPUT)

clean:
	-rm -f $(EXEC)

