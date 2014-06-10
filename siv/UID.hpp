//------------------------------------------
//	UID.hpp
//	Copyright (c) 2014 Reputeless
//	<reputeless@gmail.com>
//	Distributed under the MIT license.
//------------------------------------------

# pragma comment(lib, "IPHLPAPI")
# include <filesystem>
# include <sstream>
# include <iomanip>
# define NOMINMAX
# define STRICT
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
# include <Sddl.h>
# include <iphlpapi.h>
# include <siv/Optional.hpp>

namespace siv
{
	inline optional<unsigned> GetVolumeSerial()
	{
		const unsigned pathLength = MAX_PATH;
		wchar_t directory[pathLength];

		if (!::GetWindowsDirectoryW(directory, pathLength))
		{
			return nullopt;
		}

		namespace fs = std::tr2::sys;
		const std::wstring rootPath = fs::system_complete(fs::wpath(directory)).parent_path();

		wchar_t volume[pathLength];
		wchar_t system[pathLength];
		DWORD serial = 0, length = 0, flags = 0;

		if (!::GetVolumeInformationW(rootPath.c_str(), volume, pathLength, &serial, &length, &flags, system, pathLength))
		{
			return nullopt;
		}

		return serial;
	}

	inline optional<std::wstring> GetMacAddress()
	{
		ULONG bufferLength = 0;

		::GetAdaptersInfo(nullptr, &bufferLength);

		auto buffer = std::make_unique<unsigned char[]>(bufferLength);

		PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO) buffer.get();

		if (::GetAdaptersInfo(pAdapterInfo, &bufferLength) != ERROR_SUCCESS)
		{
			return nullopt;
		}

		while (pAdapterInfo)
		{
			if (pAdapterInfo->AddressLength)
			{
				std::wstringstream ss;

				ss << std::setfill(L'0') << std::hex << std::uppercase;

				for (unsigned i = 0; i < pAdapterInfo->AddressLength; ++i)
				{
					ss << std::setw(2) << static_cast<unsigned>(pAdapterInfo->Address[i]);
				}

				return ss.str();
			}

			pAdapterInfo = pAdapterInfo->Next;
		}

		return nullopt;
	}

	inline bool ConvertNameToSid(const wchar_t* name, PSID *ppSid)
	{
		wchar_t domainName[256];
		DWORD domainNameLength = _countof(domainName);
		DWORD sidSize = 0;
		SID_NAME_USE sidName;

		::LookupAccountNameW(nullptr, name, nullptr, &sidSize, domainName, &domainNameLength, &sidName);

		*ppSid = (PSID) ::LocalAlloc(LPTR, sidSize);

		return !!::LookupAccountNameW(nullptr, name, *ppSid, &sidSize, domainName, &domainNameLength, &sidName);
	}

	inline optional<std::wstring> GetSID(bool useUserName = false)
	{
		wchar_t name[256];
		DWORD nameLength = _countof(name);

		if (useUserName)
		{
			::GetUserNameW(name, &nameLength);
		}
		else
		{
			::GetComputerNameW(name, &nameLength);
		}

		std::wstring sid;

		{
			PSID pSidAccount;

			ConvertNameToSid(name, &pSidAccount);

			{
				LPTSTR pStringSid;

				if (::ConvertSidToStringSidW(pSidAccount, &pStringSid))
				{
					sid = pStringSid;
				}

				::LocalFree(pStringSid);
			}

			::LocalFree(pSidAccount);
		}

		if (sid.empty())
		{
			return nullopt;
		}

		return sid;
	}

	inline optional<std::wstring> GetMachineGUID()
	{
		HKEY key;

		if (::RegOpenKeyExW(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Cryptography)",
			0, KEY_READ | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS)
		{
			return nullopt;
		}

		DWORD type = REG_SZ, size;

		if (::RegQueryValueExW(key, L"MachineGuid", nullptr, &type, nullptr, &size) != ERROR_SUCCESS || size < 2)
		{
			return nullopt;
		}

		std::wstring str;
		str.resize(size / sizeof(wchar_t) - 1);

		optional<std::wstring> result;

		if (::RegQueryValueExW(key, L"MachineGuid", nullptr, &type, (LPBYTE) str.data(), &size) == ERROR_SUCCESS)
		{
			result = str;
		}

		::RegCloseKey(key);

		return result;
	}
}
