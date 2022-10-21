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
	/// PC��GUID���擾
	/// </summary>
	/// <returns>PC��GUID</returns>
	static GUID* GetHidGuid();

	/// <summary>
	/// HID�̃x���_�[ID�ƃv���_�N�gID���w�肵�A�ڑ�����Ă���Ύ擾
	/// </summary>
	/// <param name="vendorID">�擾�������f�o�C�X�̃x���_�[ID</param>
	/// <param name="productID">�擾�������f�o�C�X�̃v���_�N�gID</param>
	/// <returns>�ڑ�����Ă��ăx���_�[ID�ƃv���_�N�gID����v���Ă����HID�f�o�C�X�̏����|�C���^�ŕԂ�</returns>
	static HidDevice* GetNewHidDevice(DWORD vendorID, DWORD productID);

	static void DeleteHidDevice(HidDevice* hidDevice);
}
