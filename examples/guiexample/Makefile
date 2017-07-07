include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=../common/common.o ../common/LuaBinding.o
EXEC=guiexample
SRC=guiexample.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile 
	c++ $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

