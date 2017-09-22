#ifndef __MYACTORBINDING_H
#define __MYACTORBINDING_H
#include "LuaBinding.h"
#include "MyActor.h"

using MyActorPtr = std::shared_ptr<MyActor>;

using namespace ManualBind;

struct MyActorBinding: public Binding<MyActorBinding, MyActor> {

    static constexpr const char* class_name = "MyActor";

    static bind_properties* properties()
    {
        static bind_properties properties[] = {
            { "name", get_name, nullptr },
            { nullptr, nullptr, nullptr }
        };
        return properties;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        CheckArgCount( L, 1 );

        const char *name = luaL_checkstring( L, 1 );

        MyActorPtr sp = std::make_shared<MyActor>( name );

        push( L, sp );

        return 1;
    }

    // Propertie getters and setters

    static int get_name( lua_State *L )
    {
        CheckArgCount( L, 2 );
        MyActorPtr a = fromStack( L, 1 );
        lua_pushstring( L, a->_name.c_str() );
        return 1;
    }

};

#endif // __MYACTORBINDING_H
