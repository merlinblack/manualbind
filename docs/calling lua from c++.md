# Calling Lua from C++

You can call Lua functions with Manualbind in two different ways. Depending on
what you want, one or the other will be more suitable.

## LuaRef
The first method is to use LuaRef to get a reference to a function and call it.

```c++

LuaRef print = LuaRef::getGlobal(L, "print");

print( "Hello world!", 1, 2, 3 );

```

This can take avantage of a Lua function's ability to take any number of
arguments of any type. Currently only one return value is supported.

## getluafunc<>
If you need type safety, or to enforce a function signature, the second method
will work better.

```c++

// `add` will be a std::function<int(int,int)>

auto add = getluafunc<int(int,int)>(L, "add");

int x = 1;
int y = 2;
int ret = add(x, y);

``` 

Here the parameters have to be int or something convertable to int. Depending
on your IDE you may get intellisense too. Like the LuaRef method, only one
return value is supported.

Either method can be used with any type that LuaStack can push onto or pop from
the Lua stack.
