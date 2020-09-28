// roundtriplatency.cpp
//
// Copyright (C) Microsoft. All rights reserved.

#include "pch.h"
#include "roundtriplatency.h"

// Callback to render
void RoundtripLatency::ToRender(unsigned char* data, int available_frames,
                                int, int bit_depth, int channels,
                                uint64_t,
                                uint64_t) {

    // set buffer to 0
    int frame_size_in_bytes = available_frames * channels * (bit_depth / 8);
    memset(data, 0, frame_size_in_bytes);

    // if we should render a peak
    if (make_peak_ == true) {
        make_peak_ = false;
        // ||--------------------------|
        // ^^                          ^ 
        // ||Unit sample               |
        // |Start Frame                | 

        // render a unit sample
        if (bit_depth == 16) {
            short* samples = reinterpret_cast<short*>(data);
            samples[0] = INT16_MAX;
            // if stereo then send out on other channel as well
            if (channels > 1) {
                samples[1] = INT16_MAX;
            }
            // if more than 2 channels we just don't render on them
        }
        if (bit_depth == 32) {
            float* samples = reinterpret_cast<float*>(data);
            samples[0] = 1.f;
            // if stereo then send out on other channel as well
            if (channels > 1) {
                samples[1] = 1.f;
            }
            // if more than 2 channels we just don't render on them
        }
        peak_generation_time_ = timer_->GetTimeStampNS();
    }
}

// Callback to capture
void RoundtripLatency::FromCapture(unsigned char* data, int available_frames,
                                   int sampling_rate, int bit_depth,
                                   int channels, uint64_t,
                                   uint64_t event_timestamp) {
    uint64_t time_since_peak_generation_ = timer_->GetTimeStampNS() - peak_generation_time_;

    // first time
    if (peak_generation_time_ == 0) {
        make_peak_ = true;
    }

    // if peak has been rendered and time since generation is less than 2 ms
    if (timer_->NSToMS(time_since_peak_generation_) > 2 && make_peak_ == false) {

        float* float_samples = reinterpret_cast<float*>(data);
        short* short_samples = reinterpret_cast<short*>(data);
        float delta = 0;

        for (int i = channels; i < available_frames * channels; i++) {

            // diff
            if (bit_depth == 16) {
                delta = static_cast<short>(short_samples[i] - short_samples[i - channels]);
            } else if (bit_depth == 32) {
                delta = float_samples[i] - float_samples[i - channels];
            } else {
                break;
            }

            // abs
            if (delta < 0) {
                delta -= delta;
            }

            // save max, in frame position and frame time
            if (delta > peak_value_) {
                peak_value_ = delta;
                peak_position_ = i;
                peak_frame_delay_ = event_timestamp - peak_generation_time_;
            }
        }
    }

    // if one second has passed then identify the peak and generate new
    if (timer_->NSToMS(time_since_peak_generation_) > 1000.0 && make_peak_ == false) {
        // calculate where in the frame the peak is located in 100ns
        uint64_t peak_adjustment_time_ = static_cast<uint64_t>(1e7 * ((static_cast<double>(peak_position_ / channels)) / sampling_rate));
        // add the peak_adjustment_time to the peak_frame_delay.
        double delay = timer_->NSToMS(peak_frame_delay_ + peak_adjustment_time_);

        delay_values_.push_back(delay);
        make_peak_ = true;
        peak_value_ = 0;
    }
}

double RoundtripLatency::GetAverageDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double mean = 0;
    for (auto& i : delay_values_) {
        mean += i;
    }
    mean /= delay_values_.size();
    return mean;
};

double RoundtripLatency::GetMaxDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double max = 0;
    for (auto& i : delay_values_) {
        if (i > max) {
            max = i;
        }
    }
    return max;
};

double RoundtripLatency::GetMinDelay() {
    if (delay_values_.size() == 0) {
        return 0;
    }
    double min = 999;
    for (auto& i : delay_values_) {
        if (i < min) {
            min = i;
        }
    }
    return min;
};

RoundtripLatency::RoundtripLatency(
    Microsoft::WRL::ComPtr<WASAPICapture> capture,
    Microsoft::WRL::ComPtr<WASAPIRender> render)
    : capture_(capture), render_(render), make_peak_(false),
    peak_generation_time_(0), peak_frame_delay_(0),
    peak_value_(0), delay_values_(0) {
    timer_ = std::make_unique<TimeStamp>();
    // register for buffer events
    capture_->AddCallback(
        "RoundtripLatency",
        [this](unsigned char* buffer, int buffer_size, int sampling_rate,
               int bit_depth, int channels, uint64_t audio_timestamp,
               uint64_t system_timestamp) {
            FromCapture(buffer, buffer_size, sampling_rate, bit_depth, channels,
                        audio_timestamp, system_timestamp);
        });
    render_->AddCallback(
        "RoundtripLatency",
        [this](unsigned char* buffer, int buffer_size, int sampling_rate,
               int bit_depth, int channels, uint64_t audio_timestamp,
               uint64_t system_timestamp) {
            ToRender(buffer, buffer_size, sampling_rate, bit_depth, channels,
                     audio_timestamp, system_timestamp);
        });
}

RoundtripLatency::~RoundtripLatency() {
    // remove yourself from buffer events
    capture_->RemoveCallback("RoundtripLatency");
    render_->RemoveCallback("RoundtripLatency");
}
