include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=-llua ../common/common.o
EXEC=tables
SRC=tables.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile 
	c++ $(CFLAGS) $(INCLUDES) $(LIBS) $(LFLAGS) -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

