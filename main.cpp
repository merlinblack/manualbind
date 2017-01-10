#include <iostream>
#include <lua.hpp>
#include "MyActor.h"
#include "MyActorBinding.h"

static void stackDump (lua_State *L) {
    int i=lua_gettop(L);
    std::cout << " ----------------  Stack Dump ----------------\n";
    while(  i   ) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:
                printf("%d:`%s'\n", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                printf("%d: %s\n",i,lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                printf("%d: %g\n",  i, lua_tonumber(L, i));
                break;
            default: printf("%d: %s\n", i, lua_typename(L, t)); break;
        }
        i--;
    }
    std::cout << "--------------- Stack Dump Finished ---------------\n";
}

void run( lua_State *L, const char *code )
{
    if( luaL_dostring( L, code ) )
    {
        std::cout << lua_tostring( L, -1 ) << std::endl;
    }
}

int main(int argc, char **argv )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    run( L, "print( 'Selam Dünya' ) print( _VERSION )" );

    MyActorBinding::register_class( L );

    run( L, "a = MyActor( 'Bob', 24 )" );
    run( L, "a:walk()" );
    run( L, "a:setName('Robert')" );

    std::cout << "Closing Lua\n";
    lua_close( L );

    return 0;
}
