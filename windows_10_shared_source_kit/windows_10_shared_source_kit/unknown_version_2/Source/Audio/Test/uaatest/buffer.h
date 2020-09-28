/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#pragma once

// self-freeing memory buffer
template<class T>
class CBuffer {
public:
    // no default constructor
    // allocates memory for size_t T's; T must have a default constructor
    explicit CBuffer(size_t n) : p(NULL) { p = new T[n]; } 
    ~CBuffer() { if (NULL != p) { delete[] p; } }

    operator T* () const { return p; }
private:
    T* p;
};
