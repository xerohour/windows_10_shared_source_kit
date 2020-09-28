struct VSOut
{
	float4 pos   : SV_POSITION;
	float4 color : CLR;
};

struct PSOut
{
	float4 color : SV_TARGET0;
	
#ifdef PS_OUTPUT_DEPTH
	float  depth : SV_DEPTH;
#endif
};

RWByteAddressBuffer RBuff : register(u1);

struct Record
{
	DATATYPE x;
	DATATYPE y;
};
RWStructuredBuffer<Record> SBuff : register(u1);

AppendStructuredBuffer<Record> ABuff : register(u1);

RWBuffer<DATATYPE>    TypedBuff : register(u1);
RWTexture1D<DATATYPE> TypedTexture1D : register(u1);
RWTexture2D<DATATYPE> TypedTexture2D : register(u1);
RWTexture3D<DATATYPE> TypedTexture3D : register(u1);

#ifdef FORCE_EARLY_DEPTH
[earlydepthstencil]
#endif

#ifdef USE_RAW_BUFFER

#ifdef OP_STORE
#define DO_OP(Addr, DynamicValue) RBuff.Store(Addr, DynamicValue)
#endif

#ifdef OP_ATOMIC_AND
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedAnd(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_AND
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedAnd(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_XOR
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedXor(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_XOR
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedXor(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_OR
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedOr(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_OR
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedOr(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_CMP_STORE
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedCompareStore(Addr, 0, DynamicValue)
#endif

#ifdef OP_ATOMIC_EXCHANGE
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedExchange(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_CMP_EXCHANGE
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedCompareExchange(Addr, 0, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_ADD
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedAdd(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_ADD
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedAdd(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_MAX
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedMax(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_MAX
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedMax(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_MIN
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedMin(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_MIN
#define DO_OP(Addr, DynamicValue) RBuff.InterlockedMin(Addr, DynamicValue, OriginalValue)
#endif


#else // USE_RAW_BUFFER

#ifdef OP_STORE
#define DO_OP(Addr, DynamicValue) Addr = DynamicValue
#endif

#ifdef OP_ATOMIC_AND
#define DO_OP(Addr, DynamicValue) InterlockedAnd(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_AND
#define DO_OP(Addr, DynamicValue) InterlockedAnd(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_XOR
#define DO_OP(Addr, DynamicValue) InterlockedXor(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_XOR
#define DO_OP(Addr, DynamicValue) InterlockedXor(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_OR
#define DO_OP(Addr, DynamicValue) InterlockedOr(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_OR
#define DO_OP(Addr, DynamicValue) InterlockedOr(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_CMP_STORE
#define DO_OP(Addr, DynamicValue) InterlockedCompareStore(Addr, 0, DynamicValue)
#endif

#ifdef OP_ATOMIC_EXCHANGE
#define DO_OP(Addr, DynamicValue) InterlockedExchange(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_CMP_EXCHANGE
#define DO_OP(Addr, DynamicValue) InterlockedCompareExchange(Addr, 0, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_ADD
#define DO_OP(Addr, DynamicValue) InterlockedAdd(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_ADD
#define DO_OP(Addr, DynamicValue) InterlockedAdd(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_MAX
#define DO_OP(Addr, DynamicValue) InterlockedMax(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_MAX
#define DO_OP(Addr, DynamicValue) InterlockedMax(Addr, DynamicValue, OriginalValue)
#endif

#ifdef OP_ATOMIC_MIN
#define DO_OP(Addr, DynamicValue) InterlockedMin(Addr, DynamicValue)
#endif

#ifdef OP_IMM_ATOMIC_MIN
#define DO_OP(Addr, DynamicValue) InterlockedMin(Addr, DynamicValue, OriginalValue)
#endif

#endif // USE_RAW_BUFFER

PSOut PSEntry(VSOut input, uint sampleIndex : SV_SAMPLEINDEX)
{
	PSOut result;
	result.color = input.color;
	
#ifdef PS_OUTPUT_DEPTH
	result.depth = 0.5f;
#endif
	
#ifdef DISCARD_BEFORE_WRITE
	if(input.pos.x > 1.0f)
		discard;
#endif

	uint OriginalValue = 0;

	uint2 uPos = (uint2)(input.pos - float2(0.5f, 0.5f));
	uint sampleCount = GetRenderTargetSampleCount();
	
	uint storeIndex = (uPos.x * sampleCount) + sampleIndex;

// Only do the store for the top row of pixels
	if ( 0 == uPos.y )
	{
	#ifdef USE_RAW_BUFFER
		DO_OP((storeIndex * 8),   (DATATYPE)uPos.x+1);
		DO_OP((storeIndex * 8)+4, (DATATYPE)uPos.y+1);
	#endif

	#ifdef USE_STRUCTURED_BUFFER
		DO_OP(SBuff[storeIndex].x, (DATATYPE)uPos.x+1);
		DO_OP(SBuff[storeIndex].y, (DATATYPE)uPos.y+1);
	#endif

		uint4 storeIndex4_0 = uint4(storeIndex*2, 0, 0, 0); // ensure that all elements of coord except for 1st are 0
		uint4 storeIndex4_1 = storeIndex4_0 + uint4(1,0,0,0);
		
	#ifdef USE_TYPED_BUFFER
		DO_OP(TypedBuff[storeIndex4_0.x], (DATATYPE)uPos.x+1);
		DO_OP(TypedBuff[storeIndex4_1.x], (DATATYPE)uPos.y+1);
	#endif
	
	#ifdef USE_TYPED_TEXTURE1D
		DO_OP(TypedTexture1D[storeIndex4_0.x], (DATATYPE)uPos.x+1);
		DO_OP(TypedTexture1D[storeIndex4_1.x], (DATATYPE)uPos.y+1);
	#endif
	
	#ifdef USE_TYPED_TEXTURE2D
		DO_OP(TypedTexture2D[storeIndex4_0.xy], (DATATYPE)uPos.x+1);
		DO_OP(TypedTexture2D[storeIndex4_1.xy], (DATATYPE)uPos.y+1);
	#endif
	
	#ifdef USE_TYPED_TEXTURE3D
		DO_OP(TypedTexture3D[storeIndex4_0.xyz], (DATATYPE)uPos.x+1);
		DO_OP(TypedTexture3D[storeIndex4_1.xyz], (DATATYPE)uPos.y+1);
	#endif

	#ifdef USE_APPEND_BUFFER
		Record r;
		r.x = (DATATYPE)uPos.x+1;
		r.y = (DATATYPE)sampleIndex+1; // ensure sample frequency shader execution
		ABuff.Append(r);
	#endif
	
	#ifdef USE_COUNTER_BUFFER
		uint target = SBuff.IncrementCounter();
		DO_OP(SBuff[target].x, (DATATYPE)uPos.x+1);
		DO_OP(SBuff[target].y, (DATATYPE)sampleIndex+1); // ensure sample frequency shader execution
	#endif
	}

#ifdef DISCARD_AFTER_WRITE
	if(input.pos.x > 1.0f)
		discard;
#endif

	// use OriginalValue so that the compiler won't optimize it out
	result.color.a += (OriginalValue * 0.00001f);
		
	return result;
}