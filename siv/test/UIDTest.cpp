//------------------------------------------
//	UIDTest.cpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# include <iostream>
# include <siv/UID.hpp>

int main()
{
	if (const auto volumeSerial = siv::GetVolumeSerial())
	{
		std::wcout << L"Volume Serial\t: " << volumeSerial.value() << L'\n';
	}

	if (const auto macAddress = siv::GetMacAddress())
	{
		std::wcout << L"MAC Address\t: " << macAddress.value() << L'\n';
	}

	if (const auto sid = siv::GetSID())
	{
		std::wcout << L"Computer SID\t: " << sid.value() << L'\n';
	}

	if (const auto sid = siv::GetSID(true))
	{
		std::wcout << L"User SID\t: " << sid.value() << L'\n';
	}

	if (const auto machineGUID = siv::GetMachineGUID())
	{
		std::wcout << L"Machine GUID\t: " << machineGUID.value() << L'\n';
	}
}
