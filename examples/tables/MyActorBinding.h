#ifndef __MYACTORBINDING_H
#define __MYACTORBINDING_H
#include "Binding.h"
#include "MyActor.h"

typedef std::shared_ptr<MyActor> MyActorPtr;

struct MyActorBinding: public Binding<MyActorBinding, MyActor> {

    static constexpr const char* class_name = "MyActor";

    static luaL_Reg* members()
    {
        static luaL_Reg members[] = {
            { NULL, NULL }
        };
        return members;
    }

    static bind_properties* properties() {
        static bind_properties properties[] = {
            { "name", get_name, NULL },
            { NULL, NULL, NULL }
        };
        return properties;
    }

    // Lua constructor
    static int create( lua_State *L )
    {
        std::cout << "Create called\n";

        checkArgCount( L, 1 );

        const char *name = luaL_checkstring( L, 1 );

        MyActorPtr sp = std::make_shared<MyActor>( name );

        push( L, sp );

        return 1;
    }

    // Propertie getters and setters

    static int get_name( lua_State *L )
    {
        checkArgCount( L, 2 );
        MyActorPtr a = fromStack( L, 1 );
        lua_pushstring( L, a->_name.c_str() );
        return 1;
    }

};

#endif // __MYACTORBINDING_H
