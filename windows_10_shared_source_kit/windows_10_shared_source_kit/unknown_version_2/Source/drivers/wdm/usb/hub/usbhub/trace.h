//
//    Copyright (C) Microsoft.  All rights reserved.
//

#ifdef HUB_WPP


#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(usbhub, (b10d03b8,e1f6,47f5,afc2,0fa0779b8188),\
        WPP_DEFINE_BIT(UsbhDebugTrace)           /* bit  0 = 0x00000001 */ \
        WPP_DEFINE_BIT(UsbhIoctlTrace)           /* bit  1 = 0x00000002 */ \
        WPP_DEFINE_BIT(UsbhPwBootTrace)          /* bit  2 = 0x00000004 */ \
        WPP_DEFINE_BIT(FilterDebugD03)          /* bit  3 = 0x00000008 */ \
        WPP_DEFINE_BIT(FilterDebugD04)          /* bit  4 = 0x00000010 */ \
        WPP_DEFINE_BIT(FilterDebugD05)          /* bit  5 = 0x00000020 */ \
        WPP_DEFINE_BIT(FilterDebugD06)          /* bit  6 = 0x00000040 */ \
        WPP_DEFINE_BIT(FilterDebugD07)          /* bit  7 = 0x00000080 */ \
        WPP_DEFINE_BIT(FilterDebugD08)          /* bit  8 = 0x00000100 */ \
        WPP_DEFINE_BIT(FilterDebugD09)          /* bit  9 = 0x00000200 */ \
        WPP_DEFINE_BIT(FilterDebugD10)          /* bit 10 = 0x00000400 */ \
        WPP_DEFINE_BIT(FilterDebugD11)          /* bit 11 = 0x00000800 */ \
        WPP_DEFINE_BIT(FilterDebugD12)          /* bit 12 = 0x00001000 */ \
        WPP_DEFINE_BIT(FilterDebugD13)          /* bit 13 = 0x00002000 */ \
        WPP_DEFINE_BIT(FilterDebugD14)          /* bit 14 = 0x00004000 */ \
        WPP_DEFINE_BIT(FilterDebugD15)          /* bit 15 = 0x00008000 */ \
        WPP_DEFINE_BIT(FilterDebugD16)          /* bit 16 = 0x00010000 */ \
        WPP_DEFINE_BIT(FilterDebugD17)          /* bit 17 = 0x00020000 */ \
        WPP_DEFINE_BIT(FilterDebugD18)          /* bit 18 = 0x00040000 */ \
        WPP_DEFINE_BIT(FilterDebugD19)          /* bit 19 = 0x00080000 */ \
        WPP_DEFINE_BIT(FilterDebugD20)          /* bit 20 = 0x00100000 */ \
        WPP_DEFINE_BIT(FilterDebugD21)          /* bit 21 = 0x00200000 */ \
        WPP_DEFINE_BIT(FilterDebugD22)          /* bit 22 = 0x00400000 */ \
        WPP_DEFINE_BIT(FilterDebugD23)          /* bit 23 = 0x00800000 */ \
        WPP_DEFINE_BIT(FilterDebugD24)          /* bit 24 = 0x01000000 */ \
        WPP_DEFINE_BIT(FilterDebugD25)          /* bit 25 = 0x02000000 */ \
        WPP_DEFINE_BIT(FilterDebugD26)          /* bit 26 = 0x04000000 */ \
        WPP_DEFINE_BIT(FilterDebugD27)          /* bit 27 = 0x08000000 */ \
        WPP_DEFINE_BIT(FilterDebugD28)          /* bit 28 = 0x10000000 */ \
        WPP_DEFINE_BIT(FilterDebugD29)          /* bit 29 = 0x20000000 */ \
        WPP_DEFINE_BIT(FilterDebugD30)          /* bit 30 = 0x40000000 */ \
        WPP_DEFINE_BIT(FilterDebugD31)          /* bit 31 = 0x80000000 */ \
)

// define the 'xstr' structure for logging buffer and length pairs
// and the 'log_xstr' function which returns it to create one in-place.
// this enables logging of complex data types.
typedef struct xstr { char * _buf; short  _len; } xstr_t;
__inline xstr_t log_xstr( size_t length, void * p) {
    if (length >= MAXSHORT) {
        xstr_t xs;

        xs._buf=  (char*)p;
        xs._len = MAXSHORT;        

        return xs;
    } else {
        xstr_t xs;

        xs._buf=  (char*)p;
        xs._len = (short)length;        

        return xs;
    }
}

// define the macro required for a hexdump
// use as:
//   MrwromTrace(( "%!HEXDUMP!\n", log_xstr(buffersize,(char *)buffer) ));
//
#define WPP_LOGHEXDUMP(x) WPP_LOGPAIR(2, &((x)._len)) WPP_LOGPAIR((x)._len, (x)._buf)

#else
#define WPP_INIT_TRACING(DriverObject, UniRegistryPath)
#define WPP_CLEANUP(DriverObject)

#define  DbgTrace(_x_)
#endif


