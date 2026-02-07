#include "LuaBinding.h"
#include "lauxlib.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

#include <iostream>

using namespace ManualBind;

class Basic {};

// A very basic opaque pointer binding.
struct BasicPODbinding : public PODBinding<BasicPODbinding, Basic *> {
  static constexpr const char *class_name = "Basic";
};

TEST_CASE("Basic POD binding retains identical pointer value.") {

  lua_State *L = luaL_newstate();

  BasicPODbinding::register_class(L);

  Basic *bp = new Basic();

  // Give Lua a copy.
  BasicPODbinding::push(L, bp);
  lua_setglobal(L, "bp");

  // Retrieve
  lua_getglobal(L, "bp");
  Basic *fromLua = BasicPODbinding::fromStackThrow(L, 1);
  lua_pop(L, 1);

  REQUIRE(bp == fromLua);

  lua_close(L);

  delete bp;
}

TEST_CASE("Basic POD binding can test for type.") {

  lua_State *L = luaL_newstate();

  BasicPODbinding::register_class(L);

  Basic *bp = new Basic();

  // Give Lua a copy.
  BasicPODbinding::push(L, bp);
  lua_setglobal(L, "bp");

  // Check the 'type'
  lua_getglobal(L, "bp");
  REQUIRE(BasicPODbinding::isType(L, -1) == true);
  lua_pop(L, 1);

  // Check for false positive
  lua_pushinteger(L, 42);
  REQUIRE(BasicPODbinding::isType(L, -1) == false);
  lua_pop(L, 1);

  lua_close(L);

  delete bp;
}

TEST_CASE(
    "Lua calling create on binding without create function gives lua error.") {

  lua_State *L = luaL_newstate();

  BasicPODbinding::register_class(L);

  luaL_dostring(L, "b = Basic()");

  std::string error(lua_tostring(L, -1));

  lua_close(L);

  REQUIRE(error ==
          "[string \"b = Basic()\"]:1: Can not create an instance of Basic.");
}
