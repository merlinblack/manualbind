# -std=c++11 needed for Xcode g++
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LUA=lua5.3
else
	LUA=lua
endif
CFLAGS+=$(shell pkg-config --cflags $(LUA) )
LFLAGS+=$(shell pkg-config --libs   $(LUA) )
CFLAGS+=-std=c++11
CFLAGS+=-O3 -Wall
