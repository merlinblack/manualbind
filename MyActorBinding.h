#ifndef __MYACTORBINDING_H
#define __MYACTORBINDING_H
#include "Binding.h"
#include "MyActor.h"

typedef std::shared_ptr<MyActor> MyActorPtr;

struct MyActorBinding: public Binding<MyActorBinding, MyActor> {

    static constexpr const char* lua_type   = "MyActorType";
    static constexpr const char* class_name = "MyActor";
    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "walk", walk },
            { "setName", setName },
            { NULL, NULL }
        };
        return members;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        checkArgCount( L, 2 );

        const char *name = luaL_checkstring( L, 1 );
        int age = luaL_checkint( L, 2 );

        MyActorPtr sp = std::make_shared<MyActor>( name, age );

        push( L, sp );

        return 1;
    }

    // Method glue functions
    //

    static int walk( lua_State *L )
    {
        checkArgCount( L, 1 );

        MyActorPtr a = fromStack( L, 1 );

        a->walk();

        return 0;
    }

    static int setName( lua_State *L )
    {
        checkArgCount( L, 2 );

        MyActorPtr a = fromStack( L, 1 );
        const char *name = lua_tostring( L, 2 );

        a->setName( name );

        return 0;
    }

};

#endif // __MYACTORBINDING_H
