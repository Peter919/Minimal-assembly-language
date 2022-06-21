#pragma once

#define OS_WINDOWS 0
#define OS_UNIX 1
#define OS_UNKNOWN 2

#ifdef WIN32
#define OPERATING_SYSTEM OS_WINDOWS
#elif unix
#define OPERATING_SYSTEM OS_UNIX
#else
#define OPERATING_SYSTEM OS_UNKNOWN
#endif
