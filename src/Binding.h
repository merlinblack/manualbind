#ifndef BINDING_H
#define BINDING_H
#include <memory>
#include <lua.hpp>

struct bind_properties {
    const char *name;
    lua_CFunction getter;
    lua_CFunction setter;
};

// B - the binding class / struct
// T - the class you are binding to Lua.
//
template<typename B, typename T>
struct Binding {

    static void push( lua_State *L, std::shared_ptr<T> sp )
    {
        void *ud = lua_newuserdata( L, sizeof(std::shared_ptr<T>));

        new(ud) std::shared_ptr<T>( sp );

        luaL_setmetatable( L, B::class_name );
    }

    static void register_class( lua_State *L )
    {
        luaL_newmetatable( L, B::class_name );
        luaL_setfuncs( L, B::members(), 0 );
        lua_pushcfunction( L, index );
        lua_setfield( L, -2, "__index" );
        lua_pushcfunction( L, newindex );
        lua_setfield( L, -2, "__newindex" );
        lua_pushcfunction( L, destroy );
        lua_setfield( L, -2, "__gc" );
        lua_newtable( L ); // __properties
        set_properties( L, B::properties() );
        lua_setfield( L, -2, "__properties" );
        lua_pop( L, 1 );

        lua_register( L, B::class_name, B::create );
    }

    static void set_properties( lua_State *L, bind_properties* properties )
    {
        // Assumes table at top of the stack for the properties.
        while( properties->name != NULL ) {

            lua_newtable( L );

            lua_pushcfunction( L, properties->getter );
            lua_setfield( L, -2, "get" );
            if( properties->setter ) {
                lua_pushcfunction( L, properties->setter );
                lua_setfield( L, -2, "set" );
            }
            lua_setfield( L, -2, properties->name );

            properties++;
        }
    }

    static int index( lua_State *L )
    {
        // 1 - class user data
        // 2 - key
        lua_getmetatable( L, 1 );
        // 3 - class metatable
        if( lua_isnumber( L, 2 ) ) { // Array access
            lua_pushliteral( L, "__arrayindex" );
            lua_gettable( L, 3 );
            if( lua_type( L, 4 ) == LUA_TFUNCTION ) {
                lua_pushvalue( L, 1 );
                lua_pushvalue( L, 2 );
                lua_call( L, 2, 1 );
            } else {
                luaL_error( L, "Attempt to index without __arrayindex" );
            }
            return 1;
        }
        lua_pushvalue( L, 2 ); // Key
        lua_gettable( L, 3 );
        if( lua_type( L, 4 ) != LUA_TNIL ) { // Found in metatable.
            return 1;
        }
        lua_pop( L, 1 );
        lua_pushliteral( L, "__properties" );
        lua_gettable( L, 3 );
        // 4 - class properties table
        lua_pushvalue( L, 2 );
        lua_gettable( L, 4 );
        // 5 - property table for key ( or nil )
        if( lua_type( L, 5 ) == LUA_TTABLE ) { // Found in properties.
            lua_pushliteral( L, "get" );
            lua_gettable( L, 5 );
            // Call get function.
            if( lua_type( L, 6 ) == LUA_TFUNCTION ) {
                lua_pushvalue( L, 1 );
                lua_pushvalue( L, 2 );
                lua_call( L, 2, 1 );
            }
            return 1;
        }

        lua_pushnil( L );
        return 1; // Not found, return nil.
    }

    static int newindex( lua_State *L )
    {
        // 1 - class user data
        // 2 - key
        // 3 - value
        lua_getmetatable( L, 1 );
        if( lua_isnumber( L, 2 ) ) { // Array access
            lua_pushliteral( L, "__arraynewindex" );
            lua_gettable( L, 4 );
            if( lua_type( L, 5 ) == LUA_TFUNCTION ) {
                lua_pushvalue( L, 1 );
                lua_pushvalue( L, 2 );
                lua_pushvalue( L, 3 );
                lua_call( L, 3, 0 );
            } else {
                luaL_error( L, "Attempt to assign to index without __arraynewindex" );
            }
            return 0;
        }
        // 4 - class metatable
        lua_pushliteral( L, "__properties" );
        lua_gettable( L, 4 );
        // 5 - class properties table
        lua_pushvalue( L, 2 );
        lua_gettable( L, 5 );
        // 6 - property table for key ( or nil )
        if( lua_type( L, 6 ) == LUA_TTABLE ) { // Found in properties.
            lua_pushliteral( L, "set" );
            lua_gettable( L, 6 );
            if( lua_type( L, 7 ) == LUA_TFUNCTION ) {
                // Call set function.
                lua_pushvalue( L, 1 );
                lua_pushvalue( L, 2 );
                lua_pushvalue( L, 3 );
                lua_call( L, 3, 0 );
            }
            return 0;
        }

        // set in class metatable
        lua_pushvalue( L, 2 );
        lua_pushvalue( L, 3 );
        lua_rawset( L, 4 );
        return 0;
    }

    static int destroy( lua_State *L )
    {
        // We don't use fromStack as we want to
        // to work on the shared pointer that Lua
        // has, not a copy.
        void* ud = luaL_checkudata( L, 1, B::class_name );

        std::shared_ptr<T> *sp = (std::shared_ptr<T>*)ud;

        sp->~shared_ptr();

        return 0;
    }

    // Helpers

    static std::shared_ptr<T> fromStack( lua_State *L, int index )
    {
        void* ud = luaL_checkudata( L, index, B::class_name );

        return *((std::shared_ptr<T>*)ud);
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
