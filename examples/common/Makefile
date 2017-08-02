include ../config.mk
.PHONY: all clean

all: common.o LuaBinding.o

# No debug info, so we don't have to step through common code.
common.o:	common.cpp common.h Makefile
	$(CXX) $(CXXFLAGS) -c common.cpp -I../../src

LuaBinding.o:	../../src/LuaBinding.cpp Makefile
	$(CXX) $(CXXFLAGS) -c ../../src/LuaBinding.cpp -I../../src

clean:
	-rm -f common.o
	-rm -f LuaBinding.o
