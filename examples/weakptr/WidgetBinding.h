#ifndef __WIDGETBINDING_H
#define __WIDGETBINDING_H
#include "LuaBinding.h"
#include "Widget.h"
#include "lua.h"

using namespace ManualBind;

using WidgetPtr = std::shared_ptr<Widget>;

struct WidgetBinding : public Binding<WidgetBinding, Widget> {
  static constexpr const char* class_name = "Widget";

  static bind_properties* properties()
  {
    static bind_properties properties[] = {{"name", get_name, set_name},
                                           {"weakref", get_weak_ptr, nullptr},
                                           {nullptr, nullptr, nullptr}};
    return properties;
  }

  // Property getters and setters

  static int get_name(lua_State* L)
  {
    CheckArgCount(L, 2);
    WidgetPtr w = fromStack(L, 1);
    lua_pushstring(L, w->get().c_str());
    return 1;
  }

  static int set_name(lua_State* L)
  {
    CheckArgCount(L, 3);
    WidgetPtr w = fromStack(L, 1);
    w->set(lua_tostring(L, 3));
    return 0;
  }

  static int get_weak_ptr(lua_State* L);
};

#endif  // __WIDGETBINDING_H
