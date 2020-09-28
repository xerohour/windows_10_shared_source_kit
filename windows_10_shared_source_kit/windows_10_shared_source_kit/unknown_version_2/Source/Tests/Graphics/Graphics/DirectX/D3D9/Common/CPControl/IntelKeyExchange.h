#pragma once

typedef struct _tagGPUCP_CRYPTOSESSION_PAVP_KEYEXCHANGE
{
	GUID	PAVPKeyExchangeGUID;
	UINT	DataSize;
	VOID*	pKeyExchangeParams;

}GPUCP_CRYPTOSESSION_PAVP_KEYEXCHANGE;

typedef struct _tagPAVP_FIXED_EXCHANGE_PARAMS
{
	DWORD	FixedKey[4];
	DWORD	SessionKey[4];

}PAVP_FIXED_EXCHANGE_PARAMS;

typedef struct _tagPAVP_SAFEID_EXCHANGE_PARAMS
{
	//TBD
	DWORD	FixedKey[4];
	DWORD	SessionKey[4];

}PAVP_SAFEID_EXCHANGE_PARAMS;

// {B15AFE59-70BF-4063-A733-EAE1A2E78242}
static const GUID D3DKEYEXCHANGE_CANTIGA = 
{ 0xB15AFE59, 0x70BF, 0x4063, { 0xa7, 0x33, 0xea, 0xe1, 0xa2, 0xe7, 0x82, 0x42 } };

// {A75334C7-080C-4539-8E2D-E6A0D2B10FF0}
static const GUID D3DKEYEXCHANGE_EAGLELAKE = 
{ 0xA75334C7, 0x080C, 0x4539, { 0x8e, 0x2d, 0xe6, 0xa0, 0xd2, 0xb1, 0x0f, 0xf0 } };

// {TBD}
static const GUID D3DKEYEXCHANGE_SAFEID = 
{ 0xB15AFE59, 0x70BF, 0x4063, { 0xa7, 0x33, 0xea, 0xe1, 0xa2, 0xe7, 0x82, 0x44 } };

// {TBD}
static const GUID D3DKEYEXCHANGE_LARABEE = 
{ 0xB15AFE59, 0x70BF, 0x4063, { 0xa7, 0x33, 0xea, 0xe1, 0xa2, 0xe7, 0x82, 0x45 } };

// These are Cantiga B2 keys - FOR TESTING ONLY!!!  NOT FOR PRODUCTION USE!!!
static const DWORD   s_dwFixedKeyCtgB2[4] = {0x1d593900, 0xa3310324, 0x20e774c4, 0x6b32bd3c};
static const DWORD   s_dwEncryptedFixedKeyCtgB2[4] = {0xb0b03c49, 0x1653d48f, 0xca39be75, 0x5e6ea5dc};

// These are Eaglelake A2 hardware keys Try 4- FOR TESTING ONLY!!!  NOT FOR PRODUCTION USE!!!
static const DWORD   s_dwFixedKeyEglA2[4] = {0x43489E03, 0x2FE2969E, 0xAF158415, 0xE472EB55};
static const DWORD   s_dwEncryptedFixedKeyEglA2[4] = {0xAF2896BF, 0xCA3E16E8, 0xFA8AC85C, 0xD2ECC9FF};