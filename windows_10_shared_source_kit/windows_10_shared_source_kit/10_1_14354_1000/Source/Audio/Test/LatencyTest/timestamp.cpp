// timestamp.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"
#include "timestamp.h"

namespace {
const double kTimerGranularity = 10000000.0;
const double kMSToQPC = 10000.0;
}  // namespace

TimeStamp::TimeStamp() {
    QueryPerformanceFrequency(&performance_frequency_);
    granularity_ratio_ =
        kTimerGranularity / performance_frequency_.QuadPart;  // 100ns accuracy
}

TimeStamp::~TimeStamp() {}

double TimeStamp::NSToMS(uint64_t timestamp) {
    return static_cast<double>(timestamp) / kMSToQPC;
}

double TimeStamp::GetTimeStampMS() { return static_cast<double>(GetTimeStampNS()) / kMSToQPC; }

uint64_t TimeStamp::GetTimeStampNS() {
    QueryPerformanceCounter(&current_QPC_);
    return static_cast<uint64_t>(current_QPC_.QuadPart * granularity_ratio_);
}
