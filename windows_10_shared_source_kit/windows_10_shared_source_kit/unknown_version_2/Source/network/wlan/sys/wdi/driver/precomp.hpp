#pragma once

extern "C"
{
#include <ntosp.h>
#include <zwapi.h>
#include <ndis.h>
#include <ndis_p.h>
#include <intrin.h>
#include <windot11_priv.h>
#include <wdf.h>
#include <netiok.h>
#include <netioapi.h>
#include <NdisMiniportHook.h>
#include <stdlib.h>

#include <wmikm.h>
#include <ntddksec.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>
#include <Ntintsafe.h>
#include <ntddsd.h>
#include <windef.h>
#include <kernelprng.h>

#include "dot11wdi.h"
#include "wditypes.hpp"         // Publicly sharable WDI types

#include "wlclient.h"
#include "80211hdr.h"
#include "wsidot11.h"

#include "l2cmn.h"
#include "wlanapi.h"
#include "P2PIEAttrib.h"
#include "WFDSvcANQPParser.h"
#include "xdvexport.h"
}


// Only public headers should be included before TlvGeneratorParser.hpp
// Because it is also a public header file and should not have
// any private dependencies
#define _WDI_WIFI_SYS // Pickup the GenerateToIhv/ParseFromIhv APIs by default
#include "TlvGeneratorParser.hpp"

// Private WDI types
#include "wdiprivate.hpp"

#include "wdilib.hpp"
#include "wdilibwpp.hpp"

// Include TraceLogging APIs for telemetry gathering
#ifndef WDI_WIN81_COMPLIANT
#include <TraceLoggingProvider.h>
#include "winmeta.h"
#include <telemetry\MicrosoftTelemetry.h>
#endif

#include "CommonTypes.hpp"
#include "CppUtil.hpp"
#include "Interfaces.hpp"
#include "CtlPlane.hpp"
#include "PropertyCache.hpp"
#include "Event.hpp"
#include "lock.hpp"
#include "Timer.hpp"
#include "EventQueue.hpp"
#include "NotificationManager.hpp"
#include "DeviceCommand.hpp"
#include "DeviceCommandScheduler.hpp"
#include "Task.hpp"
#include "Job.hpp"
#include "ActiveJobsList.hpp"
#include "SerializedJobsList.hpp"
#include "registry.hpp"
#include "bsslist.hpp"
#include "PeerList.hpp"
#include "ActionFrame.hpp"
#include "NetworkHistory.hpp"

#include "Request.hpp"
#include "Dot11Oids.hpp"
#include "NdisOids.hpp"
#include "WabiUtil.hpp"
#include "Dot11Utils.hpp"
#include "scan.hpp"

#include "datapath.hpp"
#include "rxfrag.hpp"
#include "rxmgr.hpp"
#include "txmgr.hpp"

#include "FrameworkHelpers.hpp"
#include "NdisHook.hpp"
#include "driver.hpp"
#include "miniport.hpp"
#include "adapter.hpp"
#include "WfdDevice.hpp"
#include "Connect.hpp"
#include "Port.hpp"

#include "WfdCommon.hpp"
#include "WfdClient.hpp"
#include "WfdGroupOwner.hpp"

#include "Reset.hpp"
#include "PnP.hpp"
#include "radio.hpp"
#include "RandomMacForScan.hpp"

#include "ANQPQuery.hpp"
#include "Offload.hpp"
#include "wake.hpp"
#include "coalesce.hpp"
#include "resume.hpp"
#include "Power.hpp"
