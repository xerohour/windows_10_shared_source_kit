// ======================================================================================
//
//  CD3DX.cpp
//
//  Copyright (C) 2001 Microsoft Corporation
//
// =====================================================================================

#include "CD3DX.h"

// --------------------------------------------------------------------------------------
//  Wrapped D3DX Functions
//		We wrap only functions that are REQUIRED by D3D(non-D3DX) test apps.  If you
//		think you need to wrap a new D3DX function, talk to ROBALD first to see if
//		there is a framework equivalent.
//
//		We have wrapped most of the D3DXMath lib since it's so useful.
// --------------------------------------------------------------------------------------

extern D3DXVEC2NORMALIZE                         pD3DXVec2Normalize;
extern D3DXVEC2HERMITE                           pD3DXVec2Hermite;
extern D3DXVEC2CATMULLROM                        pD3DXVec2CatmullRom;
extern D3DXVEC2BARYCENTRIC                       pD3DXVec2BaryCentric;
extern D3DXVEC2TRANSFORM                         pD3DXVec2Transform;
extern D3DXVEC2TRANSFORMCOORD                    pD3DXVec2TransformCoord;
extern D3DXVEC2TRANSFORMNORMAL                   pD3DXVec2TransformNormal;
extern D3DXVEC3NORMALIZE                         pD3DXVec3Normalize;
extern D3DXVEC3HERMITE                           pD3DXVec3Hermite;
extern D3DXVEC3CATMULLROM                        pD3DXVec3CatmullRom;
extern D3DXVEC3BARYCENTRIC                       pD3DXVec3BaryCentric;
extern D3DXVEC3TRANSFORM                         pD3DXVec3Transform;
extern D3DXVEC3TRANSFORMCOORD                    pD3DXVec3TransformCoord;
extern D3DXVEC3TRANSFORMNORMAL                   pD3DXVec3TransformNormal;
extern D3DXVEC3PROJECT                           pD3DXVec3Project;
extern D3DXVEC3UNPROJECT                         pD3DXVec3Unproject;
extern D3DXVEC4CROSS                             pD3DXVec4Cross;
extern D3DXVEC4NORMALIZE                         pD3DXVec4Normalize;
extern D3DXVEC4HERMITE                           pD3DXVec4Hermite;
extern D3DXVEC4CATMULLROM                        pD3DXVec4CatmullRom;
extern D3DXVEC4BARYCENTRIC                       pD3DXVec4BaryCentric;
extern D3DXVEC4TRANSFORM                         pD3DXVec4Transform;
extern D3DXMATRIXDETERMINANT                     pD3DXMatrixDeterminant;
extern D3DXMATRIXTRANSPOSE                       pD3DXMatrixTranspose;
extern D3DXMATRIXMULTIPLY                        pD3DXMatrixMultiply;
extern D3DXMATRIXMULTIPLYTRANSPOSE               pD3DXMatrixMultiplyTranspose;
extern D3DXMATRIXINVERSE                         pD3DXMatrixInverse;
extern D3DXMATRIXSCALING                         pD3DXMatrixScaling;
extern D3DXMATRIXTRANSLATION                     pD3DXMatrixTranslation;
extern D3DXMATRIXROTATIONX                       pD3DXMatrixRotationX;
extern D3DXMATRIXROTATIONY                       pD3DXMatrixRotationY;
extern D3DXMATRIXROTATIONZ                       pD3DXMatrixRotationZ;
extern D3DXMATRIXROTATIONAXIS                    pD3DXMatrixRotationAxis;
extern D3DXMATRIXROTATIONQUATERNION              pD3DXMatrixRotationQuaternion;
extern D3DXMATRIXROTATIONYAWPITCHROLL            pD3DXMatrixRotationYawPitchRoll;
extern D3DXMATRIXTRANSFORMATION                  pD3DXMatrixTransformation;
extern D3DXMATRIXAFFINETRANSFORMATION            pD3DXMatrixAffineTransformation;
extern D3DXMATRIXLOOKATRH                        pD3DXMatrixLookAtRH;
extern D3DXMATRIXLOOKATLH                        pD3DXMatrixLookAtLH;
extern D3DXMATRIXPERSPECTIVERH                   pD3DXMatrixPerspectiveRH;
extern D3DXMATRIXPERSPECTIVELH                   pD3DXMatrixPerspectiveLH;
extern D3DXMATRIXPERSPECTIVEFOVRH                pD3DXMatrixPerspectiveFovRH;
extern D3DXMATRIXPERSPECTIVEFOVLH                pD3DXMatrixPerspectiveFovLH;
extern D3DXMATRIXPERSPECTIVEOFFCENTERRH          pD3DXMatrixPerspectiveOffCenterRH;
extern D3DXMATRIXPERSPECTIVEOFFCENTERLH          pD3DXMatrixPerspectiveOffCenterLH;
extern D3DXMATRIXORTHORH                         pD3DXMatrixOrthoRH;
extern D3DXMATRIXORTHOLH                         pD3DXMatrixOrthoLH;
extern D3DXMATRIXORTHOOFFCENTERRH                pD3DXMatrixOrthoOffCenterRH;
extern D3DXMATRIXORTHOOFFCENTERLH                pD3DXMatrixOrthoOffCenterLH;
extern D3DXMATRIXSHADOW                          pD3DXMatrixShadow;
extern D3DXMATRIXREFLECT                         pD3DXMatrixReflect;
extern D3DXQUATERNIONTOAXISANGLE                 pD3DXQuaternionToAxisAngle;
extern D3DXQUATERNIONROTATIONMATRIX              pD3DXQuaternionRotationMatrix;
extern D3DXQUATERNIONROTATIONAXIS                pD3DXQuaternionRotationAxis;
extern D3DXQUATERNIONROTATIONYAWPITCHROLL        pD3DXQuaternionRotationYawPitchRoll;
extern D3DXQUATERNIONMULTIPLY                    pD3DXQuaternionMultiply;
extern D3DXQUATERNIONNORMALIZE                   pD3DXQuaternionNormalize;
extern D3DXQUATERNIONINVERSE                     pD3DXQuaternionInverse;
extern D3DXQUATERNIONLN                          pD3DXQuaternionLn;
extern D3DXQUATERNIONEXP                         pD3DXQuaternionExp;
extern D3DXQUATERNIONSLERP                       pD3DXQuaternionSlerp;
extern D3DXQUATERNIONSQUAD                       pD3DXQuaternionSquad;
extern D3DXQUATERNIONSQUADSETUP                  pD3DXQuaternionSquadSetup;
extern D3DXQUATERNIONBARYCENTRIC                 pD3DXQuaternionBaryCentric;
extern D3DXPLANENORMALIZE                        pD3DXPlaneNormalize;
extern D3DXPLANEINTERSECTLINE                    pD3DXPlaneIntersectLine;
extern D3DXPLANEFROMPOINTNORMAL                  pD3DXPlaneFromPointNormal;
extern D3DXPLANEFROMPOINTS                       pD3DXPlaneFromPoints;
extern D3DXPLANETRANSFORM                        pD3DXPlaneTransform;
extern D3DXCOLORADJUSTSATURATION                 pD3DXColorAdjustSaturation;
extern D3DXCOLORADJUSTCONTRAST                   pD3DXColorAdjustContrast;
extern D3DXASSEMBLESHADER9                       pD3DXAssembleShader9;
extern D3DXASSEMBLESHADER8                       pD3DXAssembleShader8;
extern D3DXCREATEEFFECTFROMFILEA                 pD3DXCreateEffectFromFileA;
extern D3DXCREATEEFFECTFROMRESOURCEA             pD3DXCreateEffectFromResourceA;
extern D3DXDEBUGMUTE							 pD3DXDebugMute;

extern D3DXCHECKVERSION							pD3DXCheckVersion;
extern D3DXCREATEFONTINDIRECTA					pD3DXCreateFontIndirect;
extern D3DXCREATEMESH                           pD3DXCreateMesh;
extern D3DXCREATEMESHFVF                        pD3DXCreateMeshFVF;
extern D3DXGENERATEOUTPUTDECL					pD3DXGenerateOutputDecl;
extern D3DXRECTPATCHSIZE						pD3DXRectPatchSize;
extern D3DXTRIPATCHSIZE							pD3DXTriPatchSize;
extern D3DXTESSELLATERECTPATCH					pD3DXTessellateRectPatch;
extern D3DXTESSELLATETRIPATCH					pD3DXTessellateTriPatch;
extern D3DXCREATETEXTUREFROMFILEINMEMORY        pD3DXCreateTextureFromFileInMemory;
extern D3DXCREATETEXTURE                        pD3DXCreateTexture;
extern D3DXCREATETEXTUREFROMFILEEXA             pD3DXCreateTextureFromFileExA;
extern D3DXLOADSURFACEFROMFILEINMEMORY          pD3DXLoadSurfaceFromFileInMemory;
extern D3DXLOADSURFACEFROMSURFACE               pD3DXLoadSurfaceFromSurface;
extern D3DXLOADVOLUMEFROMVOLUME                 pD3DXLoadVolumeFromVolume;
extern D3DXLOADVOLUMEFROMMEMORY                 pD3DXLoadVolumeFromMemory;
extern D3DXFILTERTEXTURE                        pD3DXFilterTexture;
extern D3DXFILLTEXTURE                          pD3DXFillTexture;
extern D3DXCREATETEXTUREFROMFILEINMEMORYEX      pD3DXCreateTextureFromFileInMemoryEx;
extern D3DXLOADSURFACEFROMFILEA                 pD3DXLoadSurfaceFromFileA;
extern D3DXSAVESURFACETOFILEA                   pD3DXSaveSurfaceToFileA;
extern D3DXCREATETEXTUREFROMRESOURCEA           pD3DXCreateTextureFromResourceA;
extern D3DXLOADSURFACEFROMMEMORY                pD3DXLoadSurfaceFromMemory;
extern D3DXSAVETEXTURETOFILEA                   pD3DXSaveTextureToFileA;
extern D3DXTESSELLATENPATCHES                   pD3DXTessellateNPatches;
extern D3DXCREATETEXTUREFROMFILEA               pD3DXCreateTextureFromFileA;
extern D3DXCREATETEXTUREFROMRESOURCEEXA         pD3DXCreateTextureFromResourceExA;
extern D3DXCOMPILESHADER						pD3DXCompileShader;

#ifdef __cplusplus
extern "C" {
#endif

D3DXVECTOR2* WINAPI D3DXVec2Normalize                 (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV)
	{ return pD3DXVec2Normalize(pOut, pV); }
D3DXVECTOR2* WINAPI D3DXVec2Hermite                   (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pT1,CONST D3DXVECTOR2 *pV2, CONST D3DXVECTOR2 *pT2, FLOAT s)
	{ return pD3DXVec2Hermite(pOut, pV1,pT1,pV2,pT2,s); }
D3DXVECTOR2* WINAPI D3DXVec2CatmullRom                (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV0, CONST D3DXVECTOR2 *pV1,CONST D3DXVECTOR2 *pV2, CONST D3DXVECTOR2 *pV3, FLOAT s)
	{ return pD3DXVec2CatmullRom(pOut,pV0,pV1,pV2,pV3,s);}
D3DXVECTOR2* WINAPI D3DXVec2BaryCentric               (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV1, CONST D3DXVECTOR2 *pV2,CONST D3DXVECTOR2 *pV3, FLOAT f, FLOAT g)
	{ return pD3DXVec2BaryCentric(pOut, pV1, pV2, pV3, f,g); }
D3DXVECTOR4* WINAPI D3DXVec2Transform                 (D3DXVECTOR4 *pOut, CONST D3DXVECTOR2 *pV, 	CONST D3DXMATRIX *pM)
	{ return pD3DXVec2Transform(pOut, pV, pM);}
D3DXVECTOR2* WINAPI D3DXVec2TransformCoord            (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV, 	CONST D3DXMATRIX *pM)
	{ return pD3DXVec2TransformCoord(pOut, pV, pM);}
D3DXVECTOR2* WINAPI D3DXVec2TransformNormal           (D3DXVECTOR2 *pOut, CONST D3DXVECTOR2 *pV, 	CONST D3DXMATRIX *pM)
	{ return pD3DXVec2TransformNormal(pOut, pV, pM);}
D3DXVECTOR3* WINAPI D3DXVec3Normalize                 (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV)
	{ return pD3DXVec3Normalize(pOut, pV);}
D3DXVECTOR3* WINAPI D3DXVec3Hermite                   (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pT1,CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pT2, FLOAT s)
	{ return pD3DXVec3Hermite(pOut, pV1, pT1, pV2, pT2, s); }
D3DXVECTOR3* WINAPI D3DXVec3CatmullRom                (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV0, CONST D3DXVECTOR3 *pV1,CONST D3DXVECTOR3 *pV2, CONST D3DXVECTOR3 *pV3, FLOAT s)
	{ return pD3DXVec3CatmullRom(pOut, pV0, pV1,pV2, pV3,s); }
D3DXVECTOR3* WINAPI D3DXVec3BaryCentric               (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2,CONST D3DXVECTOR3 *pV3, FLOAT f, FLOAT g)
	{ return pD3DXVec3BaryCentric(pOut, pV1, pV2, pV3, f,g); }
D3DXVECTOR4* WINAPI D3DXVec3Transform                 (D3DXVECTOR4 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
	{ return pD3DXVec3Transform(pOut,pV,pM);}
D3DXVECTOR3* WINAPI D3DXVec3TransformCoord            (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
	{ return pD3DXVec3TransformCoord(pOut,pV,pM);}
D3DXVECTOR3* WINAPI D3DXVec3TransformNormal           (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
	{ return pD3DXVec3TransformNormal(pOut,pV,pM);}
D3DXVECTOR3* WINAPI D3DXVec3Project                   (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DVIEWPORT8 *pViewport,CONST D3DXMATRIX *pProjection, CONST D3DXMATRIX *pView, CONST D3DXMATRIX *pWorld)
	{ return pD3DXVec3Project(pOut,pV,pViewport,pProjection,pView,pWorld);}
D3DXVECTOR3* WINAPI D3DXVec3Unproject                 (D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DVIEWPORT8 *pViewport,CONST D3DXMATRIX *pProjection, CONST D3DXMATRIX *pView, CONST D3DXMATRIX *pWorld)
	{ return pD3DXVec3Unproject(pOut,pV,pViewport,pProjection,pView,pWorld);}
D3DXVECTOR4* WINAPI D3DXVec4Cross                     (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2,CONST D3DXVECTOR4 *pV3)
	{ return pD3DXVec4Cross(pOut,pV1,pV2,pV3);}
D3DXVECTOR4* WINAPI D3DXVec4Normalize                 (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV)
	{ return pD3DXVec4Normalize(pOut, pV);}
D3DXVECTOR4* WINAPI D3DXVec4Hermite                   (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pT1,CONST D3DXVECTOR4 *pV2, CONST D3DXVECTOR4 *pT2, FLOAT s )
	{ return pD3DXVec4Hermite(pOut,pV1,pT1,pV2,pT2,s );}
D3DXVECTOR4* WINAPI D3DXVec4CatmullRom                (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV0, CONST D3DXVECTOR4 *pV1,CONST D3DXVECTOR4 *pV2, CONST D3DXVECTOR4 *pV3, FLOAT s )
	{ return pD3DXVec4CatmullRom(pOut,pV0,pV1,pV2,pV3,s);}
D3DXVECTOR4* WINAPI D3DXVec4BaryCentric               (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV1, CONST D3DXVECTOR4 *pV2,CONST D3DXVECTOR4 *pV3, FLOAT f, FLOAT g)
	{ return pD3DXVec4BaryCentric(pOut,pV1,pV2,pV3,f,g);}
D3DXVECTOR4* WINAPI D3DXVec4Transform                 (D3DXVECTOR4 *pOut, CONST D3DXVECTOR4 *pV, CONST D3DXMATRIX *pM)
	{ return pD3DXVec4Transform(pOut,pV,pM);}
FLOAT D3DXMatrixDeterminant            (CONST D3DXMATRIX *pM)
	{ return pD3DXMatrixDeterminant(pM); }
FLOAT D3DXMatrixfDeterminant            (CONST D3DXMATRIX *pM)
	{ return pD3DXMatrixDeterminant(pM); }
D3DXMATRIX* WINAPI D3DXMatrixTranspose               (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM)
	{ return pD3DXMatrixTranspose(pOut, pM);}
D3DXMATRIX* WINAPI D3DXMatrixMultiply                (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2)
	{ return pD3DXMatrixMultiply(pOut, pM1, pM2);}
D3DXMATRIX* WINAPI D3DXMatrixMultiplyTranspose       (D3DXMATRIX *pOut, CONST D3DXMATRIX *pM1, CONST D3DXMATRIX *pM2)
	{ return pD3DXMatrixMultiplyTranspose(pOut, pM1, pM2);}
D3DXMATRIX* WINAPI D3DXMatrixInverse                 (D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM)
	{ return pD3DXMatrixInverse(pOut, pDeterminant, pM);}
D3DXMATRIX* WINAPI D3DXMatrixScaling                 (D3DXMATRIX *pOut, FLOAT sx, FLOAT sy, FLOAT sz )
	{ return pD3DXMatrixScaling(pOut, sx, sy, sz); }
D3DXMATRIX* WINAPI D3DXMatrixTranslation             (D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z )
	{ return pD3DXMatrixTranslation(pOut, x, y, z); }
D3DXMATRIX* WINAPI D3DXMatrixRotationX               (D3DXMATRIX *pOut, FLOAT Angle)
	{ return pD3DXMatrixRotationX(pOut, Angle);}
D3DXMATRIX* WINAPI D3DXMatrixRotationY               (D3DXMATRIX *pOut, FLOAT Angle)
	{ return pD3DXMatrixRotationY(pOut, Angle);}
D3DXMATRIX* WINAPI D3DXMatrixRotationZ               (D3DXMATRIX *pOut, FLOAT Angle)
	{ return pD3DXMatrixRotationZ(pOut, Angle);}
D3DXMATRIX* WINAPI D3DXMatrixRotationAxis            (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pV, FLOAT Angle)
	{ return pD3DXMatrixRotationAxis(pOut, pV, Angle);}
D3DXMATRIX* WINAPI D3DXMatrixRotationQuaternion      (D3DXMATRIX *pOut, CONST D3DXQUATERNION *pQ)
	{ return pD3DXMatrixRotationQuaternion(pOut, pQ);}
D3DXMATRIX* WINAPI D3DXMatrixRotationYawPitchRoll    (D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll)
	{ return pD3DXMatrixRotationYawPitchRoll(pOut, Yaw, Pitch, Roll); }
D3DXMATRIX* WINAPI D3DXMatrixTransformation          (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pScalingCenter,CONST D3DXQUATERNION *pScalingRotation, CONST D3DXVECTOR3 *pScaling,CONST D3DXVECTOR3 *pRotationCenter, CONST D3DXQUATERNION *pRotation,CONST D3DXVECTOR3 *pTranslation)
	{ return pD3DXMatrixTransformation(pOut, pScalingCenter,pScalingRotation, pScaling,pRotationCenter, pRotation,pTranslation); }
D3DXMATRIX* WINAPI D3DXMatrixAffineTransformation    (D3DXMATRIX *pOut, FLOAT Scaling, CONST D3DXVECTOR3 *pRotationCenter,CONST D3DXQUATERNION *pRotation, CONST D3DXVECTOR3 *pTranslation)
	{ return pD3DXMatrixAffineTransformation(pOut,Scaling,pRotationCenter,pRotation, pTranslation); }
D3DXMATRIX* WINAPI D3DXMatrixLookAtRH                (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt,CONST D3DXVECTOR3 *pUp)
	{ return pD3DXMatrixLookAtRH(pOut,pEye,pAt,pUp); }
D3DXMATRIX* WINAPI D3DXMatrixLookAtLH                (D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt,CONST D3DXVECTOR3 *pUp)
	{ return pD3DXMatrixLookAtLH(pOut,pEye,pAt,pUp);}
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveRH           (D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixPerspectiveRH(pOut, w,h,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveLH           (D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixPerspectiveLH(pOut, w,h,zn,zf);}
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovRH        (D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixPerspectiveFovRH(pOut,fovy,Aspect,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovLH        (D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixPerspectiveFovLH(pOut,fovy,Aspect,zn,zf);}
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveOffCenterRH  (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf)
	{ return pD3DXMatrixPerspectiveOffCenterRH(pOut,l,r,b,t,zn,zf);}
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveOffCenterLH  (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf)
	{ return pD3DXMatrixPerspectiveOffCenterLH(pOut,l,r,b,t,zn,zf);}
D3DXMATRIX* WINAPI D3DXMatrixOrthoRH                 (D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixOrthoRH(pOut,w,h,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixOrthoLH                 (D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
	{ return pD3DXMatrixOrthoLH(pOut,w,h,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixOrthoOffCenterRH        (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf)
	{ return pD3DXMatrixOrthoOffCenterRH(pOut,l,r,b,t,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixOrthoOffCenterLH        (D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn,FLOAT zf)
	{ return pD3DXMatrixOrthoOffCenterLH(pOut,l,r,b,t,zn,zf); }
D3DXMATRIX* WINAPI D3DXMatrixShadow                  (D3DXMATRIX *pOut, CONST D3DXVECTOR4 *pLight,CONST D3DXPLANE *pPlane)
	{ return pD3DXMatrixShadow(pOut, pLight,pPlane); }
D3DXMATRIX* WINAPI D3DXMatrixReflect                 (D3DXMATRIX *pOut, CONST D3DXPLANE *pPlane)
	{ return pD3DXMatrixReflect(pOut,pPlane); }
void WINAPI D3DXQuaternionToAxisAngle         (CONST D3DXQUATERNION *pQ, D3DXVECTOR3 *pAxis, FLOAT *pAngle)
	{ pD3DXQuaternionToAxisAngle(pQ, pAxis, pAngle); }
D3DXQUATERNION* WINAPI D3DXQuaternionRotationMatrix      (D3DXQUATERNION *pOut, CONST D3DXMATRIX *pM)
	{ return pD3DXQuaternionRotationMatrix(pOut, pM); }
D3DXQUATERNION* WINAPI D3DXQuaternionRotationAxis        (D3DXQUATERNION *pOut, CONST D3DXVECTOR3 *pV, FLOAT Angle)
	{ return pD3DXQuaternionRotationAxis(pOut, pV, Angle); }
D3DXQUATERNION* WINAPI D3DXQuaternionRotationYawPitchRoll(D3DXQUATERNION *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll)
	{ return pD3DXQuaternionRotationYawPitchRoll(pOut, Yaw, Pitch, Roll); }
D3DXQUATERNION* WINAPI D3DXQuaternionMultiply            (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2)
	{ return pD3DXQuaternionMultiply(pOut, pQ1, pQ2); }
D3DXQUATERNION* WINAPI D3DXQuaternionNormalize           (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ)
	{ return pD3DXQuaternionNormalize(pOut, pQ); }
D3DXQUATERNION* WINAPI D3DXQuaternionInverse             (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ)
	{ return pD3DXQuaternionInverse(pOut, pQ); }
D3DXQUATERNION* WINAPI D3DXQuaternionLn                  (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ)
	{ return pD3DXQuaternionLn(pOut, pQ); }
D3DXQUATERNION* WINAPI D3DXQuaternionExp                 (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ)
	{ return pD3DXQuaternionExp(pOut, pQ); }
D3DXQUATERNION* WINAPI D3DXQuaternionSlerp               (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2, FLOAT t)
	{ return pD3DXQuaternionSlerp(pOut, pQ1, pQ2, t); }
D3DXQUATERNION* WINAPI D3DXQuaternionSquad               (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pA, CONST D3DXQUATERNION *pB,CONST D3DXQUATERNION *pC, FLOAT t)
	{ return pD3DXQuaternionSquad(pOut,pQ1,pA,pB,pC,t); }
void WINAPI D3DXQuaternionSquadSetup          (D3DXQUATERNION *pAOut, D3DXQUATERNION *pBOut, D3DXQUATERNION *pCOut,CONST D3DXQUATERNION *pQ0, CONST D3DXQUATERNION *pQ1, CONST D3DXQUATERNION *pQ2, CONST D3DXQUATERNION *pQ3)
	{ pD3DXQuaternionSquadSetup(pAOut,pBOut, pCOut,pQ0,pQ1,pQ2,pQ3); }
D3DXQUATERNION* WINAPI D3DXQuaternionBaryCentric         (D3DXQUATERNION *pOut, CONST D3DXQUATERNION *pQ1,CONST D3DXQUATERNION *pQ2, CONST D3DXQUATERNION *pQ3,FLOAT f, FLOAT g)
	{ return pD3DXQuaternionBaryCentric(pOut,pQ1,pQ2,pQ3,f,g);}
D3DXPLANE* WINAPI D3DXPlaneNormalize                (D3DXPLANE *pOut, CONST D3DXPLANE *pP)
	{ return pD3DXPlaneNormalize(pOut, pP); }
D3DXVECTOR3* WINAPI D3DXPlaneIntersectLine            (D3DXVECTOR3 *pOut, CONST D3DXPLANE *pP, CONST D3DXVECTOR3 *pV1,CONST D3DXVECTOR3 *pV2)
	{ return pD3DXPlaneIntersectLine(pOut, pP, pV1, pV2); }
D3DXPLANE* WINAPI D3DXPlaneFromPointNormal          (D3DXPLANE *pOut, CONST D3DXVECTOR3 *pPoint, CONST D3DXVECTOR3 *pNormal)
	{ return pD3DXPlaneFromPointNormal(pOut, pPoint, pNormal); }
D3DXPLANE* WINAPI D3DXPlaneFromPoints               (D3DXPLANE *pOut, CONST D3DXVECTOR3 *pV1, CONST D3DXVECTOR3 *pV2,CONST D3DXVECTOR3 *pV3)
	{ return pD3DXPlaneFromPoints(pOut, pV1, pV2, pV3); }
D3DXPLANE* WINAPI D3DXPlaneTransform                (D3DXPLANE *pOut, CONST D3DXPLANE *pP, CONST D3DXMATRIX *pM)
	{ return pD3DXPlaneTransform(pOut, pP, pM); }
D3DXCOLOR* WINAPI D3DXColorAdjustSaturation         (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC, FLOAT s)
	{ return pD3DXColorAdjustSaturation(pOut, pC, s); }
D3DXCOLOR* WINAPI D3DXColorAdjustContrast           (D3DXCOLOR *pOut, CONST D3DXCOLOR *pC, FLOAT c)
	{ return pD3DXColorAdjustContrast(pOut, pC, c); }

BOOL WINAPI D3DXCheckVersion(UINT D3DSdkVersion, UINT D3DXSdkVersion)
	{ g_bStaticD3DX = false;  return pD3DXCheckVersion(D3DSdkVersion, D3DXSdkVersion); }
HRESULT WINAPI D3DXCreateMesh(DWORD NumFaces, DWORD NumVertices, DWORD Options, CONST DWORD *pDeclaration, LPVOID pD3D, LPD3DXMESH* ppMesh)
	{ return pD3DXCreateMesh(NumFaces, NumVertices,Options,pDeclaration,pD3D,ppMesh);}
HRESULT WINAPI D3DXCreateMeshFVF(DWORD NumFaces,DWORD NumVertices, DWORD Options, DWORD FVF, LPVOID pD3D, LPD3DXMESH* ppMesh)
	{ return pD3DXCreateMeshFVF(NumFaces,NumVertices,Options,FVF,pD3D,ppMesh); }
HRESULT WINAPI D3DXTessellateRectPatch(LPDIRECT3DVERTEXBUFFER9 pVB, CONST float* pNumSegs, LPD3DVERTEXELEMENT9 pInDecl, CONST D3DRECTPATCH_INFO *pRectPatchInfo, LPD3DXMESH pMesh)
	{ return pD3DXTessellateRectPatch(pVB, pNumSegs, pInDecl, pRectPatchInfo, pMesh); }
HRESULT WINAPI D3DXTessellateTriPatch(LPDIRECT3DVERTEXBUFFER9 pVB, CONST float* pNumSegs, LPD3DVERTEXELEMENT9 pInDecl, CONST D3DTRIPATCH_INFO *pTriPatchInfo, LPD3DXMESH pMesh)
	{ return pD3DXTessellateTriPatch(pVB, pNumSegs, pInDecl, pTriPatchInfo, pMesh); }
HRESULT WINAPI D3DXCreateTexture(LPVOID pDevice,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,LPVOID* ppTexture)
	{ return pD3DXCreateTexture(pDevice,Width,Height,MipLevels,Usage,Format,Pool,ppTexture); }
HRESULT WINAPI D3DXCreateTextureFromFileA(LPVOID pDevice, LPCSTR pSrcFile, LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromFileA(pDevice,pSrcFile,ppTexture); }
HRESULT WINAPI D3DXCreateTextureFromFileInMemory(LPVOID pDevice,LPCVOID pSrcData,UINT SrcDataSize,LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromFileInMemory(pDevice,pSrcData,SrcDataSize,ppTexture); }
HRESULT WINAPI D3DXCreateTextureFromResourceExA(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromResourceExA(pDevice,hSrcModule,pSrcResource, Width, Height, MipLevels,Usage,Format,Pool,Filter,MipFilter,ColorKey,pSrcInfo,pPalette,ppTexture); }
HRESULT WINAPI D3DXCreateTextureFromFileInMemoryEx(LPVOID pDevice,LPCVOID pSrcData,UINT SrcDataSize,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromFileInMemoryEx(pDevice,pSrcData,SrcDataSize,Width,Height,MipLevels,Usage,Format,Pool,Filter,MipFilter,ColorKey,pSrcInfo, pPalette,ppTexture); }
HRESULT WINAPI D3DXSaveSurfaceToFileA(LPCSTR pDestFile, D3DXIMAGE_FILEFORMAT DestFormat,LPVOID pSrcSurface,CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect)
	{ return pD3DXSaveSurfaceToFileA(pDestFile,DestFormat,pSrcSurface,pSrcPalette,pSrcRect); }
HRESULT WINAPI D3DXLoadSurfaceFromFileA(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCSTR pSrcFile,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo)
	{ return pD3DXLoadSurfaceFromFileA(pDestSurface,pDestPalette,pDestRect,pSrcFile,pSrcRect,Filter,ColorKey,pSrcInfo); }
HRESULT WINAPI D3DXCreateTextureFromFileExA(LPVOID pDevice,LPCSTR pSrcFile,UINT Width,UINT Height,UINT MipLevels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,DWORD Filter,DWORD MipFilter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo,PALETTEENTRY* pPalette,LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromFileExA(pDevice,pSrcFile,Width,Height,MipLevels,Usage,Format,Pool,Filter,MipFilter,ColorKey,pSrcInfo,pPalette,ppTexture); }
HRESULT WINAPI D3DXCreateTextureFromResourceA(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,LPVOID* ppTexture)
	{ return pD3DXCreateTextureFromResourceA(pDevice,hSrcModule,pSrcResource,ppTexture); }
HRESULT WINAPI D3DXLoadSurfaceFromSurface(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, LPVOID pSrcSurface, CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
	{ return pD3DXLoadSurfaceFromSurface(pDestSurface,pDestPalette,pDestRect,pSrcSurface,pSrcPalette,pSrcRect,Filter,ColorKey); }
HRESULT WINAPI D3DXLoadSurfaceFromFileInMemory(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCVOID SrcData,UINT rcDataSize,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey,D3DXIMAGE_INFO* pSrcInfo)
	{ return pD3DXLoadSurfaceFromFileInMemory(pDestSurface, pDestPalette, pDestRect, SrcData, rcDataSize, pSrcRect, Filter, ColorKey, pSrcInfo); }
HRESULT WINAPI D3DXLoadSurfaceFromMemory(LPVOID pDestSurface,CONST PALETTEENTRY* pDestPalette,CONST RECT* pDestRect,LPCVOID pSrcMemory,D3DFORMAT SrcFormat,UINT SrcPitch,CONST PALETTEENTRY* pSrcPalette,CONST RECT* pSrcRect,DWORD Filter,D3DCOLOR ColorKey)
	{ return pD3DXLoadSurfaceFromMemory(pDestSurface,pDestPalette,pDestRect,pSrcMemory,SrcFormat,SrcPitch,pSrcPalette,pSrcRect,Filter,ColorKey); }
HRESULT WINAPI D3DXSaveTextureToFileA(LPCSTR pDestFile,D3DXIMAGE_FILEFORMAT DestFormat,LPVOID pSrcTexture,CONST PALETTEENTRY* pSrcPalette)
	{ return pD3DXSaveTextureToFileA(pDestFile,DestFormat,pSrcTexture,pSrcPalette); }
HRESULT WINAPI D3DXLoadVolumeFromVolume(LPVOID pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST D3DBOX* pDestBox,LPVOID pSrcVolume,CONST PALETTEENTRY* pSrcPalette,CONST D3DBOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
	{ return pD3DXLoadVolumeFromVolume(pDestVolume,pDestPalette,pDestBox,pSrcVolume,pSrcPalette,pSrcBox,Filter,ColorKey); }
HRESULT WINAPI D3DXLoadVolumeFromMemory(LPVOID pDestVolume,CONST PALETTEENTRY* pDestPalette,CONST D3DBOX* pDestBox,LPCVOID pSrcMemory,D3DFORMAT SrcFormat,UINT SrcRowPitch,UINT SrcSlicePitch,CONST PALETTEENTRY* pSrcPalette,CONST D3DBOX* pSrcBox,DWORD Filter,D3DCOLOR ColorKey)
	{ return pD3DXLoadVolumeFromMemory(pDestVolume,pDestPalette,pDestBox,pSrcMemory,SrcFormat,SrcRowPitch,SrcSlicePitch,pSrcPalette,pSrcBox,Filter,ColorKey); }
HRESULT WINAPI D3DXFillTexture(LPVOID pTexture, void* pFunction,LPVOID pData)
	{ return pD3DXFillTexture(pTexture,pFunction,pData); }
HRESULT WINAPI D3DXFilterTexture(LPVOID pBaseTexture,CONST PALETTEENTRY* pPalette,UINT SrcLevel,DWORD Filter)
	{ return pD3DXFilterTexture(pBaseTexture,pPalette,SrcLevel,Filter); }
HRESULT WINAPI D3DXGenerateOutputDecl(LPD3DVERTEXELEMENT9 pOutDecl, LPD3DVERTEXELEMENT9 pInDecl)
	{ return pD3DXGenerateOutputDecl(pOutDecl, pInDecl); }
HRESULT WINAPI D3DXRectPatchSize(CONST float* pNumSegs, DWORD* pdwTriangles, DWORD* pdwVertices)
	{ return pD3DXRectPatchSize(pNumSegs, pdwTriangles, pdwVertices); }
HRESULT WINAPI D3DXTriPatchSize(CONST float* pNumSegs, DWORD* pdwTriangles, DWORD* pdwVertices)
	{ return pD3DXTriPatchSize(pNumSegs, pdwTriangles, pdwVertices); }
HRESULT WINAPI D3DXTessellateNPatches(LPD3DXMESH pMeshIn,CONST DWORD* pAdjacencyIn,FLOAT NumSegs,BOOL  QuadraticInterpNormals,LPD3DXMESH *ppMeshOut,LPD3DXBUFFER *ppAdjacencyOut)
	{ return pD3DXTessellateNPatches(pMeshIn,pAdjacencyIn,NumSegs,QuadraticInterpNormals,ppMeshOut,ppAdjacencyOut); }
HRESULT WINAPI D3DXCreateEffectFromFileA(LPVOID pDevice,LPCSTR pSrcFile,CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,LPD3DXBUFFER* ppCompilationErrors)
	{ return pD3DXCreateEffectFromFileA(pDevice,pSrcFile,pDefines,pInclude,dwFlags,pPool,ppEffect,ppCompilationErrors); }
HRESULT WINAPI D3DXCreateEffectFromResourceA(LPVOID pDevice,HMODULE hSrcModule,LPCSTR pSrcResource,CONST D3DXMACRO* pDefines,LPD3DXINCLUDE pInclude,DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect,LPD3DXBUFFER* ppCompilationErrors)
	{ return pD3DXCreateEffectFromResourceA(pDevice,hSrcModule,pSrcResource,pDefines,pInclude,dwFlags,pPool,ppEffect,ppCompilationErrors); }
HRESULT WINAPI D3DXCompileShader(LPCSTR pSrcData,UINT SrcDataLen,CONST D3DXMACRO* pDefines,LPD3DXINCLUDE pInclude,LPCSTR pFunctionName,LPCSTR pProfile,DWORD Flags,LPD3DXBUFFER* ppShader,LPD3DXBUFFER* ppErrorMsgs,LPD3DXCONSTANTTABLE*ppConstantTable)
	{ return pD3DXCompileShader(pSrcData,SrcDataLen,pDefines,pInclude,pFunctionName,pProfile, Flags,ppShader,ppErrorMsgs, ppConstantTable); }

HRESULT WINAPI D3DXAssembleShader(LPCVOID pSrcData, UINT SrcDataLen, DWORD dwFlags, LPD3DXBUFFER* ppConstants, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrors)
{
	HRESULT hr = S_OK;

	if(g_fD3DX >= 9.f)
	{
		// Convert DX8 vertex shaders to DX9
		if (0 == _tcsstr((TCHAR*)pSrcData, _T("dcl_")))
		{
			TCHAR *sNewShader = NULL;
			
			hr = VS8ToVS9((TCHAR*)pSrcData, &sNewShader);
			if(FAILED(hr))
			{
				SAFEDELETEA(sNewShader);
				return hr;
			}
			 
			hr = ::D3DXAssembleShader9(sNewShader, _tcslen(sNewShader), NULL, NULL, dwFlags, ppShader, ppErrors);
			SAFEDELETEA(sNewShader);

			return hr;
		}
		else
        {
			return ::D3DXAssembleShader9(pSrcData, SrcDataLen, NULL, NULL, dwFlags, ppShader, ppErrors);
        }
	}
	else if (g_fD3DX >= 8.f && g_fD3DX < 9.f)
    {
        // Convert DX9 vertex shaders to DX8
        if (0 == _tcsstr((TCHAR*)pSrcData, _T("dcl_")))
        {
            TCHAR *sNewShader = NULL;

            hr = VS9ToVS8((TCHAR*)pSrcData, &sNewShader);
            if (FAILED(hr))
            {
                SAFEDELETEA(sNewShader);
                return hr;
            }

            hr = pD3DXAssembleShader8(sNewShader, _tcslen(sNewShader), dwFlags, ppConstants, ppShader, ppErrors);
            SAFEDELETEA(sNewShader);

		    return hr;
        }
        else
        {
            return pD3DXAssembleShader8(pSrcData, SrcDataLen, dwFlags, ppConstants, ppShader, ppErrors);
        }
    }
	else
    {
		return E_NOTIMPL;
    }
}
BOOL WINAPI D3DXDebugMute(BOOL Mute)
{
	BOOL result = false;

	if( pD3DXDebugMute )
		return pD3DXDebugMute( Mute );

	return result;
}

typedef struct _D3DXFONT_DESCA
{
    UINT Height;
    UINT Width;
    UINT Weight;
    UINT MipLevels;
    BOOL Italic;
    BYTE CharSet;
    BYTE OutputPrecision;
    BYTE Quality;
    BYTE PitchAndFamily;
    CHAR FaceName[LF_FACESIZE];

} D3DXFONT_DESCA, *LPD3DXFONT_DESCA;

HRESULT WINAPI D3DXCreateFontIndirectA(LPDIRECT3DDEVICE9 pDevice,CONST D3DXFONT_DESCA* pFontDesc,LPD3DXFONT* ppFont)
{ 
	if(g_fD3DX < 9.1f)
	{
		LOGFONT lf;
		
		lf.lfHeight = pFontDesc->Height;
		lf.lfWidth = pFontDesc->Width;
		lf.lfWeight = pFontDesc->Weight;
		lf.lfItalic = pFontDesc->Italic ? TRUE : FALSE;
		lf.lfCharSet = pFontDesc->CharSet;
		lf.lfOutPrecision = pFontDesc->OutputPrecision;
		lf.lfQuality = pFontDesc->Quality;
		lf.lfPitchAndFamily = pFontDesc->PitchAndFamily;
		_tcsncpy( lf.lfFaceName, pFontDesc->FaceName, LF_FACESIZE );

		return pD3DXCreateFontIndirect(pDevice, (D3DXFONT_DESCA*)&lf, ppFont);
	}
	else
	{

		return pD3DXCreateFontIndirect(pDevice, pFontDesc, ppFont);
	}
}

#ifdef __cplusplus
}
#endif
