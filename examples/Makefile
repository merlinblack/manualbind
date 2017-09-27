# Make all the examples
#
include config.mk

.PHONY: common demo downcast guiexample luaref Makefile tables clean ycm

all: demo downcast upcast guiexample luaref Makefile tables ycm

demo:	common
	@$(MAKE) --no-print-directory -C demo

downcast:	common
	@$(MAKE) --no-print-directory -C downcast

upcast:	common
	@$(MAKE) --no-print-directory -C upcast

guiexample: common
	@$(MAKE) --no-print-directory -C guiexample

luaref:	common
	@$(MAKE) --no-print-directory -C luaref

tables:	common
	@$(MAKE) --no-print-directory -C tables

common:	common/common.o common/LuaBinding.o

common/common.o:
	@$(MAKE) --no-print-directory -C common

common/LuaBinding.o: ../src/LuaBinding.cpp
	@$(MAKE) --no-print-directory -C common

clean:
	@$(MAKE) --no-print-directory -C demo clean
	@$(MAKE) --no-print-directory -C downcast clean
	@$(MAKE) --no-print-directory -C upcast clean
	@$(MAKE) --no-print-directory -C guiexample clean
	@$(MAKE) --no-print-directory -C luaref clean
	@$(MAKE) --no-print-directory -C tables clean
	@$(MAKE) --no-print-directory -C common clean
	rm -f .ycm_extra_conf.py

#YouCompleteMe extra config generation.
ycm: .ycm_extra_conf.py 

.ycm_extra_conf.py:
	@echo "def FlagsForFile( filename, **kwargs ):" > .ycm_extra_conf.py
	@echo "    return { " >> .ycm_extra_conf.py 
	@printf %s "        'flags': [ '-x', 'c++', '-I$(shell realpath ../src)', '-I$(shell realpath common)', " >> .ycm_extra_conf.py
	@printf %s $(foreach flag, $(CXXFLAGS), "'$(flag)', " ) >> .ycm_extra_conf.py
	@echo "]," >> .ycm_extra_conf.py
	@echo "    }" >> .ycm_extra_conf.py
	@echo "Generated YouCompeteMe configuration."

