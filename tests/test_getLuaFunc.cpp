#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "LuaRef.h"
#include "LuaStack.h"

void run(lua_State* L, const char* code);

using namespace ManualBind;

TEST_CASE(
    "getLuaFunc - Can get and call Lua function, no parameters, no return "
    "value.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    int test;

    run(L, "function simple() x = 42 end");
    // defaults to <void>
    auto simple = getLuaFunc(L, "simple");

    G["x"] = 5;

    simple();

    test = G["x"];
    REQUIRE(test == 42);
  }

  lua_close(L);
}

TEST_CASE(
    "getLuaFunc - Can get and call Lua function, single parameter, no return "
    "value.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    int test;

    run(L, "function single(n) x = n end");
    auto single = getLuaFunc<void, int>(L, "single");

    G["x"] = 5;

    single(42);

    test = G["x"];
    REQUIRE(test == 42);
  }

  lua_close(L);
}

TEST_CASE(
    "getLuaFunc - Can get and call Lua function, no parameters, with return "
    "value.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    int test;

    run(L, "function returner() return x end");
    auto returner = getLuaFunc<int>(L, "returner");

    G["x"] = 42;

    test = returner();

    REQUIRE(test == 42);
  }

  lua_close(L);
}

TEST_CASE(
    "getLuaFunc - Can get and call Lua function, one parameter, with return "
    "value.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    int test;

    run(L, "function returner(x) return x end");
    auto returner = getLuaFunc<int, int>(L, "returner");

    test = returner(42);

    REQUIRE(test == 42);
  }

  lua_close(L);
}

TEST_CASE(
    "getLuaFunc - Can get and call Lua function, several parameters, with "
    "return value.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    int test;

    run(L, "function add(a, b, c) return a + b + c end");
    auto add = getLuaFunc<int, int, int, int>(L, "add");

    test = add(10, 2, 30);

    REQUIRE(test == 42);
  }

  lua_close(L);
}

TEST_CASE("getLuaFunc - Can get and call Lua function, more complex types.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);
    std::string test;

    run(L, "function concat(a, b, c) return a .. b .. c end");
    auto concat =
        getLuaFunc<std::string, std::string, std::string, std::string>(
            L, "concat");

    test = concat("Hello", " ", "World!");

    REQUIRE(test == "Hello World!");
  }

  lua_close(L);
}

TEST_CASE("getLuaFunc - runtime exception on not found.")
{
  lua_State* L = luaL_newstate();

  {
    REQUIRE_THROWS_AS(getLuaFunc(L, "notfound"), std::runtime_error);
  }

  lua_close(L);
}

TEST_CASE("getLuaFunc - runtime exception on Lua error.")
{
  lua_State* L = luaL_newstate();

  {
    run(L, "function doesError() undefined() end");

    auto doesError = getLuaFunc(L, "doesError");

    REQUIRE_THROWS_AS(doesError(), std::runtime_error);
  }

  lua_close(L);
}
