
#ifndef __WIDGETWEAKBINDING_H
#define __WIDGETWEAKBINDING_H
#include "LuaBinding.h"
#include "Widget.h"
#include "lua.h"

using WidgetWeakPtr = std::weak_ptr<Widget>;
using WidgetPtr = std::shared_ptr<Widget>;

using namespace ManualBind;

struct WidgetWeakBinding : public PODBinding<WidgetWeakBinding, WidgetWeakPtr> {
  static constexpr const char* class_name = "WidgetWeakPtr";

  static bind_properties* properties()
  {
    static bind_properties properties[] = {{"name", get_name, set_name},
                                           {"valid", get_valid, nullptr},
                                           {nullptr, nullptr, nullptr}};
    return properties;
  }

  // Property getters and setters

  static int get_name(lua_State* L)
  {
    CheckArgCount(L, 2);
    WidgetWeakPtr ww = fromStack(L, 1);
    WidgetPtr ws = ww.lock();
    if (!ws) {
      lua_pushboolean(L, false);
    }
    else {
      lua_pushstring(L, ws->get().c_str());
    }
    return 1;
  }

  static int set_name(lua_State* L)
  {
    CheckArgCount(L, 3);
    WidgetWeakPtr ww = fromStack(L, 1);
    WidgetPtr ws = ww.lock();
    if (!ws) {
      lua_pushboolean(L, false);
      return 1;
    }
    ws->set(lua_tostring(L, 3));
    lua_pushboolean(L, true);
    return 1;
  }

  static int get_valid(lua_State* L)
  {
    CheckArgCount(L, 2);
    WidgetWeakPtr ww = fromStack(L, 1);
    lua_pushboolean(L, !ww.expired());
    return 1;
  }
};

#endif  // __WIDGETWEAKBINDING_H
