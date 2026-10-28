#ifndef _WIDGET_H
#define _WIDGET_H

#include <iostream>
#include <string>

class Widget {
  std::string _name;

 public:
  Widget(std::string name) : _name(name)
  {
    std::cout << "Constructed: " << _name << "\n";
  }
  ~Widget() { std::cout << "Destructed: " << _name << "\n"; }

  void set(std::string name) { _name = name; }

  auto get() { return _name; }
};

#endif  // _WIDGET_H
