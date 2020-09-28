// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  limits.h
//
// Abstract:
//
//  defines the tolerances for WHQL failures for this test app
//



// -------------------------------------------------------------------------------

#ifndef _WHQLLIMITS_H_85A355CA_5018_4F6B_84DB_43AC4B166542_
#define _WHQLLIMITS_H_85A355CA_5018_4F6B_84DB_43AC4B166542_

//  audio devices are required to be SAMPLEACCURATE (PC9x HW Design Guide)..  
//  but for some reason we are making them ms accurate??
#define JITTER_STD_THRESHOLD        1.0       // 1 ms max allowed for std-deviation
#define JITTER_MAX_THRESHOLD        4.0       // 1 ms max allowed for max deviation


//  Real time sampling rates must be within 1% of the theoretical rate,
//  per the format, as required by the PC98/PC99 HW Design Guide.
#define SAMPLERATE_THRESHOLD        1.0

// by the same token, it should take within 1% of the expected time to complete
// playing a buffer
#define DURATION_THRESHOLD          1.0


// max latency for rendering or capturing is 15 ms
#define LATENCY_THRESHOLD           15.
#define LATENCY_THRESHOLD_SYSAUDIO  100.

// max time allowed for KSPROPERTY_AUDIO_POSITION call
#define GETPOS_THRESHOLD            1.00
#define GETPOS_THRESHOLD_SYSAUDIO   1.10

// max difference between a successive SetPosition and GetPosition call
#define POS_ERR_TOLERANCE_MS 2.

#endif
