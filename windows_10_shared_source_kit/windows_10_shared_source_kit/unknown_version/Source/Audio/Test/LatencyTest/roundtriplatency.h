// roundtriplatency.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef ROUNDTRIP_LATENCY_H
#define ROUNDTRIP_LATENCY_H
#include "timestamp.h"
#include "wasapirender.h"
#include "wasapicapture.h"

class RoundtripLatency {
   public:
    RoundtripLatency(Microsoft::WRL::ComPtr<WASAPICapture> capture,
                     Microsoft::WRL::ComPtr<WASAPIRender> render);
    ~RoundtripLatency();

    void RoundtripLatency::FromCapture(unsigned char* buffer, int buffer_size,
                                       int sampling_rate, int bit_depth,
                                       int channels, uint64_t audio_timestamp,
                                       uint64_t system_timestamp);

    void RoundtripLatency::ToRender(unsigned char* buffer, int buffer_size,
                                    int sampling_rate, int bit_depth,
                                    int channels, uint64_t audio_timestamp,
                                    uint64_t system_timestamp);

    void FromCapture(unsigned char* data,
                     int sampling_rate, int bit_depth, int channels,
                     uint64_t audio_timestamp, uint64_t system_timestamp);
    void ToRender(unsigned char* data,
                  int sampling_rate, int bit_depth, int channels,
                  uint64_t audio_timestamp, uint64_t system_timestamp);
    std::vector<double> GetDelayVector() { return delay_values_; }
    double GetAverageDelay();
    double GetMaxDelay();
    double GetMinDelay();

   private:
    std::unique_ptr<TimeStamp> timer_;
    ComPtr<WASAPICapture> capture_;
    ComPtr<WASAPIRender> render_;
    uint64_t peak_generation_time_;
    uint64_t peak_frame_delay_;
    bool make_peak_;
    int peak_position_;
    float peak_value_;
    std::vector<double> delay_values_;
    
};
#endif  // ROUNDTRIP_LATENCY_H
