#include <iostream>
#include <lua.hpp>
#include "LuaRef.h"

using namespace std;

static void dump (lua_State *L) {
    int i=lua_gettop(L);
    cout << " ----------------  Stack Dump ----------------\n";
    while(  i   ) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:
                printf("%d: “%s”\n", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("%d: %s\n",i,lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%d: %g\n",  i, lua_tonumber(L, i));
                break;
            default:
                printf("%d: %s 0x%x\n", i, lua_typename(L, t), lua_topointer(L, i));
                break;
        }
        i--;
    }
    cout << "--------------- Stack Dump Finished ---------------\n";
}

void run( lua_State* L, const char* code )
{
    cout << "code: " << code << endl;
    if( luaL_dostring( L, code ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
        lua_pop( L, 1 );
    }
}

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

        table["b"][10] = "Index 10";

        run( L, "print( a.b[10] )" );

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
