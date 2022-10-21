#pragma once
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>

namespace GE
{
	struct HidDevice
	{
		HANDLE handle;
		HIDD_ATTRIBUTES hiddAttribute;
		HIDP_CAPS hidpCaps;

		BYTE* readBuffer;
		DWORD readBufferSize;

		BYTE* writeBuffer;
		DWORD writeBufferSize;

		OVERLAPPED overlapped;

		bool isBlocking;

		HidDevice()
			: handle(nullptr)
			, hiddAttribute()
			, hidpCaps()
			, readBuffer(nullptr)
			, readBufferSize(0)
			, writeBuffer(nullptr)
			, writeBufferSize(0)
			, overlapped()
			, isBlocking(false)
		{
		}

		~HidDevice()
		{
			if (handle)CloseHandle(handle);
			if (readBuffer)delete[] readBuffer;
			if (writeBuffer)delete[] writeBuffer;
			if (overlapped.hEvent)CloseHandle(overlapped.hEvent);
		}
	};

	/// <summary>
	/// PCのGUIDを取得
	/// </summary>
	/// <returns>PCのGUID</returns>
	static GUID* GetHidGuid();

	/// <summary>
	/// HIDのベンダーIDとプロダクトIDを指定し、接続されていれば取得
	/// </summary>
	/// <param name="vendorID">取得したいデバイスのベンダーID</param>
	/// <param name="productID">取得したいデバイスのプロダクトID</param>
	/// <returns>接続されていてベンダーIDとプロダクトIDが一致していればHIDデバイスの情報をポインタで返す</returns>
	static HidDevice* GetNewHidDevice(DWORD vendorID, DWORD productID);

	static void DeleteHidDevice(HidDevice* hidDevice);
}
