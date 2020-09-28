/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <hitframework.h>

// HD Audio Controller Specification Revision - 1.0 April 15, 2004

TableDefinition 
GeneralInformation = {
    {"Global Capabilities", 2},
    {"Minor Version -VMIN", 1},
    {"Major Version -VMAJ", 1},
    {"Output Payload Capability", 2},
    {"Input Payload Capability", 2},
    {"Global Control", 4},
    {"Wake Enable", 2},
    {"Wake Status", 2},
    {"Global Status", 2},
    {"Reserved_1", 6},
    {"Output Stream Payload Capability", 2},
    {"Input Stream Payload Capability", 2},
    {"Reserved_2", 4},
    {"Interrupt Control", 4},
    {"Interrupt Status", 4},
    {"Reserved_3", 8},
    {"Wall Clock Counter", 4},        
    {"Reserved_4", 4},
    {"Stream Synchronization", 4},        
    {"Reserved_5", 4},
    {"CORB Lower Base Address", 4},
    {"CORB Upper Base Address", 4},
    {"CORB Write Pointer", 2},
    {"CORB Read Pointer", 2},
    {"CORB Control", 1},        
    {"CORB Status", 1},        
    {"CORB Size", 1},        
    {"Reserved_6", 1},        
    {"RIRB Lower Base Address", 4},
    {"RIRB Upper Base Address", 4},
    {"RIRB Write Pointer", 2},
    {"Response Interrupt Count", 2},
    {"RIRB Control", 1},        
    {"RIRB Status", 1},        
    {"RIRB Size", 1},        
    {"Reserved_7", 1},        
    {"Immediate Command Output Interface", 4},
    {"Immediate Response Input Interface", 4},
    {"Immediate Command Status", 2},
    {"Reserved_8", 6},        
    {"DMA Position Buffer Lower Base", 4},
    {"DMA Position Buffer Upper Base", 4},
    {"Reserved_9", 8},
    END_DEFINITION
};

// TODO: remoave any register decleration which is more then 4 buytes in the lenght since HIT does not support testing of register more then DWORD
TableDefinition 
InputStreamDescriptor = {
    {"Input Stream Descriptor Control_1", 1},        
    {"Input Stream Descriptor Control_2", 1},        
    {"Input Stream Descriptor Control_3", 1},        
    {"Input Stream Descriptor Status", 1},        
    {"Input Stream Descriptor Link Position in Buffer", 4},        
    {"Input Stream Descriptor Cyclic Buffer Length", 4},        
    {"Input Stream Descriptor Last Valid Index", 2},        
    {"Reserved_1", 2},
    {"Input Stream Descriptor FIFO Size", 2},        
    {"Input Stream Descriptor Format", 2},        
    {"Reserved_2", 4},        
    {"Input Stream Descriptor Buffer Descriptor List Pointer - Lower", 4},        
    {"Input Stream Descriptor Buffer Descriptor List Pointer - Upper", 4},        
    END_DEFINITION
};

TableDefinition 
OutputStreamDescriptor = {
    {"Output Stream Descriptor Control_1", 1},        
    {"Output Stream Descriptor Control_2", 1},        
    {"Output Stream Descriptor Control_3", 1},        
    {"Output Stream Descriptor Status", 1},        
    {"Output Stream Descriptor Link Position in Buffer", 4},        
    {"Output Stream Descriptor Cyclic Buffer Length", 4},        
    {"Output Stream Descriptor Last Valid Index", 2},        
    {"Reserved_1", 2},
    {"Output Stream Descriptor FIFO Size", 2},        
    {"Output Stream Descriptor Format", 2},        
    {"Reserved_2", 4},        
    {"Output Stream Descriptor Buffer Descriptor List Pointer - Lower", 4},        
    {"Output Stream Descriptor Buffer Descriptor List Pointer - Upper", 4},        
    END_DEFINITION
};

TableDefinition 
BiDirectionalStreamDescriptor = {
    {"BiDirectional Stream Descriptor Control_1", 1},        
    {"BiDirectional Stream Descriptor Control_2", 1},        
    {"BiDirectional Stream Descriptor Control_3", 1},        
    {"BiDirectional Stream Descriptor Status", 1},        
    {"BiDirectional Stream Descriptor Link Position in Buffer", 4},        
    {"BiDirectional Stream Descriptor Cyclic Buffer Length", 4},        
    {"BiDirectional Stream Descriptor Last Valid Index", 2},        
    {"Reserved_1", 2},
    {"BiDirectional Stream Descriptor FIFO Size", 2},        
    {"BiDirectional Stream Descriptor Format", 2},        
    {"Reserved_2", 4},        
    {"BiDirectional Stream Descriptor Buffer Descriptor List Pointer - Lower", 4},        
    {"BiDirectional Stream Descriptor Buffer Descriptor List Pointer - Upper", 4},        
    END_DEFINITION
};

RegisterDefinition
Global_Capabilities = {
    {"64 Bit Address Supported -640K", 1},
    {"Number of Serial Data Out Signals -NSDO", 2},
    {"Number of Bidirectional Streams Supported -BSS", 5},
    {"Number of Input Streams Supported -ISS", 4},
    {"Number of Output Streams Supported -OSS", 4},
    END_DEFINITION
};

RegisterDefinition
Global_Control = {
    {"Controller Reset -CRST", 1},
    {"Flush Control -FCNTRL", 1},
    {"Reserved_1", 6},
    {"Accept Unsolicited Response Enable -UNSOL", 1},
    {"Reserved", 23},
    END_DEFINITION
};

RegisterDefinition
Wake_Enable = {
    {"SDIN Wake Enable Flags -SDIWEN", 15},
    {"Reserved_2", 1},
    END_DEFINITION
};

RegisterDefinition
Wake_Status = {
    {"SDIN State Change Status Flags -SDIWAKE", 15},
    {"Reserved_3", 1},
    END_DEFINITION
};


RegisterDefinition
Global_Status = {
    {"Reserved", 1},
    {"Flush Status -FSTS", 1},
    {"Reserved_4", 14},
    END_DEFINITION
};

RegisterDefinition
Interrupt_Control = {
    {"Stream Interrupt Enable -SIE", 30},
    {"Controller Interrupt Enable -CIE", 1},
    {"Global Interrupt Enable -GIE", 1},
    END_DEFINITION
};

RegisterDefinition
Interrupt_Status = {
    {"Stream Interrupt Status -SIS", 30},
    {"Controller Interrupt Status -CIS", 1},
    {"Global Interrupt Status -GIS", 1},
    END_DEFINITION
};

RegisterDefinition
Stream_Synchronization = {
    {"Stream Synchronization Bits -SSYNC", 30},
    {"Reserved", 2},
    END_DEFINITION
};

RegisterDefinition
CORBLBASE= {
    {"CORB Lower Base Unimplemented Bits", 7},
    {"CORB Lower Base Address - CORBLBASE", 25},
    END_DEFINITION
};

RegisterDefinition
CORBWP= {
    {"CORB Write Pointer -CORBWP", 8},
    {"Reserved", 8},
    END_DEFINITION
};

RegisterDefinition
CORBRP= {
    {"CORB Read Pointer -CORBRP", 8},
    {"Reserved", 7},
    {"CORB Read Pointer Reset -CORBRPRST", 1},   
    END_DEFINITION
};

RegisterDefinition
CORBCTL= {
    {"CORB Memory Error Interrupt Enable -CMEIE", 1},
    {"Enable CORB DMA Engine", 1},
    {"Reserved", 6},   
    END_DEFINITION
};

RegisterDefinition
CORBSTS= {
    {"CORB Memory Error Indication -CMEI", 1},
    {"Reserved", 7},   
    END_DEFINITION
};

RegisterDefinition
CORBSIZE= {
    {"CORB Size -CORBSIZE", 2},
    {"Reserved", 2},   
    {"CORB Size Capability -CORBSZCAP", 4},       
    END_DEFINITION
};

RegisterDefinition
RIRBLBASE= {
    {"RIRB Lower Base Unimplemented Bits", 7},
    {"RIRB Lower Base Address - RIRBLBASE", 25},
    END_DEFINITION
};

RegisterDefinition
RIRBWP= {
    {"RIRB Write Pointer -RIRBWP", 8},
    {"Reserved", 7},
    {"RIRB Write Pointer Reset -RIRBWPRST", 1},   	
    END_DEFINITION
};

RegisterDefinition
RINTCNT= {
    {"N Response Interrupt Count -RINTCNT", 8},
    {"Reserved", 8},
    END_DEFINITION
};

RegisterDefinition
RIRBCTL= {
    {"Response Interrupt Control -RINTCTL", 1},
    {"RIRB DMA Enable -RIRBDMAEN", 1},
    {"Response Overrun Interrupt Control -RIRBOIC", 1},
    {"Reserved", 5},   
    END_DEFINITION
};

RegisterDefinition
RIRBSTS= {
    {"Response Interrupt -RINTFL", 1},
    {"Reserved_1", 1},
    {"Response Overrun Interrupt Status -RIRBOIS", 1},
    {"Reserved_2", 5},   
	END_DEFINITION
};

RegisterDefinition
RIRBSIZE= {
    {"RIRB Size -RIRBSIZE", 2},
    {"Reserved", 2},   
    {"RIRB Size Capability -RIRBSZCAP", 4},       
    END_DEFINITION
};

RegisterDefinition
DPLBASE= {
    {"DMA Position Buffer Enable", 1},
    {"DMA Position Lower Base Unimplemented Bits", 6},   
    {"DMA Position Lower Base Address -DPLBASE", 25},       
    END_DEFINITION
};
// for the Input / Output / bidirectional Stream Descriptor Control of the table InputDirectionalStreamDescriptor / OutputDirectionalStreamDescriptor / BiDirectionalStreamDescriptor
RegisterDefinition
SDControl= {
    {"Stream Reset -SRST", 1},
    {"Stream Run -RUN", 1},   
    {"Interrupt On Completion Enable -IOCE", 1},       
    {"FIFO Error Interrupt Enable -FEIE", 1},
    {"Descriptor Error Interrupt Enable -DEIE", 1},   
    {"Reserved", 11},       
    {"Stripe Control -STRIPE", 2},
    {"Traffic Priority TP", 1},       
    {"Bidirectional Direction Control -DIR", 1},
    {"Stream Number -STRM", 4},   
    END_DEFINITION
};
RegisterDefinition
SDControl_1= {
    {"Stream Reset -SRST", 1},
    {"Stream Run -RUN", 1},   
    {"Interrupt On Completion Enable -IOCE", 1},       
    {"FIFO Error Interrupt Enable -FEIE", 1},
    {"Descriptor Error Interrupt Enable -DEIE", 1},   
    {"Reserved", 3},       
    END_DEFINITION
};

RegisterDefinition
SDControl_2= {
    {"Reserved", 8},       
    END_DEFINITION
};

RegisterDefinition
SDControl_3= {
    {"Stripe Control -STRIPE", 2},
    {"Traffic Priority TP", 1},       
    {"Bidirectional Direction Control -DIR", 1},
    {"Stream Number -STRM", 4},   
    END_DEFINITION
};

// for the Input / Output / bidirectional Stream Status of the table InputDirectionalStreamDescriptor / OutputDirectionalStreamDescriptor / BiDirectionalStreamDescriptor
RegisterDefinition
SDStatus = {
    {"Reserved_1", 2},
    {"Buffer Completion Interrupt Status BCIS", 1},   
    {"FIFO Error -FIFOE", 1},       
    {"Descriptor Error -DESE", 1},
    {"FIFO Ready -FIFORDY", 1},   
    {"Reserved_2", 2},       
    END_DEFINITION
};

// for the Input / Output / bidirectional Stream Status of the table InputDirectionalStreamDescriptor / OutputDirectionalStreamDescriptor / BiDirectionalStreamDescriptor
RegisterDefinition
SDLastValidIndex = {
    {"Last Valid Inex", 8},
    {"Reserved_2", 8},       
    END_DEFINITION
};


RegisterDefinition
SDFormat = {
    {"Number of Channels -CHAN", 4},
    {"Bits per Sample -BITS", 3},   
    {"Reserved_1", 1},       
    {"Sample Base Rate Divisor -DIV", 3},
    {"Sample Base Rate Multiple -MULT", 3},   
    {"Sample Base Rate -BASE", 1},       
    {"Reserved_2", 1},
    END_DEFINITION
};

RegisterDefinition
SDLPBA = {
    {"BDLLBASE Lower Base Unimplemented Bits", 7},
    {"Buffer Descriptor List Lower Base Address - BDLLBASE", 25},
    END_DEFINITION
};

RegisterDefinition
Immediate_Command_Status = {
    {"Immediate Command Busy -ICB", 1},
    {"Immediate Result Valid -IRV", 1},
    {"Reserved_1", 1},       
    {"Immediate Response Result Unsolicited -IRRUNSOL", 1},           
    {"Immediate Response Result Address IRRADD", 4},               
    {"Reserved_2", 8},           
    END_DEFINITION
};


