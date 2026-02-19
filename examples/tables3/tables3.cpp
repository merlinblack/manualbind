#include <iostream>
#include <lua.hpp>
#include <unordered_map>
#include "common.h"

// Lua table of strings with keys and a C++ map.
// ---------------------------------------------
//

using std::cout;

using StrStrMap = std::unordered_map<std::string, std::string>;

const StrStrMap& createMap()
{
  static StrStrMap map = {{"One", "Uno"},
                          {"Two", "Dos"},
                          {"Three", "Tres"},
                          {"Four", "Quatro"},
                          {"Five", "Cinco"}};
  return map;
}

void pushMap(lua_State* L, const StrStrMap& map)
{
  lua_newtable(L);
  for (const auto& [key, item] : map) {
    lua_pushstring(L, item.c_str());
    lua_setfield(L, -2, key.c_str());
  }
}

StrStrMap pullMap(lua_State* L)
{
  StrStrMap ret;

  if (lua_istable(L, -1)) {
    lua_pushnil(L);
    while (lua_next(L, -2)) {
      // The key is available at index -2 after calling lua_next.
      // You need to avoid modifing it for the next call to lua_next
      // to work correctly. This includes avoiding calling lua_to[l]string
      // when the key is not already a string.
      // We're just going to check the key is a string and skip if it is not.
      if (lua_isstring(L, -2)) {
        std::string key = lua_tostring(L, -2);
        std::string value = lua_tostring(L, -1);

        ret.emplace(key, value);
      }

      // Pop the value, leaving the key at the top of the stack for lua_next.
      lua_pop(L, 1);
    }
  }

  return ret;
}

int main()
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  StrStrMap map = createMap();
  pushMap(L, map);
  lua_setglobal(L, "map");

  run(L, "for k, v in pairs(map) do print('Lua:', k, v) end");

  lua_getglobal(L, "map");
  StrStrMap ret = pullMap(L);

  for (const auto& [key, value] : ret) {
    cout << "C++:\t" << key << "\t" << value << "\n";
  }

  lua_close(L);
}
