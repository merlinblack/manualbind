# Lua pkg-config name is different under MacOS brew
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LUA=lua5.3
else
	LUA=lua
endif
CXXFLAGS+=$(shell pkg-config --cflags $(LUA) )
LDFLAGS+=$(shell pkg-config --libs   $(LUA) )
CXXFLAGS+=-std=c++11 -O3 -Wall
