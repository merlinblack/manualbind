#include <iostream>
#include <lua.hpp>
#include <unordered_map>
#include <vector>
#include "MyActor.h"
#include "MyActorBinding.h"
#include "common.h"

using std::cout;
using std::make_shared;

using MyActorList = std::vector<MyActorPtr>;

MyActorList createActorList()
{
  MyActorList actors{
      make_shared<MyActor>("James"), make_shared<MyActor>("Who? Random extra"),
      make_shared<MyActor>("Harry"), make_shared<MyActor>("Mike")};

  return actors;
}

void pushActorsToLua(lua_State* L, MyActorList list)
{
  lua_newtable(L);
  int i = 1;

  for (const auto& actor : list) {
    MyActorBinding::push(L, actor);
    lua_rawseti(L, -2, i++);
  }
}

MyActorList pullActorsFromLua(lua_State* L)
{
  // Note this only stores the values, not the keys/indexes.
  // Also this simply skips over any elements that are not 'MyActor's.
  MyActorList list;

  if (lua_istable(L, -1)) {
    lua_pushnil(L);

    while (lua_next(L, -2)) {
      if (MyActorBinding::isType(L, -1)) {
        list.push_back(MyActorBinding::fromStack(L, -1));
      }
      lua_pop(L, 1);
    }
  }

  return list;
}

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

  MyActorBinding::register_class(L);

  // Lua table and a vector of class instances.
  // ------------------------------------------
  {
    cout << "Pushing actor list to Lua.\n";
    MyActorList actors = createActorList();
    pushActorsToLua(L, actors);
    lua_setglobal(L, "actors");
  }

  run(L, "for k,v in pairs(actors) do print( k, v.name ) end");

  cout << "Editing actor list..." << "\n";
  run(L, "actors[2] = nil                    -- Who is this? Pfft, delete.");
  run(L, "collectgarbage()");
  run(L, "actors['test'] = MyActor('Bob')    -- Hey Bob welcome.");

  run(L, "for k, v in pairs(actors) do print(k, v.name) end");

  {
    cout << "Pull list back into C++ vector, and list...\n";
    lua_getglobal(L, "actors");
    MyActorList actors = pullActorsFromLua(L);
    lua_pop(L, 1);

    for (const auto& actor : actors) {
      cout << actor->_name << "\n";
    }
  }

  // Lua table of strings with keys and a C++ map.
  // ---------------------------------------------
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
