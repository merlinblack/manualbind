#include <catch2/catch_test_macros.hpp>
#include "LuaBinding.h"

void run( lua_State* L, const char* code );

using namespace ManualBind;

class Basic
{
};

struct BasicBinding : public Binding<BasicBinding,Basic>
{
    static constexpr const char* class_name = "Basic";
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
    BasicPtr fromLua = BasicBinding::fromStackThrow( L, 1 );
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

TEST_CASE( "Lua object going out of scope with close annotation resets shared pointer." ) {

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    BasicBinding::register_class( L );

    run(L, "function scope(bp) local lbp <close> = bp coroutine.yield() end");
    run(L, "co = coroutine.wrap(scope)");

    BasicPtr bp = std::make_shared<Basic>();

    REQUIRE( bp.use_count() == 1 );

    int nResults, ret;
    lua_getglobal( L, "co" );
    BasicBinding::push( L, bp );
    ret = lua_resume( L, nullptr, 1, &nResults);

    REQUIRE( bp.use_count() == 2 );

    lua_getglobal( L, "co" );
    ret = lua_resume( L, nullptr, 0, &nResults);

    REQUIRE( bp.use_count() == 1 );

    lua_close( L );
}
