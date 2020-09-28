// log.h

extern IBasicLog *g_pLog;

#define LOG(...)  BasicLogPrintf(g_pLog, XMSG,  1, __VA_ARGS__)
#define WARN(...) BasicLogPrintf(g_pLog, XWARN, 1, __VA_ARGS__)
#define ERR(...)  BasicLogPrintf(g_pLog, XFAIL, 1, __VA_ARGS__)

