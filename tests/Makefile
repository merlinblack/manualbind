include ../examples/config.mk
INCLUDES=-I../src

.PHONY: clean all

all:	test ycm

test:	test1.cpp test2.cpp test_properties.cpp Makefile ../src/LuaBinding.h catch.hpp main.o LuaBinding.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) main.o LuaBinding.o test1.cpp test2.cpp test_properties.cpp -o test

main.o:	main.cpp catch.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

LuaBinding.o:	../src/LuaBinding.cpp ../src/LuaBinding.h
	$(CXX) $(CXXFLAGS) -I../src -c ../src/LuaBinding.cpp

clean:
	-rm -f test
	-rm -f *.o
	-rm .ycm_extra_conf.py

#YouCompleteMe extra config generation.
ycm: .ycm_extra_conf.py 

.ycm_extra_conf.py:
	@echo "def FlagsForFile( filename, **kwargs ):" > .ycm_extra_conf.py
	@echo "    return { " >> .ycm_extra_conf.py 
	@printf %s "        'flags': [ '-x', 'c++', '-I$(shell realpath ../src)', " >> .ycm_extra_conf.py
	@printf %s $(foreach flag, $(CXXFLAGS), "'$(flag)', " ) >> .ycm_extra_conf.py
	@echo "]," >> .ycm_extra_conf.py
	@echo "    }" >> .ycm_extra_conf.py
	@echo "Generated YouCompeteMe configuration."

