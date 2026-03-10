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
     // lua_close(); (Unless you assign them all to Nil)

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
     // lua_close(); (Unless you assign them all to Nil)

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

int testfunc(lua_State* L)
{
  return 0;
}

TEST_CASE("Adding table elements have proper types.")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);

    // Integer
    G["mynumber1"] = 12;
    // Float
    G["mynumber2"] = 12.3;
    // c string
    G["mystring"] = "Hello";
    // Plain C function
    G["myfunction"] = testfunc;
    // Lambda with captures
    G["myfunction2"] = (CPP_Function)[&](lua_State * L)->int
    {
      return 0;
    };
    // Lambda without captures - potentially a tiny bit faster than with
    // captures. Less usefull however.
    G["myfunction3"] = (lua_CFunction)[](lua_State * L)->int
    {
      return 0;
    };
    // A table
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

    lua_getglobal(L, "myfunction2");
    REQUIRE(lua_type(L, -1) == LUA_TFUNCTION);
    lua_pop(L, 1);

    lua_getglobal(L, "myfunction3");
    REQUIRE(lua_type(L, -1) == LUA_TFUNCTION);
    lua_pop(L, 1);

    lua_getglobal(L, "mytable");
    REQUIRE(lua_type(L, -1) == LUA_TTABLE);
    lua_pop(L, 1);
  }

  lua_close(L);
}

TEST_CASE("Lua can call C++ lambda with captures")
{
  lua_State* L = luaL_newstate();

  {
    LuaRef G = LuaRef::globalTable(L);

    int x = 42;

    G["lambda"] = (CPP_Function)[&x](lua_State * L)->int
    {
      x *= 2;
      return 0;
    };

    REQUIRE(x == 42);

    run(L, "lambda()");

    REQUIRE(x == 84);
  }

  lua_close(L);
}

TEST_CASE("You can destuct a LuaRef after lua_close, if it holds LUA_REFNIL")
{
  lua_State* L = luaL_newstate();

  LuaRef G = LuaRef::globalTable(L);

  // This also unreferences whatever the LuaRef was previously referencing. You
  // might want to do this to allow an object to be garbage collected, while
  // keeping the LuaRef around. The global table being a bad example of this of
  // course :wink:
  G = LuaRef::nil(L);

  // Note that this relies on the fact that a ref of LUA_REFNIL, when used with
  // luaL_unref, (called in LuaRef's destructor) does literally nothing other
  // than return.

  lua_close(L);
}
