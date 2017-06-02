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
