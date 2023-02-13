#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "OSVersion.h"

std::string getOSVersion()
{
	LPCSTR szVersionFile = "kernel32.dll";
	DWORD  verHandle = 0;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(szVersionFile, &verHandle);
	std::string a;

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(szVersionFile, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, "\\", (VOID FAR * FAR*) & lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{

						// Doesn't matter if you are on 32 bit or 64 bit,
						// DWORD is always 32 bits, so first two revision numbers
						// come from dwFileVersionMS, last two come from dwFileVersionLS

						a = "File Version: ";
						a += (verInfo->dwFileVersionMS >> 16) & 0xffff;
						a += (verInfo->dwFileVersionMS >> 0) & 0xffff;
						a += (verInfo->dwFileVersionLS >> 16) & 0xffff;
						a += (verInfo->dwFileVersionLS >> 0) & 0xffff;
						return a;
					}
				}
			}
		}
		delete[] verData;
	}
	return "";
}
#endif

#ifdef __linux__
#include <sys/utsname.h>
struct utsname {
	char sysname[];  // имя операционной системы 
	char nodename[]; // имя узла сети 
	char release[];  // номер выпуска операционной системы 
	char version[];  // номер версии этого выпуска 
	char machine[];  // тип аппаратной архитектуры 
};
#endif