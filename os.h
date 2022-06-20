#pragma once

#define OS_WINDOWS 0
#define OS_UNKNOWN 1

#ifdef WIN32
#define OPERATING_SYSTEM OS_WINDOWS
#else
#define OPERATING_SYSTEM OS_UNKNOWN
#endif
