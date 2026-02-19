#ifndef __PERSON_H
#define __PERSON_H
#include <iostream>

class Person {
 public:
  std::string _name;

  Person(std::string name) : _name(name)
  {
    std::cout << "Hello my name is " << _name << std::endl;
  }

  ~Person() { std::cout << "Goodbye from " << _name << std::endl; }
};

#endif  // __PERSON_H
