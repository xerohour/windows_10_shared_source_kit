//=======================================================================================
//
//  CHandle.h
//
//  Text-file logging class
//
// --------------------------------------------------------------------------------------
//
//  Copyright (c) 1999 Microsoft Corporation
//  All Rights Reserved
//
//  Original Author: Eron Hennessey
//
//=======================================================================================

#ifndef _CHANDLE_H_
#define _CHANDLE_H_

#include "FrameworkTypes.h"

class CHandle
{
protected:
	UINT			m_nGrowSize;
	UINT			m_uAllocHandles;

    UINT			m_uDevices;
	UINT			m_uHandles;
	UINT_PTR 	   *m_pHandles;

	void			ReleaseAll();
	virtual bool	AllocArray(UINT uWidth, UINT uHeight, UINT_PTR **pOut);

public:

	CHandle();
	virtual ~CHandle();
	virtual bool Setup(UINT uDevices, UINT uGrowSize = 100);

	virtual UINT AddHandle(UINT_PTR **pHandles);
	virtual UINT_PTR* GetHandle(UINT uHandle);
	virtual UINT_PTR* GetHandle(UINT uHandle, UINT uDevice);
	virtual UINT GetNumHandles(void) {return m_uHandles;}

};

class CVertexShaderHandle : public CHandle
{
public:

	CVertexShaderHandle();
	virtual ~CVertexShaderHandle();

	virtual UINT AddHandle(UINT_PTR **pHandles);
	virtual UINT_PTR* GetHandle(UINT uHandle);
	virtual UINT_PTR* GetHandle(UINT uHandle, UINT uDevice);
	virtual UINT GetNumHandles(void) {return m_uHandles;}

};

#endif
