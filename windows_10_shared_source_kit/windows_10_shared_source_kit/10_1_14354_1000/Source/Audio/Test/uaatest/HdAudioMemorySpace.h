/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef _PCI_EXPRESS_CONFIG_SPACE_H_
#define  _PCI_EXPRESS_CONFIG_SPACE_H_

//#include "HitDefinition.h"


extern TableDefinition GeneralInformation;
extern TableDefinition InputStreamDescriptor;
extern TableDefinition OutputStreamDescriptor;
extern TableDefinition BiDirectionalStreamDescriptor;

extern RegisterDefinition Global_Capabilities;
extern RegisterDefinition Global_Control;
extern RegisterDefinition Wake_Enable;
extern RegisterDefinition Wake_Status;
extern RegisterDefinition Global_Status;

extern RegisterDefinition Interrupt_Control;
extern RegisterDefinition Interrupt_Status;
extern RegisterDefinition Stream_Synchronization;
extern RegisterDefinition CORBLBASE;
extern RegisterDefinition CORBWP;
extern RegisterDefinition CORBRP;
extern RegisterDefinition CORBCTL;
extern RegisterDefinition CORBSTS;
extern RegisterDefinition CORBSIZE;
extern RegisterDefinition RIRBLBASE;
extern RegisterDefinition RIRBWP;
extern RegisterDefinition RINTCNT;
extern RegisterDefinition RIRBCTL;
extern RegisterDefinition RIRBSTS;
extern RegisterDefinition RIRBSIZE;
extern RegisterDefinition DPLBASE;
extern RegisterDefinition SDControl_3;
extern RegisterDefinition SDControl_2;
extern RegisterDefinition SDControl_1;
extern RegisterDefinition SDStatus;
extern RegisterDefinition SDFormat;
extern RegisterDefinition SDLPBA;
extern RegisterDefinition SDLastValidIndex;
extern RegisterDefinition Immediate_Command_Status;


#endif
