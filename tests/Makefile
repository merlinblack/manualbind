include ../examples/config.mk
INCLUDES=-I../src
VPATH=../src
SRCS=main.cpp test1.cpp test2.cpp test_properties.cpp test_calling.cpp LuaBinding.cpp
OBJS=$(SRCS:.cpp=.o)

.PHONY: clean ycm all

all:	test ycm

test:	$(OBJS) Makefile ../src/LuaBinding.h catch.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(LDFLAGS) $(OBJS) -o test

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< 

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

