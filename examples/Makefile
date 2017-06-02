# Make all the examples
#

.PHONY: demo downcast guiexample luaref Makefile tables clean

all: demo downcast guiexample luaref Makefile tables

demo:
	@$(MAKE) --no-print-directory -C demo

downcast:
	@$(MAKE) --no-print-directory -C downcast

guiexample:
	@$(MAKE) --no-print-directory -C guiexample

luaref:
	@$(MAKE) --no-print-directory -C luaref

tables:
	@$(MAKE) --no-print-directory -C tables

clean:
	@$(MAKE) --no-print-directory -C demo clean
	@$(MAKE) --no-print-directory -C downcast clean
	@$(MAKE) --no-print-directory -C guiexample clean
	@$(MAKE) --no-print-directory -C luaref clean
	@$(MAKE) --no-print-directory -C tables clean
