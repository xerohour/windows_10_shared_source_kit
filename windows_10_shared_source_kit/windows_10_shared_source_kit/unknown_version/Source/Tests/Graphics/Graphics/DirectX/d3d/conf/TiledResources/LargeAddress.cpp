////////////////////////////////////////////////////////////////
// LargeAddress.cpp
////////////////////////////////////////////////////////////////

#include "LargeAddress.h"

using namespace WindowsTest;

BEGIN_NAMED_VALUES(ADDRESS_SPACE_USAGE)
    NAMED_VALUE(_T("64M"), ADDRESS_SPACE_USAGE_64M)
    NAMED_VALUE(_T("128M"), ADDRESS_SPACE_USAGE_128M)
    NAMED_VALUE(_T("1G"), ADDRESS_SPACE_USAGE_1G)
    NAMED_VALUE(_T("2G"), ADDRESS_SPACE_USAGE_2G)
    NAMED_VALUE(_T("4G"), ADDRESS_SPACE_USAGE_4G)
    NAMED_VALUE(_T("64G"), ADDRESS_SPACE_USAGE_64G)
    NAMED_VALUE(_T("128G"), ADDRESS_SPACE_USAGE_128G)
    NAMED_VALUE(_T("256G"), ADDRESS_SPACE_USAGE_256G)
    NAMED_VALUE(_T("512G"), ADDRESS_SPACE_USAGE_512G)
    NAMED_VALUE(_T("920G"), ADDRESS_SPACE_USAGE_920G)
END_NAMED_VALUES(ADDRESS_SPACE_USAGE)
