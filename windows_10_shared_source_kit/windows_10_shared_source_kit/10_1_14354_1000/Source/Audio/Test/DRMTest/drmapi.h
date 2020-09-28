// Copyright (C) Microsoft. All rights reserved.
#ifndef _SXDRMAPI_H
#define _SXDRMAPI_H

// Error codes from DRM. 
// From krmcommstructs.h and DrmErrs.h
//enum AuthFailureEnum 
#define AuthOK                          0
#define AuthNoCert                      1
#define AuthInadequateDRMLevel          2
#define AuthTampered                    3
#define AuthCantParse                   4
#define AuthBadProvingFunc              5
#define AuthBadImage                    6
#define AuthNoDrmAttrib                 7

#define DRM_ERROR_ROOT				    0x80000000

// DRM-generated errors 
#define DRM_OK							0
#define DRM_SYSERR						DRM_ERROR_ROOT + 100	 // bad - assert-like error
#define DRM_KRM_NOT_RUNNING				DRM_ERROR_ROOT + 101
#define DRM_KRM_COMMS					DRM_ERROR_ROOT + 102
#define DRM_FILENOTFOUND				DRM_ERROR_ROOT + 103
#define DRM_FILEERR						DRM_ERROR_ROOT + 104
#define DRM_BADFILE						DRM_ERROR_ROOT + 105
#define DRM_BADTYPE						DRM_ERROR_ROOT + 106
#define DRM_DEVENUMERATE				DRM_ERROR_ROOT + 107
#define DRM_DEVDETAIL					DRM_ERROR_ROOT + 108
#define DRM_KRMNOTFOUND					DRM_ERROR_ROOT + 109
#define DRM_NOTTRUSTED					DRM_ERROR_ROOT + 110
#define DRM_FILENOTINCAT				DRM_ERROR_ROOT + 111
#define DRM_BADHANDLE					DRM_ERROR_ROOT + 112
#define DRM_AUTHFAILURE					DRM_ERROR_ROOT + 113
#define DRM_BADDRIVER					DRM_ERROR_ROOT + 114
#define DRM_BADPARAM					DRM_ERROR_ROOT + 115
#define DRM_BADKRMCERTSIG				DRM_ERROR_ROOT + 116
#define DRM_BADKRMCERT					DRM_ERROR_ROOT + 117
#define DRM_OUTOFMEMORY					DRM_ERROR_ROOT + 118
#define DRM_BADLIBRARY					DRM_ERROR_ROOT + 119
#define DRM_MISSINGLIBRARY				DRM_ERROR_ROOT + 120
#define DRM_MISSINGDLL					DRM_ERROR_ROOT + 122
#define DRM_BADPROVINGFUNCTION			DRM_ERROR_ROOT + 123
#define DRM_DATALENGTH					DRM_ERROR_ROOT + 124
#define DRM_BUFSIZE						DRM_ERROR_ROOT + 125
#define DRM_AUTHREQUIRED				DRM_ERROR_ROOT + 126
#define DRM_BADDRMLEVEL					DRM_ERROR_ROOT + 127
#define DRM_NODRMATTRIB					DRM_ERROR_ROOT + 128
#define DRM_RIGHTSNOTSUPPORTED          DRM_ERROR_ROOT + 129
#define DRM_VERIFIERENABLED             DRM_ERROR_ROOT + 130

// KRM-DRM comms
#define KRM_OK							0
#define KRM_BADIOCTL					DRM_ERROR_ROOT + 200
#define KRM_BUFSIZE						DRM_ERROR_ROOT + 201
#define KRM_BADALIGNMENT				DRM_ERROR_ROOT + 202
#define KRM_BADADDRESS					DRM_ERROR_ROOT + 203
#define KRM_OUTOFHANDLES				DRM_ERROR_ROOT + 204
#define KRM_BADSTREAM					DRM_ERROR_ROOT + 205
#define KRM_SYSERR						DRM_ERROR_ROOT + 206  // bad - assert-like error
#define DRM_BADKRMVERSION 				DRM_ERROR_ROOT + 207
// mist KRM errors
#define KRM_NOTPRIMARY					DRM_ERROR_ROOT + 300

// DRM Errors that will be propaged to users
#define DRM_INVALIDPROVING				DRM_ERROR_ROOT + 1000		// Proving function is not in text seg
#define DRM_BADIMAGE					DRM_ERROR_ROOT + 1001		// Image digest does not match PE file


// DRM error codes weren't thought out very well...  DRM_VERIFIERENABLED is a success code
#define IS_DRM_SUCCESS(d)  ((d == DRM_OK) || (d == DRM_VERIFIERENABLED))





DRM_STATUS SXDRMKOpen(OUT PDRMKHANDLE H);

DRM_STATUS SXDRMKCreateStream
( 
    IN     DRMKHANDLE      DrmHandle, 
    IN     HANDLE          KsPinHandle,
    IN     IPropertyStore *Properties,
       OUT DWORD          *StreamId, 
       OUT PSTREAMHANDLE   StreamHandle,
    IN     PDRMRIGHTS      Rights
);

DRM_STATUS SXDRMKDestroyStream(IN STREAMHANDLE StreamId);

DRM_STATUS SXDRMKAuthenticate
(
    IN     STREAMHANDLE     StreamHandle, 
    IN     BOOL             CheckCat, 
    IN     DWORD            MinDrmLevel, 
    IN     BOOL             RequireCerts, 
    IN     BOOL             CheckImage, 
       OUT PAUTHSTRUCT      Failures, 
       OUT DWORD*           NumFailures, 
    IN     DWORD            MaxFailures,
    IN     BOOL             AllowTestCert
);

DRM_STATUS SXDRMKClose(IN DRMKHANDLE H);

DRM_STATUS SXDRMKEncryptSamples
(   
    IN STREAMHANDLE     StreamHandle, 
    BYTE*               InData, 
    DWORD               InDataLength, 
    DWORD*              pInDataUsed,
    IN PDRMWAVEFORMAT   Wfmt
);

DRM_STATUS SXDRMKMakeHeader
(
    IN WAVEFORMATEX*    InFormat,
    OUT DRMWAVEFORMAT*  OutFormat, 
    OUT DWORD*          OutLen,
    IN DWORD            MaxLen
);



void LogDRMKStats();

class CDrmHelper
{
protected:
    DRMKHANDLE           m_DrmHandle;

public:
    CDrmHelper();
    ~CDrmHelper();

    BOOL                Authenticate
                        (
                            STREAMHANDLE StreamHandle,
                            BOOL fIsDrmCompliant
                        );
    BOOL                Authenticate
                        (
                            STREAMHANDLE                StreamHandle,
                            BOOL                        ParseCatalogFile,
                            BOOL                        MinDrmLevel,
                            BOOL                        RequireSignedCatalog,
                            BOOL                        CheckDriverImages,
                            PAUTHSTRUCT                 DriverFailures,
                            PDWORD                      NumFailures,
                            DWORD                       MaxFailure,
                            BOOL                        AllowTestCertificate
                        );
    BOOL                CreateStream
                        (
                            HANDLE                      KsPinHandle,
                            IPropertyStore             *Properties,
                            DWORD                      *pdwStreamId, 
                            PSTREAMHANDLE               pStreamHandle,
                            PDRMRIGHTS                  pRights
                        );
    void                DestroyStream
                        (
                            STREAMHANDLE                StreamHandle
                        );
    BOOL                Encrypt
                        (
                            STREAMHANDLE                StreamHandle,
                            PBYTE                       pInData,
                            DWORD                       InDataLength,
                            PDRMWAVEFORMAT              pWfx
                        );
    BOOL                EncryptBuffer
                        (
                            LPDIRECTSOUNDBUFFER         lpDirectSoundBuffer,
                            PDRMWAVEFORMAT              pDrmWaveFormat,
                            STREAMHANDLE                StreamHandle
                        );
    BOOL                MakeHeader
                        (
                            PWAVEFORMATEX               pInFormat,
                            PDRMWAVEFORMAT              pOutFormat
                        );
};
typedef CDrmHelper *PCDrmHelper;

#endif
