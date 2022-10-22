#pragma once
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>

namespace GE
{
	namespace HID
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

			OVERLAPPED readOverlapped;
			OVERLAPPED writeOverlapped;

			bool isBlocking;
			bool isReadPending;

			HidDevice()
				: handle(nullptr)
				, hiddAttribute()
				, hidpCaps()
				, readBuffer(nullptr)
				, readBufferSize(0)
				, writeBuffer(nullptr)
				, writeBufferSize(0)
				, readOverlapped()
				, writeOverlapped()
				, isBlocking(false)
				, isReadPending(false)
			{
			}

			~HidDevice()
			{
				if (handle)
				{
					CloseHandle(handle);
					handle = nullptr;
				}

				if (readBuffer)delete[] readBuffer;

				if (writeBuffer)delete[] writeBuffer;

				if (readOverlapped.hEvent)
				{
					CloseHandle(readOverlapped.hEvent);
					readOverlapped = {};
				}

				if (writeOverlapped.hEvent)
				{
					CloseHandle(writeOverlapped.hEvent);
					writeOverlapped = {};
				}
			}
		};

		/// <summary>
		/// PCのGUIDを取得
		/// </summary>
		/// <returns>PCのGUID</returns>
		GUID* GetHidGuid();

		/// <summary>
		/// HIDのベンダーIDとプロダクトIDを指定し、接続されていれば取得
		/// </summary>
		/// <param name="vendorID">取得したいデバイスのベンダーID</param>
		/// <param name="productID">取得したいデバイスのプロダクトID</param>
		/// <returns>接続されていてベンダーIDとプロダクトIDが一致していればHIDデバイスの情報をポインタで返す</returns>
		HidDevice* GetNewHidDevice(DWORD vendorID, DWORD productID);

		/// <summary>
		/// 入力レポートを取得する
		/// </summary>
		/// <param name="pHidDevice">取得したレポートを持つデバイス</param>
		void GetInputReport(HidDevice* pHidDevice);

		/// <summary>
		/// HIDデバイスにデータを送る
		/// </summary>
		/// <param name="pHidDevice">データを送信したいHID</param>
		/// <param name="data">送りたいデータ</param>
		/// <param name="dataSize">送りたいデータサイズ</param>
		void SetOutputReport(HidDevice* pHidDevice, void* data, int dataSize);

		/// <summary>
		/// HidDeviceを解放する
		/// </summary>
		/// <param name="hidDevice">メモリを解放させたいHidDevice</param>
		void DeleteHidDevice(HidDevice* hidDevice);

		/// <summary>
		/// 指定のHIDDeviceの入力レポートを取得する際にHID側の応答を待つか
		/// </summary>
		/// <param name="pHidDevice">指定のHIDDevice</param>
		/// <param name="isBlocking">待つか</param>
		void SetBlocking(HidDevice* pHidDevice, bool isBlocking);
	}
}
