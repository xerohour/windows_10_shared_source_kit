// Copyright(C) Microsoft.All rights reserved.

// multiarray.h
//
// creates an N-dimensional array of booleans on the heap
// all are initially false
//
// it is possible to specify a particular value for a particular dimension
// and query whether any value in this (N-1)-dimensional space is true, or false
// or set all values of the (N-1)-dimensional space to true, or false
//
// For example suppose you wanted to make an array of license plate assignments
// of the form AA##
// where the first two symbols are letters from A-Z
// and the last two symbols are numbers from 0-9
//
// // four-dimensional array
// // dimension 0 goes from 0 to 25 inclusive; add 'A'
// // dimension 1 goes from 0 to 25 inclusive; add 'A'
// // dimension 2 goes from 0 to 9 inclusive; add '0'
// // dimension 3 goes from 0 to 9 inclusive; add '0'
// HRESULT hr = S_OK;
// int maxes[] = { 26, 26, 10, 10 };
// CHeapBoolMultiArray license_plates(maxes, hr);
// if (FAILED(hr)) { ... } // e.g., E_OUTOFMEMORY
//
// // as license plates are "taken" you can set the particular license plate slot in question to "true"
// int coordinates[] = { 'G' - 'A', 'Y' - 'A', '8' - '0', '7' - '0' }; // GY87
// license_plates(coordinates) = true;
// ...
//
// // you can query to see whether there are any license plate slots available which end in 6
// // the first argument is the dimension (in this case, 3, the last symbol)
// // the second argument is the index to look up within that dimension (in this case, is the last symbol equal to 6)
// // the last argument is to check whether any slots are filled (true) or available (false)
// if (license_plates.AnyI(3, '6' - '0', false)) {
// ...
// }
//
// // you can set all slots for a particular index in a particular dimension at once
// license_plates.SetAllI(1, 'Q' - 'A', true); // take all slots where the second letter is "Q"
// license_plates.SetAllI(0, 'M' - 'A', false); // clear all slots where the first letter is "M"
//
template<int N>
class CHeapBoolMultiArray {
public:
    CHeapBoolMultiArray(int maxes[N], HRESULT &hr)
    : m_b(nullptr)
    {
        int count = 1;
        for (int d = 0; d < N; d++) {
            m_maxes[d] = maxes[d];
            count *= maxes[d];
        }
        
        m_b = new bool[count];
        
        if (nullptr == m_b) {
            hr = E_OUTOFMEMORY;
        } else {
            SetAll(false);
        }
    }
    
    ~CHeapBoolMultiArray() {
        delete [] m_b;
    }
    
    bool Any(bool value) {
        for (int i = 0; i < m_maxes[0]; i++) {
            if (AnyI(0, i, value)) {
                return true;
            }
        }
        
        return false;
    }
    
    bool AnyI(
        _In_range_(0, N - 1) int dim,
        int i, // 0 <= i < m_maxes[dim]
        bool value
    ) {
        // iterate over each possible value of indices
        // and see if the boolean there is equal to the given value
        // except that if the dimension number is "dim"
        // then restrict ourselves to coordinate value "i"
        // for that dimension
        //
        // think of this as reading and incrementing an odometer
        //
        // indices that are further out vary fastest
        // except that each wheel might "turn over" at a different value
        int coordinates[N] = {0};
        coordinates[dim] = i;
        
        bool last = false;
        while (!last) {
            if ((*this)(coordinates) == value) {
                return true;
            }
            
            // if the odometer is on its highest setting, we're done
            last = true;
            for (int d = N - 1; d >= 0; d--) {
                // don't increment the "fixed" dimension
                if (d == dim) { continue; }
                
                if (coordinates[d] < m_maxes[d] - 1) {
                    // we can increment this coordinate, so this wasn't the last
                    // e.g. 345 => 346
                    // or 799 => 800
                    last = false;
                    coordinates[d]++;
                    
                    // in the 799 => 800 case
                    // we then need to set all coordinates after the increment to 0
                    // except that we don't touch the "fixed" dimension
                    for (int d2 = d + 1; d2 < N; d2++) {
                        if (d2 == dim) { continue; }
                        coordinates[d2] = 0;
                    }
                    
                    break;
                }
            } // for each dimension
        } // for each odometer value
        
        return false;
    }
    
    void SetAll(bool value) {
        for (int i = 0; i < m_maxes[0]; i++) {
            SetAllI(0, i, value);
        }
    }
    
    void SetAllI(
        _In_range_(0, N - 1) int dim,
        int i, // 0 <= i < m_maxes[dim]
        bool value
    ) {
        // iterate over each possible value of indices
        // and set that boolean to the given value
        // except that if the dimension number is "dim"
        // then restrict ourselves to coordinate value "i"
        // for that dimension
        //
        // think of this as reading and incrementing an odometer
        //
        // indices that are further out vary fastest
        // except that each wheel might "turn over" at a different value
        int coordinates[N] = {0};
        coordinates[dim] = i;
        
        bool last = false;
        while (!last) {
            (*this)(coordinates) = value;
            
            // if the odometer is on its highest setting, we're done
            last = true;
            for (int d = N - 1; d >= 0; d--) {
                // don't increment the "fixed" dimension
                if (d == dim) { continue; }
                
                if (coordinates[d] < m_maxes[d] - 1) {
                    // we can increment this coordinate, so this wasn't the last
                    // e.g. 345 => 346
                    // or 799 => 800
                    last = false;
                    coordinates[d]++;
                    
                    // in the 799 => 800 case
                    // we then need to set all coordinates after the increment to 0
                    // except that we don't touch the "fixed" dimension
                    for (int d2 = d + 1; d2 < N; d2++) {
                        if (d2 == dim) { continue; }
                        coordinates[d2] = 0;
                    }
                    
                    break;
                }
            } // for each dimension
        } // for each odometer value
    }
    
    // for example, suppose:
    // * N = 3
    // * maxes = (4, 7, 9)
    // * coordinates = (3, 2, 5)
    //
    // then index = 3 * 7 * 9 + 2 * 9 + 5
    //
    // start out: index = 3
    // then: index = (3 * 7) + 2 = 3 * 7 + 2 = 23
    // then: index = (23 * 9) + 5 = 3 * 7 * 9 + 2 * 9 + 5 = 212
    //
    // note that maxes[0] is never used
    //
    // to recover the original coordinates (in reverse order) from the index,
    // do the following:
    // i(N - 1) = index % m(N - 1); index /= m(N - 1)
    // i(N - 2) = index % m(N - 2); index /= m(N - 2)
    //
    // i2 = 212 % 9 = 5; index = 212 / 9 = 23
    // i1 = 23 % 7 = 2; index = 23 / 7 = 3
    // i0 = 3
    bool &operator() (int coordinates[N]) {
        int index = coordinates[0];
        for (int i = 1; i < N; i++) {
            index = index * m_maxes[i] + coordinates[i];
        }
        
        return m_b[index];
    }
    
private:
    // m_maxes[0] is the limit in the first dimension
    // m_maxes[1] is the limit in the second dimension
    // ...
    // m_maxes[N - 1] is the limit in the Nth dimension
    int m_maxes[N];
    
    // there are (m_maxes[0] * m_maxes[1] * ... * m_maxes[N - 1]) booleans
    // the first dimension "varies slowest"
    // for example, suppose N = 3 and m_maxes is (3, 2, 4)
    // then the 24 entries are laid out as follows
    // read left-to-right in each row, then top-to-bottom, as normal
    // [0][0][0] [0][0][1] [0][0][2] [0][0][3]
    // [0][1][0] [0][1][1] [0][1][2] [0][1][3]
    // [1][0][0] [1][0][1] [1][0][2] [1][0][3]
    // [1][1][0] [1][1][1] [1][1][2] [1][1][3]
    // [2][0][0] [2][0][1] [2][0][2] [2][0][3]
    // [2][1][0] [2][1][1] [2][1][2] [2][1][3]
    bool *m_b;
};
