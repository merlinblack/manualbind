#include "WidgetBinding.h"
#include "WidgetWeakBinding.h"

#include "common.h"
#include "lua.h"

using std::make_shared;

auto main() -> int
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  WidgetBinding::register_class(L);
  WidgetWeakBinding::register_class(L);

  WidgetPtr a = make_shared<Widget>("Widget A");
  WidgetPtr b = make_shared<Widget>("Widget B");

  // Note the implicit convesion of shared pointer to weak when we call push
  WidgetWeakBinding::push(L, a);
  lua_setglobal(L, "WidgetA");
  WidgetWeakBinding::push(L, b);
  lua_setglobal(L, "WidgetB");

  // These should be 1 each
  std::cout << "Use counts, A: " << a.use_count() << " B: " << b.use_count()
            << "\n";

  run(L, "print( WidgetA.name, WidgetB.name )");

  // Should be true
  run(L, "print( 'Does WidgetB still exist?', WidgetB.valid )");

  // Access the name via weak pointer (binding grabs a shared pointer temporarily)
  run(L, "WidgetB.name = 'Soon to be ex widget.'");

  // Should show we've updated the name
  std::cout << "b.get() -> " << b->get() << "\n";

  // Goodbye WidgetB. The weak pointer should not stop it being free'd
  b.reset();

  // Should be false
  run(L, "print( 'Does WidgetB still exist?', WidgetB.valid )");

  // Doesn't error - binding just returns false for WidgetB
  run(L, "print( WidgetA.name, WidgetB.name )");

  // Get a shared pointer from the weak one.
  // In multi-threaded, this would stop WidgetA getting free'd
  // until your done with it.
  run(L, "strong = WidgetA:lock()");

  run(L, "print(WidgetA, strong)");

  // Should be 2 now.
  std::cout << "Use count for A: " << a.use_count() << "\n";

  // Access the name via shared pointer
  run(L, "strong.name = 'I am strong'");

  // Getting a weak pointer from a shared pointer
  run(L, "weak = strong.weakref");

  // Prove it works
  run(L, "print( weak.name )");

  // Drop the shared pointer to release the reference.
  run(L, "strong = nil");
  lua_gc(L, LUA_GCCOLLECT);
 
  // Back to 1.
  std::cout << "Use count for A: " << a.use_count() << "\n";

  // Try locking an expired weak pointer
  run(L, "gone = WidgetB:lock()");

  // Should be nil
  run(L, "print(gone)");

  // WidgetA will destruct now as 'a' goes out of scope, and no other shared pointer has a reference.
}
