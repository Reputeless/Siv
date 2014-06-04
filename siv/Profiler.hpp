//------------------------------------------
//	Profiler.hpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# pragma once
# define  NOMINMAX
# define  STRICT
# define  WIN32_LEAN_AND_MEAN
# include <Windows.h>
# include "PropertyMacro.hpp"

namespace siv
{
	struct CounterFrequency
	{
		LARGE_INTEGER frequency;

		CounterFrequency()
		{
			::QueryPerformanceFrequency(&frequency);
		}
	};

	inline unsigned long long GetMicrosec()
	{
		const static CounterFrequency f;

		LARGE_INTEGER counter;

		::QueryPerformanceCounter(&counter);

		return counter.QuadPart * 1000000ULL / f.frequency.QuadPart;
	}

	struct MillisecClock
	{
		unsigned long long start = GetMicrosec();

		Property_Get(unsigned long long, elapsed) const
		{
			return (GetMicrosec() - start) / 1000ULL;
		}
	};

	struct MicrosecClock
	{
		unsigned long long start = GetMicrosec();

		Property_Get(unsigned long long, elapsed) const
		{
			return GetMicrosec() - start;
		}
	};

	struct RDTSCClock
	{
		unsigned long long start = ::__rdtsc();

		Property_Get(unsigned long long, elapsed) const
		{
			return ::__rdtsc() - start;
		}
	};
}
