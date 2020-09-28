//-----------------------------------------------------------------------------
//
//  Node.  A simple tree
//
//-----------------------------------------------------------------------------
#pragma once
#include <windows.h>
#include <vector>

template <class T>
class Node
{
public:
    Node();
    virtual ~Node();

    std::vector<Node> Children;
    T Data;

};

//-----------------------------------------------------------------------------
//
//  Constructors/Destructors
//
//-----------------------------------------------------------------------------
template <class T>
Node<T>::Node()
{
    //
    //  do nothing
    //
}

template <class T>
Node<T>::~Node()
{
    //
    //  do nothing
    //
}
