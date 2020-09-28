// TRef - reference template that supports weak references

#ifndef _TREF_H_
#define _TREF_H_

// Asserts are used to catch programming errors in reference templates
#include <assert.h>


// to work around VC++ 6 compiler crash / bogus warning:
# pragma warning(push)
# pragma warning(disable:4284) // return type for 'identifier::operator->' is not a UDT or reference to a UDT. Will produce errors if applied using infix notation

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// The full generic TRef, supports weak references:

class TRef_Node
{
	void* m_pVoid;
	unsigned long m_iRefs, m_iWeakRefs;

public:
	inline TRef_Node(void* pVoid) : m_pVoid(pVoid), m_iRefs(1), m_iWeakRefs(0) { assert(pVoid); }
	inline ~TRef_Node() { assert(m_iRefs == 0 && m_iWeakRefs == 0 && m_pVoid == 0); }

	inline void Inc(BOOL bWeak = false)
	{
		assert(m_pVoid);
		if(bWeak)
			m_iWeakRefs++;
		else
			m_iRefs++;
	}

	// returns pointer to delete in event that ref drops to 0
	inline void* Dec(BOOL bWeak = false)
	{
		void* pDeleteMe = 0;
		if(bWeak)
		{
			assert(m_iWeakRefs);		// <-- if this fires, there is an error in TRef
			m_iWeakRefs--;
		}
		else
		{
			assert(m_pVoid && m_iRefs);	// <-- if this fires, there is an error in TRef
			m_iRefs--;
			if(!m_iRefs)
			{
				pDeleteMe = m_pVoid;
				m_pVoid = 0;
			}
		}
		
		// we are responsible for deleting ourselves when all refs are gone!
		if(!(m_iRefs || m_iWeakRefs))
			delete this;

		return pDeleteMe;
	}
	
	inline void* Get() const { return m_pVoid; }
};

template<typename T, bool A = false>
class TRef
{
private:
	// Private constructor:
	TRef(TRef_Node* pNode, BOOL bWeak) :
		m_pNode(pNode), m_bWeak(bWeak) {}

	void SetInternal(TRef_Node* pNode, BOOL bWeak)
	{
		if(pNode)
			pNode->Inc(bWeak);
		Release();
		m_pNode = pNode;
		m_bWeak = bWeak;
	}

	TRef WeakOrStrong(bool bWeak)
	{
		if(Valid()) {
			m_pNode->Inc(bWeak);
			return TRef(m_pNode, bWeak);
		} else {
			Release();
			return TRef();
		}
	}

public:		// has to be public for our VC6 version to work:
	TRef_Node* m_pNode;		// shared node that holds object and ref counts
	BOOL m_bWeak;			// is *this* reference a weak one?

public:
	typedef T Type;

	TRef() : m_pNode(0), m_bWeak(false) {}
	TRef(const TRef& rT) : m_pNode(rT.m_pNode), m_bWeak(rT.m_bWeak) { if(m_pNode) m_pNode->Inc(m_bWeak); }
	~TRef() { Release(); }

	TRef& Release()
	{
		if(m_pNode)
		{
			T* pDeleteMe = (T*)m_pNode->Dec(m_bWeak);
			if(pDeleteMe)
			{
				// Here we delete contents after deleting this node
				if(A)	delete [] pDeleteMe;
				else	delete pDeleteMe;
			}
		}
		m_pNode = 0;
		return *this;
	}

	bool operator==(const TRef& rT)
	{
		if(Get() == rT.Get())
			return true;
		if(Get() && rT.Get())
			return *((T*)m_pNode->Get()) == *((T*)rT.m_pNode->Get());
		return false;
	}
	bool operator!=(const TRef& rT) { return !(*this == rT); }


#ifndef _NOPOLYMORPHICREF_		// VC6 can't handle the following:
	template<typename T2> TRef(const TRef<T2,A>& other) : m_pNode(0), m_bWeak(false)
	{
		T* pT = other.Get();
		SetInternal(other.m_pNode, other.m_bWeak);
	}
	// set this ref to right, inheriting the weak flag:
	template<typename T2> TRef& operator=(const TRef<T2,A>& right)
	{
		T* pT = right.Get();
		SetInternal(right.m_pNode, right.m_bWeak);
		return *this;
	}
	template<typename T2> operator TRef<T2,A>() { return TRef<T2,A>(*this); }
#endif

	TRef& operator=(const TRef& right)
	{
		SetInternal(right.m_pNode, right.m_bWeak);
		return *this;
	}

	// In order to use this funcion, you must enable Run-Time Type Information:
	// Can't use it with arrays!
	template<typename T2> TRef& DynamicSet(const TRef<T2,false>& right)
	{
		T* pT = dynamic_cast<T*>(right.Get());
		if(pT)
			SetInternal(right.m_pNode, right.m_bWeak);
		else
			Release();
		return *this;
	}


	// Take over control of this pointer, deleting it when ref counts drop to 0:
	TRef& Own(T*& pT)		// Note: this also sets the pointer to 0
	{
		Release();

		if(pT) m_pNode = new TRef_Node((void*)pT);
		m_bWeak = false;

		// To handle failure of allocation without leak:
		if(!m_pNode)
		{
			if(A)	delete [] pT;
			else	delete pT;
		}

		pT = 0;

		return *this;
	}

	// Return a weak or strong reference:
	TRef Weak() { return WeakOrStrong(true); }
	TRef Strong() { return WeakOrStrong(false); }

	// get a pointer to your object:
	T* Get() const { return m_pNode ? (T*)m_pNode->Get() : 0; }
	T& operator*() const { return *Get(); }
	operator T&() const { return *Get(); }
	operator T*() const { return Get(); }

	// you can use this indirection syntax also:  (ex: rObj->DoSomething())
	T* operator->() const { return Get(); }

	bool Valid() const { return (m_pNode && m_pNode->Get()) ? true : false; }
};

template<typename T> TRef<T,false> PointerRef(T* pT) { TRef<T,false> r; return r.Own(pT); }
template<typename T> TRef<T,true> ArrayRef(T* pT) { TRef<T,true> r; return r.Own(pT); }



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Slimmed down version of TRef that removes support for weak references:
struct TSRef_Node
{
	TSRef_Node(void* p) : pVoid(p), iRefs(1) {}
	void* pVoid;
	unsigned long iRefs;
};

template<typename T, bool A = false>
class TSRef
{
	void SetInternal(TSRef_Node* pNode)
	{
		if(pNode)
			pNode->iRefs++;
		Release();
		m_pNode = pNode;
	}

public:		// has to be public for our VC6 version to work:
	TSRef_Node *m_pNode;

public:
	TSRef() : m_pNode(0) {}
	TSRef(const TSRef& rT) : m_pNode(rT.m_pNode) { if(m_pNode) m_pNode->iRefs++; }
	~TSRef() { Release(); }

	TSRef& Release()
	{
		if(m_pNode && !(--m_pNode->iRefs))
		{
			if(A)	delete [] (T*)m_pNode->pVoid;
			else	delete (T*)m_pNode->pVoid;
			delete m_pNode;
		}
		m_pNode = 0;
		return *this;
	}

	bool operator==(const TSRef& rT)
	{
		if(Get() == rT.Get())
			return true;
		if(Get() && rT.Get())
			return *((T*)m_pNode->pVoid) == *((T*)rT.m_pNode->pVoid);
		return false;
	}
	bool operator!=(const TSRef& rT) { return !(*this == rT); }

	
#ifndef _NOPOLYMORPHICREF_		// VC6 can't handle the following:
	template<typename T2> TSRef(const TSRef<T2,A>& other) : m_pNode(0)
	{
		T* pT = other.Get();
		SetInternal(other.m_pNode);
	}
	// set this ref to right:
	template<typename T2> TSRef& operator=(const TSRef<T2,A>& right)
	{
		T* pT = right.Get();
		SetInternal(right.m_pNode);
		return *this;
	}
	template<typename T2> operator TSRef<T2,A>() { return TSRef<T2,A>(*this); }
#endif

	TSRef& operator=(const TSRef& right)
	{
		SetInternal(right.m_pNode);
		return *this;
	}

	// In order to use this funcion, you must enable Run-Time Type Information:
	// Can't use it with arrays!
	template<typename T2> TSRef& DynamicSet(const TSRef<T2,false>& right)
	{
		T* pT = dynamic_cast<T*>(right.Get());
		if(pT)
			SetInternal(right.m_pNode, right.m_bWeak);
		else
			Release();
		return *this;
	}


	// Take over control of this pointer, deleting it when ref counts drop to 0:
	TSRef& Own(T*& pT)		// Note: this also sets the pointer to 0
	{
		Release();

		if(pT) m_pNode = new TSRef_Node((void*)pT);

		// To handle failure of allocation without leak:
		if(!m_pNode)
		{
			if(A)	delete [] pT;
			else	delete pT;
		}
		
		pT = 0;
		
		return *this;
	}

	// get a pointer to your object:
	T* Get() const { return (m_pNode)? (T*)m_pNode->pVoid : 0; }
	T& operator*() const { return *Get(); }
	operator T&() const { return *Get(); }
	operator T*() const { return Get(); }

	// you can use this indirection syntax also:  (ex: rObj->DoSomething())
	// careful! this must be a valid reference!
	T* operator->() const { return Get(); }

	bool Valid() const { return m_pNode != NULL; }

#ifdef _DEBUG
	// Temp hack:
	unsigned long GetRefCount() { if(m_pNode) return m_pNode->iRefs; return 0; }
#endif
};

template<typename T> TSRef<T,false> PointerSRef(T* pT) { TSRef<T,false> r; return r.Own(pT); }
template<typename T> TSRef<T,true> ArraySRef(T* pT) { TSRef<T,true> r; return r.Own(pT); }




//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// TRef for COM interfaces, supports weak references:

class TCOMRef_Node
{
public:		// has to be public for our VC6 version to work:
	void* m_pVoid;
	unsigned long m_iRefs, m_iWeakRefs;

public:
	inline TCOMRef_Node(void* pVoid) : m_pVoid(pVoid), m_iRefs(1), m_iWeakRefs(0) { assert(pVoid); }
	inline ~TCOMRef_Node() { assert(m_iRefs == 0 && m_iWeakRefs == 0 && m_pVoid == 0); }

	inline void Inc(BOOL bWeak = false)
	{
		assert(m_pVoid);
		if(bWeak)
			m_iWeakRefs++;
		else
			m_iRefs++;
	}

	// returns pointer to delete in event that ref drops to 0
	inline void* Dec(BOOL bWeak = false)
	{
		void* pReleaseMe = 0;
		if(bWeak)
		{
			assert(m_iWeakRefs);
			m_iWeakRefs--;
		}
		else
		{
			assert(m_pVoid && m_iRefs);
			m_iRefs--;
			if(!m_iRefs)
			{
				pReleaseMe = m_pVoid;
				m_pVoid = 0;
			}
		}
		
		// we are responsible for deleting ourselves when all refs are gone!
		if(!(m_iRefs || m_iWeakRefs))
			delete this;

		return pReleaseMe;
	}
	
	inline void* Get() const { return m_pVoid; }
};



template<typename T>
class TCOMRef
{
	void SetInternal(TCOMRef_Node* pNode, BOOL bWeak)
	{
		if(pNode)
			pNode->Inc(bWeak);
		Release();
		m_pNode = pNode;
		m_bWeak = bWeak;
	}

	TCOMRef WeakOrStrong(bool bWeak)
	{
		if(Valid()) {
			m_pNode->Inc(bWeak);
			return TCOMRef(m_pNode, bWeak);
		} else {
			Release();
			return TCOMRef();
		}
	}

public:	// for polymorphic refs

	// Private constructor:
	TCOMRef(TCOMRef_Node* pNode, BOOL bWeak) :
		m_pNode(pNode), m_bWeak(bWeak) {}

	TCOMRef_Node* m_pNode;
	BOOL m_bWeak;

public:
	TCOMRef() : m_pNode(0), m_bWeak(false) {}
	TCOMRef(const TCOMRef& rT) : m_pNode(rT.m_pNode), m_bWeak(rT.m_bWeak) { if(m_pNode) m_pNode->Inc(m_bWeak); }
	~TCOMRef() { Release(); }

	TCOMRef& Release()
	{
		if(m_pNode)
		{
			T* pReleaseMe = (T*)m_pNode->Dec(m_bWeak);
			if(pReleaseMe)
				pReleaseMe->Release();
		}
		m_pNode = 0;
		return *this;
	}

	bool operator==(const TCOMRef& rT)
	{
		if(Get() == rT.Get())
			return true;
		if(Get() && rT.Get())
			return *(m_pNode->Get()) == *(rT.m_pNode->Get());
		return false;
	}
	bool operator!=(const TCOMRef& rT) { return !(*this == rT); }


#ifndef _NOPOLYMORPHICREF_		// VC6 can't handle the following:
	// set this ref to right, inheriting the weak flag:
	template<typename T2> TCOMRef(const TCOMRef<T2>& other) : m_pNode(0), m_bWeak(false)
	{
		T* pT = other.Get();
		SetInternal(other.m_pNode, other.m_bWeak);
	}
	// set this ref to right, inheriting the weak flag:
	template<typename T2> TCOMRef& operator=(const TCOMRef<T2>& right)
	{
		T* pT = right.Get();
		SetInternal(right.m_pNode, right.m_bWeak);
		return *this;
	}
	template<typename T2> operator TCOMRef<T2>() { return TCOMRef<T2>(*this); }
#endif

	TCOMRef& operator=(const TCOMRef& right)
	{
		SetInternal(right.m_pNode, right.m_bWeak);
		return *this;
	}

	// In order to use this funcion, you must enable Run-Time Type Information:
	template<typename T2> TCOMRef& DynamicSet(const TCOMRef<T2>& right)
	{
		T* pT = dynamic_cast<T*>(right.Get());
		if(pT)
			SetInternal(right.m_pNode, right.m_bWeak);
		else
			Release();
		return *this;
	}


	// Take over control of this pointer, deleting it when ref counts drop to 0:
	TCOMRef& Own(T*& pT)		// Note: this also sets the pointer to 0
	{
		Release();

		if(pT) m_pNode = new TCOMRef_Node((void*)pT);
		m_bWeak = false;

		if(!m_pNode && pT) pT->Release();

		pT = 0;

		return *this;
	}

	// Return a weak or strong reference:
	TCOMRef Weak() { return WeakOrStrong(true); }
	TCOMRef Strong() { return WeakOrStrong(false); }

	// get a pointer to your object:
	T* Get() const { return m_pNode ? (T*)m_pNode->Get() : 0; }
	T* operator*() const { return Get(); }
	operator T*() const { return Get(); }

	// you can use this indirection syntax also:  (ex: rObj->DoSomething())
	// make sure your reference is valid before using this operator:
	T* operator->() const { return Get(); }

	bool Valid() const { return (m_pNode) ? true : false; }
};

// Utility function for creating a TCOMRef easily:
template<typename T> TCOMRef<T> COMRef(T* pT) { TCOMRef<T> r; return r.Own(pT); }



//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// TCOMPtr - Simple auto releasing COM pointer type:

template<typename T>
class TCOMPtr
{
protected:
	T m_p;
public:
	TCOMPtr(T p = NULL, bool bAddRef = true) : m_p(p) { if(m_p && bAddRef) m_p->AddRef(); }
	TCOMPtr(const TCOMPtr& other, bool bAddRef = true) : m_p(other.m_p) { if(m_p && bAddRef) m_p->AddRef(); }
	~TCOMPtr() { Release(); }
	void Release() { if(m_p) m_p->Release(); m_p = NULL; }
	TCOMPtr& Set(T p = NULL, bool bAddRef = true) { if(p && bAddRef) p->AddRef(); Release(); m_p = p; return *this; }
	TCOMPtr& operator = (const TCOMPtr& other) { if(other.m_p) other.m_p->AddRef(); Release(); m_p = other.m_p; return *this; }
	TCOMPtr& operator = (const T p) { if(p) p->AddRef(); Release(); m_p = p; return *this; }
	const T& operator* () { return m_p; }
	const T& operator -> () { return m_p; }
	// Implicit conversions:
	operator T () { return m_p; }
	operator T*() { Release(); return &m_p; }
};


# pragma warning(pop)

#endif // _TREF_H_
