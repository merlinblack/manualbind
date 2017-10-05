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

TEST_CASE( "LuaRef can index and access tables." ) {

    lua_State* L = luaL_newstate();

    {
        LuaRef table = LuaRef::newTable( L );
        table.push();
        lua_setglobal( L, "t" );

        run( L, "for i = 1, 100 do t[i] = i end" );

        REQUIRE( (int)table[25] == 25 );
        REQUIRE( (int)table[1] == 1 );
        REQUIRE( (int)table[100] == 100 );
        REQUIRE( table[1000].isNil() == true );
    }

    lua_close( L );
}
