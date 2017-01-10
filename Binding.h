#ifndef BINDING_H
#define BINDING_H
#include <memory>
#include <lua.hpp>

template<typename B, typename T>
struct Binding {

    static void push( lua_State *L, std::shared_ptr<T> sp )
    {
        void *ud = lua_newuserdata( L, sizeof(std::shared_ptr<T>));

        new(ud) std::shared_ptr<T>( sp );

        luaL_setmetatable( L, B::lua_type );
    }

    static int register_class( lua_State *L )
    {
        std::cout << "Registering class " << B::class_name << std::endl;

        luaL_newmetatable( L, B::lua_type );
        luaL_setfuncs( L, B::members(), 0 );
        lua_pushvalue( L, -1 );
        lua_setfield( L, -2, "__index" );
        lua_pushvalue( L, -1 );
        lua_setfield( L, -2, "__newindex" );
        lua_pushcfunction( L, destroy );
        lua_setfield( L, -2, "__gc" );

        lua_register( L, B::class_name, B::create );
    }


    static int destroy( lua_State *L )
    {
        std::cout << "Destroy called\n";

        // We don't use fromStack as we want to
        // to work on the shared pointer that Lua
        // has, not a copy.
        void* ud = luaL_checkudata( L, 1, B::lua_type );

        std::shared_ptr<T> *sp = (std::shared_ptr<T>*)ud;

        std::cout << "Use count is: " << sp->use_count() << std::endl;

        std::cout << "Resetting shared pointer\n";

        sp->reset();

        return 0;
    }

    // Helpers

    static std::shared_ptr<T> fromStack( lua_State *L, int index )
    {
        std::cout << "fromStack called ";
        void* ud = luaL_checkudata( L, index, B::lua_type );

        std::shared_ptr<T> *psp = (std::shared_ptr<T>*)ud;

        std::cout << "Use count is: " << psp->use_count() << std::endl;

        return *psp;
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

#endif // BINDING_H
