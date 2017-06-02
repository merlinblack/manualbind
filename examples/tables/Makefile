INCLUDES=-I../../src -I../common
LIBS=-llua ../common/common.o
EXEC=tables
SRC=tables.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile 
	g++ $(INCLUDES) $(LIBS) -O3 -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

