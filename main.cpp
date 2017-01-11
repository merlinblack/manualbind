#include <iostream>
#include <lua.hpp>
#include "MyActor.h"
#include "MyActorBinding.h"

using std::cout;
using std::endl;

static void stackDump (lua_State *L) {
    int i=lua_gettop(L);
    cout << " ----------------  Stack Dump ----------------\n";
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
            default: 
                printf("%d: %s\n", i, lua_typename(L, t)); 
                break;
        }
        i--;
    }
    cout << "--------------- Stack Dump Finished ---------------\n";
}

void run( lua_State *L, const char *code )
{
    cout << "code> " << code << endl;

    if( luaL_dostring( L, code ) )
    {
        cout << lua_tostring( L, -1 ) << endl;
    }
}

int main(int argc, char **argv )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    run( L, "print( 'Selam Dünya' ) print( _VERSION )" );

    MyActorBinding::register_class( L );

    run( L, "a = MyActor( 'Alara', 27 )" );
    run( L, "a:walk()" );
    run( L, "b = a" );
    run( L, "a:setName('Awesome Chick')" );

    {
        lua_getglobal( L, "b" );
        MyActorPtr b = MyActorBinding::fromStack( L, -1 );
        lua_pop( L, 1 );

        cout << "Use count is now: " << b.use_count() << endl;
        b->walk();
    }

    {
        MyActorPtr actor = std::make_shared<MyActor>("Nigel",39);
        cout << "Actor use count is: " << actor.use_count() << endl;
        MyActorBinding::push( L, actor );
        lua_setglobal( L, "actor" );
        cout << "Pushed to Lua" << endl;
        cout << "Actor use count is: " << actor.use_count() << endl;
        run( L, "actor:walk()" );
        run( L, "actor.age = actor.age + 1 print( 'Happy Birthday')" );
        run( L, "print( actor.age )" );
        cout << actor->_age << endl;
    }

    // Override (for all instances) a method, while calling the old implementation
    run( L, "local old = actor.walk actor.walk = function(self) old(self) print( 'RUN!' ) end" );
    run( L, "b:walk()" );

    run( L, "a:splat()" );

    cout << "Closing Lua\n";
    lua_close( L );

    return 0;
}
