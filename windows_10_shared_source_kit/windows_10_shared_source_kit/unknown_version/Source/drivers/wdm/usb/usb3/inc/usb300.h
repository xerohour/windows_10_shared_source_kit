//
//    Copyright (C) Microsoft.  All rights reserved.
//
//
// Temporary definations moved from usbspec.w to avoid a collision.
//
typedef enum _USB_HUB_FEATURE_SELECTOR {
    C_HUB_LOCAL_POWER       = 0,
    C_HUB_OVER_CURRENT      = 1
} USB_HUB_FEATURE_SELECTOR, *PUSB_HUB_FEATURE_SELECTOR;

typedef enum _USB_PORT_FEATURE_SELECTOR {
    PORT_CONNECTION         = 0,
    PORT_ENABLE             = 1,
    PORT_SUSPEND            = 2,
    PORT_OVER_CURRENT       = 3,
    PORT_RESET              = 4,
    PORT_LINK_STATE         = 5,
    PORT_POWER              = 8,
    PORT_LOW_SPEED          = 9,
    C_PORT_CONNECTION       = 16,
    C_PORT_ENABLE           = 17,
    C_PORT_SUSPEND          = 18,
    C_PORT_OVER_CURRENT     = 19,
    C_PORT_RESET            = 20,
    PORT_TEST               = 21,
    PORT_INDICATOR          = 22,
    PORT_U1_TIMEOUT         = 23,
    PORT_U2_TIMEOUT         = 24,
    C_PORT_LINK_STATE       = 25,
    C_PORT_CONFIG_ERROR     = 26,
    PORT_REMOTE_WAKE_MASK   = 27,
    BH_PORT_RESET           = 28,
    C_BH_PORT_RESET         = 29,
    FORCE_LINKPM_ACCEPT     = 30
} USB_PORT_FEATURE_SELECTOR, *PUSB_PORT_FEATURE_SELECTOR;


