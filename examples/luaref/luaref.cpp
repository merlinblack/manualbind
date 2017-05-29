#include <iostream>
#include <lua.hpp>
#include "LuaRef.h"

using namespace std;

void run( lua_State* L, const char* code )
{
    if( luaL_dostring( L, code ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
    }
}


int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    run( L, "function testing( ... ) print( '> ', ... ) end" );

    {
        LuaRef testing = getGlobal( L, "testing" );
        LuaRef tbl = newTable( L );
        tbl["testing"] = testing;
        tbl.push(L);
        lua_setglobal( L, "a" );
        

        testing();
        testing( 1, 2, 3 );
        testing( "Hello" );
        testing( "Hello", "World" );
        testing( "Hello", "World", 1, 2, 3, testing );

        testing( "Nigel", "Alara", "Aldora", "Ayna",
                "Sarah", "Gavin", "Joe",    "Linda",
                "Tom",   "Sonja", "Greg",   "Trish" 
               );

        testing = getGlobal( L, "a" );

        testing["testing"](testing,3,2,1,"Calling array element");
        testing["testing"]();
    }

    lua_close(L);
    return 0;
}
