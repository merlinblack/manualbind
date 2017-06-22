include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=-llua ../common/common.o
EXEC=upcast
SRC=upcast.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile ../../src/LuaBinding.h
	c++ $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

