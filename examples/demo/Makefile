include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=../common/common.o
EXEC=demo
SRC=demo.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile ../../src/LuaBinding.h *.h
	c++ $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

