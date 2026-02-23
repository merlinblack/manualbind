#include <catch2/catch_test_macros.hpp>
#include "LuaRef.h"

void run(lua_State* L, const char* code);

using namespace ManualBind;

TEST_CASE("Can call lua functions with LuaRef")
{
  lua_State* L = luaL_newstate();

  run(L, "function set( nx, ny, nz ) x = nx y = ny z = nz end");

  {
    LuaRef set(L, "set");

    REQUIRE(set.isFunction() == true);

    set("Hello", 10, set);

    LuaRef x(L, "x");
    LuaRef y(L, "y");
    LuaRef z(L, "z");

    REQUIRE(x.isString() == true);
    REQUIRE(y.isNumber() == true);
    REQUIRE(z.isFunction() == true);

  }  // Importaint: LuaRefs going out of scope and destructing, before
     // lua_close();

  lua_close(L);
}

TEST_CASE("LuaRef::call works like LuaRef::operator()")
{
  lua_State* L = luaL_newstate();

  run(L, "function set( nx, ny, nz ) x = nx y = ny z = nz end");

  {
    LuaRef set(L, "set");

    REQUIRE(set.isFunction() == true);

    set.call(0, "Hello", 10, set);

    LuaRef x(L, "x");
    LuaRef y(L, "y");
    LuaRef z(L, "z");

    REQUIRE(x.isString() == true);
    REQUIRE(y.isNumber() == true);
    REQUIRE(z.isFunction() == true);

  }  // Importaint: LuaRefs going out of scope and destructing, before
     // lua_close();

  lua_close(L);
}

TEST_CASE("LuaRef can index and access tables.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef table = LuaRef::newTable(L);
    table.push();
    lua_setglobal(L, "t");

    run(L, "for i = 1, 100 do t[i] = i end");

    REQUIRE((int)table[25] == 25);
    REQUIRE((int)table[1] == 1);
    REQUIRE((int)table[100] == 100);
    REQUIRE(table[1000].isNil() == true);
  }

  lua_close(L);
}

TEST_CASE("Accessing table elements leaves luaTop at the same place.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef table = LuaRef::newTable(L);
    table.push();
    lua_setglobal(L, "t");

    run(L, "t[5] = 123");

    int top = lua_gettop(L);

    int number = table[5];

    int topAfter = lua_gettop(L);

    REQUIRE(number == 123);
    REQUIRE(top == topAfter);
  }
}

static int testfunc(lua_State* L)
{
  lua_pushstring(L, "a string.");
  return 1;
}

TEST_CASE("New table elements have proper types.")
{
  lua_State* L = luaL_newstate();

  {
    lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    LuaRef G = LuaRef::fromStack(L);

    G["mynumber1"] = 12;
    G["mynumber2"] = 12.3;
    G["mystring"] = "Hello";
    G["myfunction"] = testfunc;
    G["mytable"] = LuaRef::newTable(L);

    lua_getglobal(L, "mynumber1");
    REQUIRE(lua_type(L, -1) == LUA_TNUMBER);
    lua_pop(L, 1);

    lua_getglobal(L, "mynumber2");
    REQUIRE(lua_type(L, -1) == LUA_TNUMBER);
    lua_pop(L, 1);

    lua_getglobal(L, "mystring");
    REQUIRE(lua_type(L, -1) == LUA_TSTRING);
    lua_pop(L, 1);

    lua_getglobal(L, "myfunction");
    REQUIRE(lua_type(L, -1) == LUA_TFUNCTION);
    lua_pop(L, 1);

    lua_getglobal(L, "mytable");
    REQUIRE(lua_type(L, -1) == LUA_TTABLE);
    lua_pop(L, 1);
  }

  lua_close(L);
}
