#include <iostream>
#include <vector>
#include "MyActor.h"
#include "MyActorBinding.h"
#include "common.h"

using std::cout;
using std::endl;
using std::make_shared;

using MyActorList = std::vector<MyActorPtr>;

MyActorList createList()
{
  MyActorList actors{
      make_shared<MyActor>("James"), make_shared<MyActor>("Who? Random extra"),
      make_shared<MyActor>("Harry"), make_shared<MyActor>("Mike")};

  return actors;
}

void pushToLua(lua_State* L, MyActorList list)
{
  lua_newtable(L);
  int i = 1;

  for (const auto& actor : list) {
    MyActorBinding::push(L, actor);
    lua_rawseti(L, -2, i++);
  }
}

MyActorList pullFromLua(lua_State* L)
{
  // Note this only stores the values, not the keys/indexes.
  // The keys are available at index -2 after calling lua_next,
  // but if you need it, copy it first using lua_pushvalue and use that
  // so that the original is left intact for the next call to lua_next.
  //
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

int main()
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  MyActorBinding::register_class(L);

  {
    cout << "Pushing actor list to Lua." << endl;
    MyActorList actors = createList();
    pushToLua(L, actors);
    lua_setglobal(L, "actors");
  }

  run(L, "for k,v in pairs(actors) do print( k, v.name ) end");

  cout << "Editing actor list..." << endl;
  run(L, "actors[2] = nil                    -- Who is this? Pfft, delete.");
  run(L, "collectgarbage()");
  run(L, "actors['test'] = MyActor('Bob')    -- Hey Bob welcome.");

  run(L, "for k,v in pairs(actors) do print( k, v.name ) end");

  {
    cout << "Pull list back into C++ vector, and list..." << endl;
    lua_getglobal(L, "actors");
    MyActorList actors = pullFromLua(L);
    lua_pop(L, 1);

    for (const auto& actor : actors) {
      cout << actor->_name << endl;
    }
  }

  lua_close(L);
}
