#include "WidgetBinding.h"
#include "WidgetWeakBinding.h"

// These functions are here to avoid circular references
// I.e. WidgetBinding needing WidgetWeakBinding to compile,
// yet WidgetWeakBinding needing WidgetBinding to compile.

int WidgetBinding::get_weak_ptr(lua_State* L)
{
  CheckArgCount(L, 2);
  WidgetPtr w = fromStack(L, 1);
  WidgetWeakBinding::push(L, w);
  return 1;
}

int WidgetWeakBinding::get_lock(lua_State* L)
{
  CheckArgCount(L, 1);
  WidgetWeakPtr ww = fromStack(L, 1);
  WidgetPtr w = ww.lock();
  if (!w) {
    lua_pushnil(L);
  }
  else {
    WidgetBinding::push(L, w);
  }
  return 1;
}
