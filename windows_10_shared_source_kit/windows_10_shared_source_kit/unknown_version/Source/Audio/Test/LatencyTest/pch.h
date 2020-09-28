// pch.h
//
// Copyright (C) Microsoft. All rights reserved.

#ifndef PCH_H_
#define PCH_H_
#undef NOMINMAX
#define NOMINMAX  // define to handle STL min/max
#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <string>
#include <cstdint>
#include <hstring.h>
#include <WexTestClass.h>
#include <audioclient.h>
#include <windows.foundation.h>
#include <windows.media.capture.h>
#include <windows.media.effects.h>
#include <windows.media.devices.h>
#include <windows.media.render.h>
#include <windows.devices.enumeration.h>
#include <hstring.h>
#include "log.h"
#include "options.h"
#include "device.h"
#include "wasapirender.h"
#include "wasapicapture.h"
#include "latencytest.h"
#include "roundtriplatency.h"
#include "categories.h"

class WindowsDeleteStringOnExit {
public:
    WindowsDeleteStringOnExit(HSTRING s) : s_(s) {}
    ~WindowsDeleteStringOnExit() {
        if (s_ != nullptr) {
            WindowsDeleteString(s_);
        }
    }
private:
    HSTRING s_;
};

#endif  // PCH_H_
