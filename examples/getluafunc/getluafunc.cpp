#include <iostream>
#include <string>
#include "LuaRef.h"
#include "LuaStack.h"
#include "lua.hpp"

using ManualBind::getLuaFunc;
using ManualBind::LuaRef;
using std::string;

int main(int argc, char* argv[])
{
  const char* script = R"(
    function add(a, b)
      return a + b
    end

    function concat( a, b, c )
      return a .. b .. c
    end

    function nothing()
      print('No args, no return. Bit boring really.')
    end
  )";

  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_dostring(L, script) != LUA_OK) {
    std::cerr << lua_tostring(L, -1);
    return EXIT_FAILURE;
  }

  // std::function<int(int, int)>
  auto add = getLuaFunc<int, int, int>(L, "add");

  // std::function<float(float, float)>, but the same Lua function as 'add'.
  auto addF = getLuaFunc<float, float, float>(L, "add");

  // std::function<string(string, string, string)>
  auto concat = getLuaFunc<string, string, string, string>(L, "concat");

  // The first template parameter, the return type, defaults to 'void'.
  // The lack of any template parameters after that gives an empty arg list.
  // std::function<void()>
  auto boring = getLuaFunc(L, "nothing");

  // Any parameters you like (that LuaStack can push). There is no type checking.
  // LuaRef
  auto print = LuaRef::getGlobal(L, "print");

  int x = add(5, 10);
  float y = addF(41.5, 0.5);

  // Works but first parameter is implicitly narrowed to an int.
  int z = add(123.2, 10);

  // int nope = add("Hello", "World"); - does not compile, no matching call.

  string result = concat("Hello", " ", "world!");

  boring();

  print(result, x, y, z);

  return EXIT_SUCCESS;
}
