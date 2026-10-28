#include "WidgetWeakBinding.h"
#include "common.h"

using std::make_shared;

auto main() -> int
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  WidgetWeakBinding::register_class(L);

  WidgetPtr a = make_shared<Widget>("Widget A");
  WidgetPtr b = make_shared<Widget>("Widget B");

  WidgetWeakBinding::push(L, a);
  lua_setglobal(L, "WidgetA");
  WidgetWeakBinding::push(L, b);
  lua_setglobal(L, "WidgetB");

  std::cout << "Use counts, A: " << a.use_count() << " B: " << b.use_count()
            << "\n";

  run(L, "print( WidgetA.name, WidgetB.name )");

  run(L, "print( 'Does WidgetB still exist?', WidgetB.valid )");

  run(L, "WidgetB.name = 'Soon to be ex widget.'");

  std::cout << "b.get() -> " << b->get() << "\n";

  b.reset();

  run(L, "print( 'Does WidgetB still exist?', WidgetB.valid )");

  run(L, "print( WidgetA.name, WidgetB.name )");

  run(L, "print( WidgetA.name )");
}
