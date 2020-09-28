// Copyright(C) Microsoft.All rights reserved.

#pragma once

// Supported test types
typedef enum GAUDITTESTFLAGS { GOF_ROOT=0, GOF_PROP, GOF_IOCTL } GAUDITTESTFLAGS;

#define MASK_GOF			0x0000ffff

// Optimization flags
#define RUN_IF_SUPPORTED	0x00010000
#define RUN_PIN_PROP        0x00020000
#define RUN_PIN_PROP_PINHANDLE        0x01000000      // added as  I wanted run some test cases with pin Handle after pin creat and do not wnat to mix it with Run_Pin_Prop
#define RUN_NODE_PROP       0x00040000
#define RUN_PROP    		0x00080000
#define RUN_FILTER_CREATE	0x00100000
#define RUN_PIN_CREATE		0x00200000


// Gaudit params
typedef struct tagGauditParams
{
	HANDLE hFile;
	union
	{
		KSPROPERTY Prop;
		KSP_PIN    PinProp;
		KSNODEPROPERTY NodeProp;
		ULONG      Ioctl;
		ULONG_PTR  Reserved; 
	};
	ULONG      cbSize;
	ULONG      cbOutSize;
} GAUDITPARAMS, *PGAUDITPARAMS;

// Gaudit test function prototype
typedef DWORD (WINAPI *PGAUDITTESTFCT)(PGAUDITPARAMS);

// Macros for test definitions
#define INVALID_PROPERTY    (ULONG)-1
#define INVALID_IOCTL       (ULONG)-1

// Gaudit test definition
typedef struct tagGauditTestDef
{
	ULONG  ulGauditFlags;

	// KSPROP
	struct 
	{
		GUID   guidPropSet;
		ULONG  ulPropId;
		LPTSTR szPropSetName;
		LPTSTR szPropIdName;
	} KsProp;

	// IOCTL
	struct
	{
		ULONG  ulIoctl;
		LPTSTR szIoctlName;
	} Ioctl;

	ULONG_PTR      pReserved;
	ULONG		   cbOutSize;
} GAUDITTESTDEF, *PGAUDITTESTDEF;


// Instead of the macros
template<class T>
__forceinline ULONG ArraySize(T obj)
{
	return sizeof(obj)/sizeof(obj[0]);
}

