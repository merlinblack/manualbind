include ../config.mk
INCLUDES=-I../../src -I../common
LIBS=../common/common.o ../common/LuaBinding.o
EXEC=downcast
SRC=downcast.cpp

.PHONY: clean

$(EXEC):	$(SRC) Makefile 
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) -g $(SRC) -o $(EXEC)

clean:
	-rm -f $(EXEC)

