#pragma once


//common stuff
extern const tstring g_vsOutDecl;
extern const tstring g_cbTessValsDecl;
extern const tstring g_cbTessFactorsDecl;
extern const tstring g_cbNumCPsDecl;
extern tstring      g_domainOpen;
extern tstring      g_partitionOpen;
extern tstring      g_outputTopologyOpen;
extern tstring      g_patchConstantFuncOpen;
extern tstring      g_attributeClose;

// hull shader stuff
extern const tstring g_hsCpDecl_pos_vertId;
extern const tstring g_hsPassThruCpFunc_2Sizes_OutIn;
extern const tstring g_hsPassThruCpFuncClamped_3Sizes_OutInIn;
extern const tstring g_hsConstDecl_quadTessFactors;
extern const tstring g_hsConstDecl_triTessFactors;
extern const tstring g_hsConstDecl_lineTessFactors;
extern const tstring g_hsConstDecl_floatVals_1Size;
extern const tstring g_hsConstDecl_intVals_1Size;
extern const tstring g_hsConstDecl_finish;