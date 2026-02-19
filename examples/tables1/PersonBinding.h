#ifndef __PERSONBINDING_H
#define __PERSONBINDING_H
#include "LuaBinding.h"
#include "Person.h"

using PersonPtr = std::shared_ptr<Person>;

using namespace ManualBind;

struct PersonBinding : public Binding<PersonBinding, Person> {
  static constexpr const char* class_name = "Person";

  static bind_properties* properties()
  {
    static bind_properties properties[] = {{"name", get_name, nullptr},
                                           {nullptr, nullptr, nullptr}};
    return properties;
  }

  // Lua constructor
  static int create(lua_State* L)
  {
    std::cout << "Create called\n";

    CheckArgCount(L, 1);

    const char* name = luaL_checkstring(L, 1);

    PersonPtr sp = std::make_shared<Person>(name);

    push(L, sp);

    return 1;
  }

  // Propertie getters and setters

  static int get_name(lua_State* L)
  {
    CheckArgCount(L, 2);
    PersonPtr a = fromStack(L, 1);
    lua_pushstring(L, a->_name.c_str());
    return 1;
  }
};

#endif  // __PERSONBINDING_H
