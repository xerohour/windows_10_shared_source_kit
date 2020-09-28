// log.h
//
// Copyright (C) Microsoft. All rights reserved.

#define LOG(...) Log::Comment(String().Format(__VA_ARGS__))
#define ERR(...) Log::Comment(String().Format(__VA_ARGS__))