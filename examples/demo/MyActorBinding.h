#ifndef __MYACTORBINDING_H
#define __MYACTORBINDING_H
#include "LuaBinding.h"
#include "MyActor.h"

using MyActorPtr = std::shared_ptr<MyActor>;

using namespace ManualBind;

struct MyActorBinding: public Binding<MyActorBinding, MyActor> {

    static constexpr const char* class_name = "MyActor";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { "walk", walk },
            { "setName", setName },
            { nullptr, nullptr }
        };
        return members;
    }

    static bind_properties* properties() {
        static bind_properties properties[] = {
            { "age", get_age, set_age },
            { nullptr, nullptr, nullptr }
        };
        return properties;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        CheckArgCount( L, 2 );

        const char *name = luaL_checkstring( L, 1 );
        int age = luaL_checkinteger( L, 2 );

        MyActorPtr sp = std::make_shared<MyActor>( name, age );

        push( L, sp );

        return 1;
    }

    // Method glue functions
    //

    static int walk( lua_State *L )
    {
        CheckArgCount( L, 1 );

        MyActorPtr a = fromStack( L, 1 );

        a->walk();

        return 0;
    }

    static int setName( lua_State *L )
    {
        CheckArgCount( L, 2 );

        MyActorPtr a = fromStack( L, 1 );
        const char *name = lua_tostring( L, 2 );

        a->setName( name );

        return 0;
    }

    // Propertie getters and setters

    // 1 - class metatable
    // 2 - key
    static int get_age( lua_State *L )
    {
        CheckArgCount( L, 2 );

        MyActorPtr a = fromStack( L, 1 );

        lua_pushinteger( L, a->_age );

        return 1;
    }

    // 1 - class metatable
    // 2 - key
    // 3 - value
    static int set_age( lua_State *L )
    {
        CheckArgCount( L, 3 );

        MyActorPtr a = fromStack( L, 1 );

        int age = luaL_checkinteger( L, 3 );

        a->_age = age;

        return 0;
    }

};

#endif // __MYACTORBINDING_H
