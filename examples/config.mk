# -std=c++11 needed for Xcode g++
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS+=-std=c++11
endif
# Local install of Lua
#CFLAGS+=-I/usr/local/include
#LFLAGS+=-L/usr/local/lib
CFLAGS+=-O3
