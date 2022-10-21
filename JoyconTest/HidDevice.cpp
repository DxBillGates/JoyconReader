#include "HidDevice.h"

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "SetupAPI.lib")

GUID* GE::GetHidGuid()
{
	// このPCのGUIDを取得
	static GUID guid;
	static bool isOnceTime = true;

	if (isOnceTime == true)
	{
		HidD_GetHidGuid(&guid);
	}
	return &guid;
}

GE::HidDevice* GE::GetNewHidDevice(DWORD vendorID, DWORD productID)
{
	// hidデバイス情報のハンドルを取得
	HDEVINFO devinfo;
	// DIGCF_PRESENT : 接続されているHIDデバイスだけ
	// DIGCF_DEVICEINTERFACE : ?
	auto pHidGuid = GetHidGuid();
	devinfo = SetupDiGetClassDevs(pHidGuid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	// 接続されているHIDデバイスを一つずつ取得
	SP_DEVICE_INTERFACE_DATA spid;
	spid.cbSize = sizeof(spid);

	HidDevice* hidDevice = new HidDevice();
	hidDevice->overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	bool isFind = false;

	for (int index = 0; ; ++index)
	{
		// 取得できなくなったらループを終わる
		if (!SetupDiEnumDeviceInterfaces(devinfo, NULL, pHidGuid, index, &spid))
		{
			break;
		}

		// 接続されているHIDデバイスの情報があるであろうアドレスを取得?
		unsigned long size;
		SetupDiGetDeviceInterfaceDetail(devinfo, &spid, NULL, 0, &size, 0);
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetailData = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[size]);
		deviceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SetupDiGetDeviceInterfaceDetail(devinfo, &spid, deviceDetailData, size, &size, 0);

		// HIDデバイスにアクセスするために必要なハンドルの作成
		hidDevice->handle = CreateFile(deviceDetailData->DevicePath
			, GENERIC_READ | GENERIC_WRITE
			, FILE_SHARE_READ | FILE_SHARE_WRITE
			, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		// HIDに対して入出力ができないデバイスはハンドルを作れない?
		if (hidDevice->handle == INVALID_HANDLE_VALUE)continue;

		// 警告邪魔だからif文で戻り値を使用
		if (!HidD_GetAttributes(hidDevice->handle, &hidDevice->hiddAttribute))continue;

		if (hidDevice->hiddAttribute.VendorID == vendorID && hidDevice->hiddAttribute.ProductID == productID)
		{
			isFind = true;
			break;
		}
		else
		{
			CloseHandle(hidDevice->handle);
			hidDevice->handle = nullptr;
			continue;
		}
	}

	// 目的のHIDデバイスが見つからなかったら確保したメモリはすべて削除
	if (isFind == false)
	{
		DeleteHidDevice(hidDevice);
		return nullptr;
	}

	PHIDP_PREPARSED_DATA pp_data = NULL;

	// HIDデバイスの使用バイト数などを取得
	bool isPreparsed = HidD_GetPreparsedData(hidDevice->handle, &pp_data);
	bool isGetCaps = HidP_GetCaps(pp_data, &hidDevice->hidpCaps);

	// インプットバッファーをどれだけ作れるかを取得して最大値を設定
	ULONG numReport = 0;
	bool isGetNumInputBuffer = HidD_GetNumInputBuffers(hidDevice->handle, &numReport);
	bool isSetNumInputBuffer = HidD_SetNumInputBuffers(hidDevice->handle, numReport);

	hidDevice->readBufferSize = hidDevice->hidpCaps.InputReportByteLength;
	hidDevice->writeBufferSize = hidDevice->hidpCaps.OutputReportByteLength;

	return hidDevice;
}

void GE::DeleteHidDevice(HidDevice* hidDevice)
{
	delete hidDevice;
	hidDevice = nullptr;
}
