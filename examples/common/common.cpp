#include "common.h"
#include <iostream>

using std::cout;
using std::endl;

void dump( lua_State* L )
{
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
            printf("%d: %s 0x%lX\n", i, lua_typename(L, t), (unsigned long)lua_topointer(L, i));
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

void printtable( lua_State* L, int index )
{
    int t;
    if( lua_type( L, index ) != LUA_TTABLE ) {
        printf( "Index: %d is not a table.", index );
        return;
    }

    lua_pushnil( L );

    printf( "Key - Value\n" );
    while( lua_next( L, index ) != 0 ) {
        t = lua_type(L, -2);
        switch (t) {
        case LUA_TSTRING:
            printf("“%s” - ", lua_tostring(L, -2));
            break;
        case LUA_TBOOLEAN:
            printf("%s - ", lua_toboolean(L, -2) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("%g - ", lua_tonumber(L, -2));
            break;
        default:
            printf("%s 0x%lX - ", lua_typename(L, t), (unsigned long)lua_topointer(L, -2 ));
            break;
        }
        t = lua_type(L, -1);
        switch (t) {
        case LUA_TSTRING:
            printf("“%s”\n", lua_tostring(L, -1));
            break;
        case LUA_TBOOLEAN:
            printf("%s\n", lua_toboolean(L, -1) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("%g\n", lua_tonumber(L, -1));
            break;
        default:
            printf("%s 0x%lX\n", lua_typename(L, t), (unsigned long)lua_topointer(L, -1 ));
            break;
        }
        lua_pop( L, 1 );
    }

    return;
}
