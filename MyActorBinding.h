#ifndef __MYACTORBINDING_H
#define __MYACTORBINDING_H
#include <memory>
#include <lua.hpp>
#include "MyActor.h"

typedef std::shared_ptr<MyActor> MyActorPtr;

void lua_pushMyActorPtr( lua_State *L, MyActorPtr sp );

// Using struct basically as a namespace.

struct MyActorBinding {

    // Method glue functions
    //

    static int walk( lua_State *L )
    {
        checkArgCount( L, 1 );

        MyActorPtr a = getSelf( L, 1 );

        a->walk();

        return 0;
    }

    static int setName( lua_State *L )
    {
        checkArgCount( L, 2 );

        MyActorPtr a = getSelf( L, 1 );
        const char *name = lua_tostring( L, 2 );

        a->setName( name );

        return 0;
    }


    // Class maintenance
    //

    static int register_class( lua_State *L )
    {
        std::cout << "Registering class\n";

        static const luaL_Reg members[] = {
            { "walk", walk },
            { "setName", setName },
            { NULL, NULL }
        };

        luaL_newmetatable( L, "MyActorType" );
        luaL_setfuncs( L, members, 0 );
        lua_pushvalue( L, -1 );
        lua_setfield( L, -2, "__index" );
        lua_pushvalue( L, -1 );
        lua_setfield( L, -2, "__newindex" );
        lua_pushcfunction( L, destroy );
        lua_setfield( L, -2, "__gc" );

        lua_register( L, "MyActor", create );
    }

    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        checkArgCount( L, 2 );

        const char *name = luaL_checkstring( L, 1 );
        int age = luaL_checkint( L, 2 );

        MyActorPtr sp = std::make_shared<MyActor>( name, age );

        lua_pushMyActorPtr( L, sp );

        return 1;
    }

    static int destroy( lua_State *L )
    {
        std::cout << "Destroy called\n";

        void* ud = 0;
        ud = luaL_checkudata( L, 1, "MyActorType" );
        luaL_argcheck( L, ud != 0, 1, "MyActor expected." );

        MyActorPtr *sp = (MyActorPtr*)ud;

        std::cout << "Use count is: " << sp->use_count() << std::endl;

        std::cout << "Resetting shared pointer\n";

        sp->reset();

        return 0;
    }

    // Helpers

    static MyActorPtr getSelf( lua_State *L, int index )
    {
        std::cout << "getSelf called\n";
        void* ud = 0;
        ud = luaL_checkudata( L, index, "MyActorType" );
        luaL_argcheck( L, ud != 0, index, "MyActor expected." );

        MyActorPtr sp = *((MyActorPtr*)ud);

        std::cout << "Use count is: " << sp.use_count() << std::endl;

        return sp;
    }

    static void checkArgCount( lua_State *L, int expected )
    {
        int n = lua_gettop(L);
        if( n != expected ) {
            luaL_error( L, "Got %d arguments, expected %d", n, expected );
            return;
        }
        return;
    }
};

// Push and pop
//
    
MyActorPtr lua_toMyActorPtr( lua_State *L, int index )
{
    MyActorPtr sp = MyActorBinding::getSelf( L, index );
    return sp;
}

void lua_pushMyActorPtr( lua_State *L, MyActorPtr sp )
{
    void *ud = lua_newuserdata( L, sizeof(MyActorPtr));

    new(ud) MyActorPtr( sp );

    luaL_setmetatable( L, "MyActorType" );
}

#endif // __MYACTORBINDING_H
