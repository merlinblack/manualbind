// Common stuff for the examples.
#ifndef COMMON_H
#define COMMON_H

#include <lua.hpp>

void dump( lua_State* L );
void run( lua_State* L, const char* code );

#endif // COMMON_H
