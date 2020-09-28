// timestamp.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <windows.h>

class TimeStamp {
   public:
    TimeStamp();
    ~TimeStamp();
    uint64_t GetTimeStampNS();
    double GetTimeStampMS();
    double NSToMS(uint64_t timestamp);

   private:
    LARGE_INTEGER current_QPC_;
    LARGE_INTEGER performance_frequency_;
    double granularity_ratio_;
};
#endif  // TIMESTAMP_H_
