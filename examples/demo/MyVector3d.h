#ifndef __MYVECTOR_H
#define __MYVECTOR_H

#include <iostream>
using std::cout;
using std::endl;
using std::hex;
using std::dec;

class MyVector3d
{
public:
    float x, y, z;

    MyVector3d( float nx, float ny, float nz ) : x(nx), y(ny), z(nz)
    {
        cout << "Hello from MyVector3d " << hex << this << dec << endl;
    }

    MyVector3d( const MyVector3d& vec ) : x(vec.x), y(vec.y), z(vec.z)
    {
        cout << "Hello from MyVector3d copy constructor " << hex << this << dec << endl;
    }

    ~MyVector3d()
    {
        cout << "Goodbye from MyVector3d " << hex << this << dec << endl;
    }
};

#endif // __MYVECTOR_H
