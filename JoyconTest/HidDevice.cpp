#include "HidDevice.h"

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "SetupAPI.lib")

GUID* GE::HID::GetHidGuid()
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

GE::HID::HidDevice* GE::HID::GetNewHidDevice(DWORD vendorID, DWORD productID)
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
	hidDevice->readOverlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

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
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetailData = new SP_INTERFACE_DEVICE_DETAIL_DATA[size];
		deviceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SetupDiGetDeviceInterfaceDetail(devinfo, &spid, deviceDetailData, size, &size, 0);

		// HIDデバイスにアクセスするために必要なハンドルの作成
		hidDevice->handle = CreateFile(deviceDetailData->DevicePath
			, GENERIC_READ | GENERIC_WRITE
			, FILE_SHARE_READ | FILE_SHARE_WRITE
			, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		// 確保したメモリを開放
		delete[] deviceDetailData;

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

	hidDevice->readBuffer = new BYTE[hidDevice->readBufferSize];
	hidDevice->writeBuffer = new BYTE[hidDevice->writeBufferSize];

	return hidDevice;
}

void GE::HID::GetInputReport(HidDevice* pHidDevice)
{
	if (pHidDevice == nullptr)return;

	int milisec = pHidDevice->isBlocking ? -1 : 0;
	bool result = false;
	bool isOverlapped = false;

	// 読み込み中かどうか
	// 読み込み終わってるならデバイスからデータを取得
	if (pHidDevice->isReadPending == false)
	{
		pHidDevice->isReadPending = true;
		ResetEvent(pHidDevice->readOverlapped.hEvent);
		DWORD resultReadBufferSize = 0;
		result = ReadFile(pHidDevice->handle, pHidDevice->readBuffer, pHidDevice->readBufferSize, &resultReadBufferSize, &pHidDevice->readOverlapped);
		if (result == false)
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				isOverlapped = true;
			}
		}
	}

	if (isOverlapped)
	{
		if (milisec >= 0)
		{
			result = WaitForSingleObject(pHidDevice->readOverlapped.hEvent,0);
			if (result != WAIT_OBJECT_0)
			{
				return;
			}
		}
		DWORD resultOverlappedSize = 0;
		result = GetOverlappedResult(pHidDevice->handle, &pHidDevice->readOverlapped, &resultOverlappedSize, TRUE);
	}

	pHidDevice->isReadPending = false;
}

void GE::HID::SetOutputReport(HidDevice* pHidDevice, void* data, int dataSize)
{
	if (pHidDevice == nullptr)return;

	unsigned char* buffer;
	int bufferSize = 0;

	// 遅れてきたデータサイズがHidDeviceで確保されているデータサイズと不一致時の対応
	if ((unsigned)dataSize >= pHidDevice->writeBufferSize)
	{
		buffer = (unsigned char*)data;
		bufferSize = dataSize;
	}
	else
	{
		buffer = pHidDevice->writeBuffer;
		bufferSize = pHidDevice->writeBufferSize;

		// データをコピー
		memcpy_s(buffer, bufferSize, data, dataSize);
		// 余剰データ分があるならそこは0で埋める
		memset(buffer + dataSize, 0, (INT64)pHidDevice->writeBufferSize - dataSize);
		bufferSize = pHidDevice->writeBufferSize;
	}

	DWORD resultDataSize = 0;
	bool result = WriteFile(pHidDevice->handle, buffer, bufferSize, &resultDataSize, &pHidDevice->writeOverlapped);

	// 書き込み終了待ち
	if (result == false)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			result = WaitForSingleObject(pHidDevice->writeOverlapped.hEvent, 1000);
			if (result == false)return;

			DWORD overlappedSize = 0;
			result = GetOverlappedResult(pHidDevice->handle, &pHidDevice->writeOverlapped, &overlappedSize, FALSE);
		}
	}
}

void GE::HID::DeleteHidDevice(HidDevice* hidDevice)
{
	if (hidDevice == nullptr)return;

	delete hidDevice;
	hidDevice = nullptr;
}

void GE::HID::SetBlocking(HidDevice* pHidDevice, bool isBlocking)
{
	if (pHidDevice == nullptr)return;

	pHidDevice->isBlocking = isBlocking;
}
