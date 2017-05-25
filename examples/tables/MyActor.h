#ifndef __MYACTOR_H
#define __MYACTOR_H
#include <iostream>

class MyActor {

    public:

    std::string _name;

    MyActor( std::string name ) : _name(name)
    {
        std::cout << "Hello my name is " << _name << std::endl;
    }

    ~MyActor()
    {
        std::cout << "Goodbye from " << _name << std::endl;
    }
};

#endif // __MYACTOR_H
