#include <iostream>
#include <lua.hpp>
#include "common.h"
#include "LuaRef.h"

using namespace std;

LuaRef getTesting( lua_State* L )
{
    lua_getglobal( L, "testing" );
    return LuaRef::fromStack(L);
}

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    run( L, "function testing( ... ) print( '> ', ... ) end" );

    {
        LuaRef testing = LuaRef::getGlobal( L, "testing" );
        LuaRef table = LuaRef::newTable( L );
        table["testing"] = testing;

        table.push();
        lua_setglobal( L, "a" );

        run( L, "print( a.testing )" );
        run( L, "a.b = {}" );
        run( L, "a.b.c = {}" );

        cout << "Is table a table? " 
             << (table.isTable() ? "true" : "false" )
             << endl;
        cout << "Is table[\"b\"] a table? " 
             << (table["b"].isTable() ? "true" : "false" ) 
             << endl;

        table["b"]["c"]["hello"] = "World!";

        run( L, "print( a.b.c.hello )" );

        auto b = table["b"];
        b[3] = "Index 3";
        for( int i = 1; i < 5; i++ )
        {
            b.append( i );
        }
        b[1] = LuaNil();
        b.append( "Add more." );

        run( L, "for k,v in pairs( a.b ) do print( k,v ) end" );

        table["b"] = LuaNil();

        run( L, "print( a.b )" );

        testing();
        testing( 1, 2, 3 );
        testing( "Hello" );
        testing( "Hello", "World" );
        
        // Note: BAD. This makes a boat load of LuaRef copies
        // temporarily. The recursive parameter pushing
        // means later parameters get for each parameter in
        // front of them.
        // So put expensive parameters at the start!!!
        testing( "Hello", "World", 1, 2, 3, testing );

        testing( "Nigel", "Alara", "Aldora", "Ayna",
                 "Sarah", "Gavin", "Joe",    "Linda",
                 "Tom",   "Sonja", "Greg",   "Trish" 
               );

        table["testing"](testing,3,2,1,"Calling array element");
        table["testing"]();

        LuaRef newfuncref( L );

        newfuncref = testing;

        newfuncref( "Did it copy correctly?" );

        newfuncref = getTesting( L );
    }

    lua_close(L);
    return 0;
}
