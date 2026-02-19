#include <iostream>
#include <lua.hpp>
#include <vector>
#include "LuaRef.h"
#include "LuaStack.h"
#include "Person.h"
#include "PersonBinding.h"
#include "common.h"

//
// Lua table and a vector of class instances.
// Using LuaRef
// ------------------------------------------

using ManualBind::LuaRef;
using std::cout;
using std::make_shared;

using PersonList = std::vector<PersonPtr>;

// Teach LuaStack how to push People - so that LuaRef::append can push them
// too.
template <>
struct ManualBind::LuaStack<PersonPtr> {
  static void push(lua_State* L, PersonPtr v) { PersonBinding::push(L, v); }
};

PersonList createListOfPeople()
{
  PersonList people{make_shared<Person>("James"),
                    make_shared<Person>("Who? Random extra"),
                    make_shared<Person>("Harry"), make_shared<Person>("Mike")};

  return people;
}

void pushPeopleToLua(lua_State* L, PersonList list)
{
  LuaRef table = LuaRef::newTable(L);

  for (const auto& person : list) {
    table.append(person);
  }

  table.push();
}

PersonList pullPeopleFromLua(lua_State* L)
{
  // Note this only stores the values, not the keys/indexes.
  // Also this simply skips over any elements that are not 'Person's.
  PersonList list;

  if (lua_istable(L, -1)) {
    lua_pushnil(L);

    while (lua_next(L, -2)) {
      if (PersonBinding::isType(L, -1)) {
        list.push_back(PersonBinding::fromStack(L, -1));
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

  PersonBinding::register_class(L);

  {
    cout << "Pushing list of people to Lua.\n";
    PersonList people = createListOfPeople();
    pushPeopleToLua(L, people);
    lua_setglobal(L, "people");
  }

  run(L, "for k,v in pairs(people) do print( k, v.name ) end");

  cout << "Editing list..." << "\n";
  run(L, "people[2] = nil                    -- Who is this? Pfft, delete.");
  run(L, "collectgarbage()");
  run(L, "people['test'] = Person('Bob')    -- Hey Bob welcome.");

  run(L, "for k, v in pairs(people) do print(k, v.name) end");

  {
    cout << "Pull list back into C++ vector, and list...\n";
    lua_getglobal(L, "people");
    PersonList people = pullPeopleFromLua(L);
    lua_pop(L, 1);

    for (const auto& person : people) {
      cout << person->_name << "\n";
    }
  }

  lua_close(L);
}
