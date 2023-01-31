#include <catch2/catch_test_macros.hpp>
#include <lua.hpp>

void run( lua_State* L, const char* code )
{
    if( luaL_dostring( L, code ) )
    {
        WARN( lua_tostring( L, -1 ) );
        lua_pop( L, 1 );
    }
}

