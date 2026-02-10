#include <catch2/catch_test_macros.hpp>
#include <lua.hpp>
#include <sstream>
#include "LuaBinding.h"
#include "lauxlib.h"

void run(lua_State* L, const char* code);

using namespace ManualBind;

struct Vector {
  double x;
  double y;
  double z;
};

struct VectorPODbinding : public PODBinding<VectorPODbinding, Vector> {
  static constexpr const char* class_name = "Vector";

  static luaL_Reg* members()
  {
    static luaL_Reg members[] = {{"__tostring", to_string}, {nullptr, nullptr}};

    return members;
  }

  static int to_string(lua_State* L)
  {
    Vector v = fromStack(L, 1);
    std::stringstream ss;

    ss << v.x << ", " << v.y << ", " << v.z;

    lua_pushstring(L, ss.str().c_str());

    return 1;
  }

  static bind_properties* properties()
  {
    static bind_properties properties[] = {
        {"x", getx, set},
        {"y", gety, set},
        {"z", getz, set},
    };
    return properties;
  }

  static int getx(lua_State* L)
  {
    Vector& v = fromStack(L, 1);
    lua_pushnumber(L, v.x);
    return 1;
  }

  static int gety(lua_State* L)
  {
    Vector& v = fromStack(L, 1);
    lua_pushnumber(L, v.y);
    return 1;
  }

  static int getz(lua_State* L)
  {
    Vector& v = fromStack(L, 1);
    lua_pushnumber(L, v.z);
    return 1;
  }

  static int set(lua_State* L)
  {
    Vector& v = fromStack(L, 1);
    const char* key = lua_tostring(L, 2);
    double value = lua_tonumber(L, 3);

    switch (key[0]) {
      case 'x':
        v.x = value;
        break;
      case 'y':
        v.y = value;
        break;
      case 'z':
        v.z = value;
        break;
    }

    return 0;
  }

  static int create(lua_State* L)
  {
    double x = lua_tonumber(L, 1);
    double y = lua_tonumber(L, 2);
    double z = lua_tonumber(L, 3);

    Vector v = {x, y, z};

    push(L, v);

    return 1;
  }
};

TEST_CASE("Vector POD, can be created from Lua.")
{
  lua_State* L = luaL_newstate();

  VectorPODbinding::register_class(L);

  run(L, "v = Vector(4,5,6)");

  // Retrieve
  lua_getglobal(L, "v");
  Vector fromLua = VectorPODbinding::fromStackThrow(L, 1);
  lua_pop(L, 1);

  REQUIRE(fromLua.x == 4);
  REQUIRE(fromLua.y == 5);
  REQUIRE(fromLua.z == 6);

  lua_close(L);
}

TEST_CASE("Vector POD, can be updated from Lua.")
{
  lua_State* L = luaL_newstate();

  VectorPODbinding::register_class(L);

  Vector v = {1, 2, 3};

  // Note Lua gets a copy of Vector v
  VectorPODbinding::push(L, v);
  lua_setglobal(L, "vlua");

  run(L, "vlua.y = 5");

  // Retrieve Lua's modified copy.
  lua_getglobal(L, "vlua");
  Vector fromLua = VectorPODbinding::fromStackThrow(L, 1);
  lua_pop(L, 1);

  REQUIRE(fromLua.y == 5);

  lua_close(L);
}

TEST_CASE("Vector POD, Lua can call member function (__tostring).")
{
  lua_State* L = luaL_newstate();

  luaL_openlibs(L);

  VectorPODbinding::register_class(L);

  Vector v = {1, 2, 3};

  // Note Lua gets a copy of Vector v
  VectorPODbinding::push(L, v);
  lua_setglobal(L, "vlua");

  run(L, "str = tostring(vlua)");

  lua_getglobal(L, "str");
  std::string str = lua_tostring(L, 1);

  REQUIRE(str == "1, 2, 3");

  lua_close(L);
}
