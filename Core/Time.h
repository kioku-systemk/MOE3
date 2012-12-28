//
//  MOE/Time.h
//

#ifndef INCLUDE_MOETIME_H
#define INCLUDE_MOETIME_H

#include "Type.h"

#if MOE_PLATFORM_WINDOWS
#include <windows.h>
#else // other platform
#include <sys/time.h>
#endif

#define USE_LOWPRECISION

namespace MOE {
#if MOE_PLATFORM_WINDOWS

#ifdef USE_LOWPRECISION	
inline f64 GetTimeCount()
{
    return static_cast<f64>(GetTickCount()) * 1e-3;
}

#else

inline f64 GetTimeCount()
{
	// TODO: use performance counter...
	return 0;
}
#endif // MOE_PLATFORM_WINDOWS


// other OS
#else

inline f64 GetTimeCount()
{
    timeval timet;
    gettimeofday(&timet,0);
    return timet.tv_sec + static_cast<f64>(timet.tv_usec) * 1e-6;
}

#endif
} // namespace MOE

#endif // INCLUDE_MOETIME_H

