Quick Tutorial
==============

For this tutorial we will bind a simple class for Lua to use, progressively adding more features.

Here's the class we will work with.

```c++

class Widget
{
  private:
  std::string caption;
  int x;
  int y;
  
  public:
  Widget()
  {}
  Widget( std::string newCaption, int nx, int ny ) : caption(newCaption), x(nx), y(ny)
  {}
  void setCaption( std::string& newCaption )
  {
    caption = newCaption;
  }
  std::string getCaption()
  {
    return caption;
  }
  void move( int nx, int ny )
  {
    x = nx;
    y = ny;
  }
  int getX() { return x; }
  int getY() { return y; }
  void Show( bool upsideDown = false );	// Defined elsewhere.
}

```
This class is not the best for binding, however I am going to pretend that for whatever reason, we are stuck with it. This is a common senario.

First off, lets do a really simple binding, that allows us to hand a pointer to Lua, and Lua give it back to C++, with type checking. We will use the full shared pointer style binding, as we'll add more later.

```c++

struct WidgetBinding : public ManualBind::Binding<WidgetBinding, Widget>
{
  static constexpr const char* class_name = "Widget";
};

```
This is all we need for now. It is a struct just to make everything public by default. ManualBind::Binding will supply defaults for the list of member functions, list of properties, and the constructor.
The default constructor raises an error, saying you can not construct this object from Lua.

To use this we need to register the binding to Lua.

```c++

  lua_State* L = luaL_newstate();
  ...
  WidgetBinding::register_class(L);
  ...
```

Now we can push and pull an instance of Widget to Lua as follows. We can't really do anything with it in Lua yet, except pass it back to C++.

```c++
  using WidgetPtr = std::shared_ptr<Widget>;
  
  WidgetPtr w = std::make_shared<Widget>();
  
  // Push and assign to global.
  WidgetBinding::push( L, w );
  lua_setglobal( L, "widget1" );
  
  lua_getglobal( L, "widget1" );
  // Retrieve from the stack.
  WidgetPtr w = WidgetBinding::fromStackThrow( L, -1 );
```

There are two versions of 'fromStack'. 'fromStackThrow' is used in C++ code not called by Lua, and will throw an exception if the item on the stack is not the class you are trying to get. 'fromStack' is for code called by Lua and will call luaL_error instead.

So now we can pass an instance of a Widget to Lua, and retrieve it or Lua could call a C/C++ function with the instance. In rare cases this is all you will need.

Let's give Lua the ability to make Widgets:
```c++

struct WidgetBinding : public ManualBind::Binding<WidgetBinding, Widget>
{
  static constexpr const char* class_name = "Widget";
  
  static void create( lua_State* L )
  {
  	std::string newCaption( luaL_checkstring( L, 1 ) );
  	int nx = luaL_checkinteger( L, 2 );
  	int ny = luaL_checkinteger( L, 3 );
  	
  	WidgetPtr w = std::make_shared<Widget>( newCaption, nx, ny );
  	
  	push( L, w );
  	
  	return 1;
  }
};

```

This construction function will be automatically found by template magic and added to the binding. For this reason it must be called 'create'. Unlike most Lua Binding libraries, all 'glue' functions, i.e. this create and other functions we will write soon, need some familiarity with working with the Lua stack and Lua's C API. I've done this intentionally. This allows to allows you all the power of the Lua C API.

Here we are simply grabbing each parameter from the Lua stack and using them to create a shared pointer to a brand new Widget. Then we push that pointer onto the Lua stack and return '1' as in the count of return values this function returns to Lua. The push function is the same one we saw earlier, albeit without the 'WidgetBinding::' qualifier.

Now in Lua code we can do this:

```Lua
  w = Widget( 'Hello World', 5, 15 )
```

Now that we can make Widgets from Lua, it would be nice to be able to call the 'show' method.

We do this by adding another static function to the binding to override the default one which has no bound functions. We also need a small 'glue' function to marshal any parameters.
```c++

struct WidgetBinding : public ManualBind::Binding<WidgetBinding, Widget>
{
  static constexpr const char* class_name = "Widget";
  
  static void create( lua_State* L )
  {
  	std::string newCaption( luaL_checkstring( L, 1 ) );
  	int nx = luaL_checkinteger( L, 2 );
  	int ny = luaL_checkinteger( L, 3 );
  	
  	WidgetPtr w = std::make_shared<Widget>( newCaption, nx, ny );
  	
  	push( L, w );
  	
  	return 1;
  }
  
  static luaL_Reg* members()
  {
    static luaL_Reg members[] =
    {
      { "show", show },
      { nullptr, nullptr }
    };
    return members;
  }
  
  static int show( lua_State* L )
  {
    WidgetPtr w = fromStack( L, 1 );
    // nil or not present will result in 'false'. Otherwise whatever lua_toboolean returns.
    bool upsideDown = luaL_opt( L, lua_toboolean, 2, false );
    
    w->show( upsideDown );
    
    return 0;
  }

};

```
The list in the members function is simply passed to the luaL_setfuncs. It follows the same rules, as does the function prototype for the 'show' glue function. This function grabs the 'self' or 'this' pointer from the stack and the optional boolean parameter and then calls the real member function. Note that unless you make the binding struct a friend of the class you are binding, you can only use it's public interface.

In Lua we can now call the member function like this:
```Lua
  widget1:show()
  -- or --
  widget1:show(true)
```
Note the colon, rather than dot as per usual Lua convention. This is functionally equivalent to 'widget1.show(widget1)'.

Now we could bind all the 'getX' and 'getY' and 'move' member functions in the same way. However there is another way to expose these to Lua, in the form of properties. This allows you to use these as if they are public member variables, even though they are calling functions to set and get.

```c++

struct WidgetBinding : public ManualBind::Binding<WidgetBinding, Widget>
{
  static constexpr const char* class_name = "Widget";
  
  static void create( lua_State* L )
  {
  	std::string newCaption( luaL_checkstring( L, 1 ) );
  	int nx = luaL_checkinteger( L, 2 );
  	int ny = luaL_checkinteger( L, 3 );
  	
  	WidgetPtr w = std::make_shared<Widget>( newCaption, nx, ny );
  	
  	push( L, w );
  	
  	return 1;
  }
  
  static luaL_Reg* members()
  {
    static luaL_Reg members[] =
    {
      { "show", show },
      { nullptr, nullptr }
    };
    return members;
  }
  
  static int show( lua_State* L )
  {
    WidgetPtr w = fromStack( L, 1 );
    // nil or not present will result in 'false'. Otherwise whatever lua_toboolean returns.
    bool upsideDown = luaL_opt( L, lua_toboolean, 2, false );
    
    w->show( upsideDown );
    
    return 0;
  }
  
  static bind_properties* properties()
  {
    static bind_properties properties[] =
    {
      { "x", get, set },	// Using the same two get and set functions for both
      { "y", get, set },	// However you can define seperate ones for each property.
      { nullptr, nullptr, nullptr }
    };
    return properties;
  }
  
  static int get( lua_State* L )
  {
  	// Lua stack will be:
  	// 1	- instance
  	// 2	- property name to get.
  	
  	WidgetPtr w = fromStack( L, 1 );
  	const char* prop = luaL_checkstring( L, 2 );
  	
  	switch( prop[0] )
  	{
  	   case 'x':
  	     lua_pushinteger( L, w->getX() );
  	     break;
  	   case 'y':
  	     lua_pushinteger( L, w->getY() );
  	     break;
  	   default:
  	     return luaL_error( L, "This should never happen." );
  	     break;
  	 }
  	 
  	 return 1; // Returning one item on the stack.
  }
  
  static int set( lua_State* L )
  {
  	// Lua stack will be:
  	// 1	- instance
  	// 2	- property name to set.
  	// 3	- value to set
  	
  	// In this example we can not set X or Y independantly, so we grab both first and update.
  	WidgetPtr w = fromStack( L, 1 );
  	int x = w->getX();
  	int y = w->getY();
  	
  	const char* prop = luaL_checkstring( L, 2 );
  	int value = luaL_checkinteger( L, 3 );
  	
  	switch( prop[0] )
  	{
  	  case 'x':
  	    x = value;
  	    break;
  	  case 'y':
  	    y = value;
  	    break;
  	  default:
  	    return luaL_error( L, "this really can't happen!" );
  	    break;
  	}
  	
  	w->move( x, y );
  	
  	return 0;
  }

};

```

There we have it.  In this example both X and Y properties share the same glue code, but this is not required. They could have completely different functions each. The set function does some extra work to allow us to set X and Y independently in Lua, even though in C++ you need to set both at once. In this way your glue functions can make a class more Lua friendly, or just the way you want them. We can now do this in Lua.
```Lua
  widget1.x = 100
  widget1.y = widget.y + 5
```
Note the dot operator this time.

This concludes the 'quick' tutorial. There are a few more things the binding code can do. Have a look at the examples and the tests.
