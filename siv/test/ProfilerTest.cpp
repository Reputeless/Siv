//------------------------------------------
//	ProfilerTest.cpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# include <iostream>
# define  NOMINMAX
# define  STRICT
# define  WIN32_LEAN_AND_MEAN
# include <Windows.h>
# include <siv/Profiler.hpp>

int main()
{
	for (int i = 0; i < 5; ++i)
	{
		siv::MillisecClock ms;

		::Sleep(100);

		std::cout << ms.elapsed << "ms\n";
	}

	for (int i = 0; i < 5; ++i)
	{
		siv::MicrosecClock us;

		::Sleep(100);

		std::cout << us.elapsed << "μs\n";
	}

	for (int i = 0; i < 5; ++i)
	{
		siv::RDTSCClock hz;

		::Sleep(100);

		std::cout << hz.elapsed << "Hz\n";
	}
}
