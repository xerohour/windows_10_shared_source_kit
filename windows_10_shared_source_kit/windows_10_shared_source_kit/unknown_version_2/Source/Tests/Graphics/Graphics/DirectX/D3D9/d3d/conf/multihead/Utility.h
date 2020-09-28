//////////////////////////////////////////////////////////////////////
// File:  Utility.h
//
// Purpose:
// Helper functions.
//
// History:
// 21 Nov 2001  BruceN    Created.
// 07 Apr 2006  SEdmison  Factored into separate header.
//////////////////////////////////////////////////////////////////////

#ifndef __UTILITY_H__
#define __UTILITY_H__


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

#include <vector>
#include <algorithm>


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CPtrList
//
// Purpose:
// Holds a list of pointers to be deleted when the list
// is deleted.
///////////////////////////////////////////////////////
template< class T >
class CPtrList
:
	public std::vector< T* >
{
	public:
		~CPtrList()
		{
			std::for_each( begin(), end(), DeletePtr );
		}


	protected:
		static void DeletePtr( T *pT )
		{
			pT->Close();
			delete pT;
		}
};






/////////////////////////////////////////////////////////////////
//
// Helper functions
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetRefCount
//
// Purpose:
// Retrieves the reference count of a COM object.
///////////////////////////////////////////////////////
template< typename T >
LONG GetRefCount( T *t )
{
    return t ? t->AddRef(), t->Release() : 0;
}


#endif //__UTILITY_H__
