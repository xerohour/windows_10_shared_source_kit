// Copyright(C) Microsoft.All rights reserved.

// log.h

#define LOG(...) BasicLogPrintf(pLogger, XMSG, 1, __VA_ARGS__)
#define WARN(...) BasicLogPrintf(pLogger, XWARN, 1, __VA_ARGS__)
#define ERR(...) BasicLogPrintf(pLogger, XFAIL, 1, __VA_ARGS__)
