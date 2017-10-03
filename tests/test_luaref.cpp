#include "catch.hpp"
#include "LuaRef.h"

void run( lua_State* L, const char* code );

using namespace ManualBind;

TEST_CASE( "Can call lua functions with LuaRef" ) {

    lua_State* L = luaL_newstate();

    run( L, "function set( nx, ny, nz ) x = nx y = ny z = nz end" );

    {
        LuaRef set( L, "set" );

        REQUIRE( set.isFunction() == true );

        set( "Hello", 10, set );

        LuaRef x( L, "x" );
        LuaRef y( L, "y" );
        LuaRef z( L, "z" );

        REQUIRE( x.isString() == true );
        REQUIRE( y.isNumber() == true );
        REQUIRE( z.isFunction() == true );

    } // Importaint: LuaRefs going out of scope and destructing, before lua_close();

    lua_close( L );
}
