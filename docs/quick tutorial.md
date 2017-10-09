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
