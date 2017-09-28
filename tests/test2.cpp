#include "catch.hpp"
#include "LuaBinding.h"

using namespace ManualBind;

class Basic
{
};

struct BasicBinding : public Binding<BasicBinding,Basic>
{
    static constexpr const char* class_name = "Basic";

    static int create( lua_State* L )
    {
        return luaL_error( L, "Can not create an instance." );
    }
};

using BasicPtr = std::shared_ptr<Basic>;

TEST_CASE( "Binding maintains shared pointer to same class instance." ) {

    lua_State* L = luaL_newstate();

    BasicBinding::register_class( L );

    BasicPtr bp = std::make_shared<Basic>();

    // Give
    BasicBinding::push( L, bp );
    lua_setglobal( L, "bp" );

    // Retrieve
    lua_getglobal( L, "bp" );
    BasicPtr fromLua = BasicBinding::fromStack( L, 1 );
    lua_pop( L, 1 );

    REQUIRE( bp == fromLua );

    lua_close( L );
}

TEST_CASE( "Garbage collection calls shared pointer destructor." ) {

    lua_State* L = luaL_newstate();

    BasicBinding::register_class( L );

    BasicPtr bp = std::make_shared<Basic>();

    REQUIRE( bp.use_count() == 1 );

    BasicBinding::push( L, bp );
    lua_setglobal( L, "bp" );

    REQUIRE( bp.use_count() == 2 );

    // Garbage collect Lua's pointer.
    lua_pushnil( L );
    lua_setglobal( L, "bp" );
    lua_gc( L, LUA_GCCOLLECT, 0 );

    REQUIRE( bp.use_count() == 1 );

    lua_close( L );
}

TEST_CASE( "Closing Lua state calls shared pointer destructor." ) {

    lua_State* L = luaL_newstate();

    BasicBinding::register_class( L );

    BasicPtr bp = std::make_shared<Basic>();

    REQUIRE( bp.use_count() == 1 );

    BasicBinding::push( L, bp );
    lua_setglobal( L, "bp" );

    REQUIRE( bp.use_count() == 2 );

    lua_close( L );

    REQUIRE( bp.use_count() == 1 );
}
