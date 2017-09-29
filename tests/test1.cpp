#include "catch.hpp"
#include "LuaBinding.h"

using namespace ManualBind;

class Basic
{
};

// A very basic opaque pointer binding.
struct BasicPODbinding : public PODBinding<BasicPODbinding,Basic*>
{
    static constexpr const char* class_name = "Basic";
};

TEST_CASE( "Basic POD binding retains identical pointer value." ) {

    lua_State* L = luaL_newstate();

    BasicPODbinding::register_class( L );

    Basic* bp = new Basic();

    // Give Lua a copy.
    BasicPODbinding::push( L, bp );
    lua_setglobal( L, "bp" );

    // Retrieve
    lua_getglobal( L, "bp" );
    Basic* fromLua = BasicPODbinding::fromStack( L, 1 );
    lua_pop( L, 1 );

    REQUIRE( bp == fromLua );

    lua_close( L );

    delete bp;
}
