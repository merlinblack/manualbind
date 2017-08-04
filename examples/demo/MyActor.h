#ifndef __MYACTOR_H
#define __MYACTOR_H
#include <iostream>

class MyActor {
protected:

    std::string _name;

public:

    int _age;

public:

    MyActor( std::string name, int age ) : _name(name), _age(age)
    {
        std::cout << "Hello my name is " << _name << " and I am " << _age << " years old." << std::endl;
        std::cout << std::hex << this << std::dec << std::endl;
    }

    void walk() {
        std::cout << _name << " is Walking\n";
    }

    void setName( std::string name )
    {
        std::cout << "Changing my name to: " << name << std::endl;
        _name = name;
    }

    ~MyActor()
    {
        std::cout << "Goodbye from " << _name << std::endl;
        std::cout << std::hex << this << std::dec << std::endl;
    }
};

#endif // __MYACTOR_H
