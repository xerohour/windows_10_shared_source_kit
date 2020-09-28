#ifdef DXGI_RATIONAL_CMP
#undef DXGI_RATIONAL_CMP
#endif

#define DXGI_RATIONAL_CMP(a, b, c) (((a).Numerator * (c).Denominator) b ((a).Denominator * (c).Numerator))
