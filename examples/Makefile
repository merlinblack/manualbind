# Make all the examples
#

.PHONY: common demo downcast guiexample luaref Makefile tables clean

all: demo downcast upcast guiexample luaref Makefile tables

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

common/LuaBinding.o:
	@$(MAKE) --no-print-directory -C common

clean:
	@$(MAKE) --no-print-directory -C demo clean
	@$(MAKE) --no-print-directory -C downcast clean
	@$(MAKE) --no-print-directory -C upcast clean
	@$(MAKE) --no-print-directory -C guiexample clean
	@$(MAKE) --no-print-directory -C luaref clean
	@$(MAKE) --no-print-directory -C tables clean
	@$(MAKE) --no-print-directory -C common clean
