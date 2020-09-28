#include "precomp.hpp"


//
// Handy macro to declare types to be made public
//
#define DECLARE_TYPE(Name)  Name _DECL_##Name
#define DECLARE_TYPE_1(Name)  Name _DECL_##Name(NULL)

DECLARE_TYPE_1(CAdapter);
DECLARE_TYPE(CMiniportDriver);
DECLARE_TYPE(CPort);
DECLARE_TYPE(CCtlPlane);
DECLARE_TYPE(CGlobalCtlDevice);
DECLARE_TYPE(EventQueue);
DECLARE_TYPE(SerializedJobList);
DECLARE_TYPE(ActiveJobsList);
DECLARE_TYPE(NotificationManager);
DECLARE_TYPE(DeviceCommandScheduler);


//
// Make it build
//
int __cdecl main() {
    return 0;
}

