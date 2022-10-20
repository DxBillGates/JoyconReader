#include <stdio.h>
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>
#include <iostream>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "SetupAPI.lib")

int main()
{
	// �ڑ�����Ă���hid�f�o�C�X�ɃA�N�Z�X���邽�߂�GUID���擾
	GUID hidGuid;
	HidD_GetHidGuid(&hidGuid);

	// hid�f�o�C�X���̃n���h�����擾
	HDEVINFO devinfo;
	// DIGCF_PRESENT : �ڑ�����Ă���HID�f�o�C�X����
	// DIGCF_DEVICEINTERFACE : ?
	devinfo = SetupDiGetClassDevs(&hidGuid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	// �ڑ�����Ă���HID�f�o�C�X������擾
	SP_DEVICE_INTERFACE_DATA spid;
	spid.cbSize = sizeof(spid);

	HANDLE joyconL = nullptr,joyconR = nullptr;

	OVERLAPPED ol = {};
	ol.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	DWORD getOverlappedResultSize = 0;

	for (int index = 0; ; ++index)
	{
		// �擾�ł��Ȃ��Ȃ����烋�[�v���I���
		if (!SetupDiEnumDeviceInterfaces(devinfo, NULL, &hidGuid, index, &spid))
		{
			break;
		}

		// �ڑ�����Ă���HID�f�o�C�X�̏�񂪂���ł��낤�A�h���X���擾?
		unsigned long size;
		SetupDiGetDeviceInterfaceDetail(devinfo, &spid, NULL, 0, &size, 0);
		PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetailData = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[size]);
		deviceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		SetupDiGetDeviceInterfaceDetail(devinfo, &spid, deviceDetailData, size, &size, 0);
		//std::cout << deviceDetailData->DevicePath << std::endl;

		// HID�f�o�C�X�ɃA�N�Z�X���邽�߂ɕK�v�ȃn���h���̍쐬
		HANDLE handle = CreateFile(deviceDetailData->DevicePath
								  ,GENERIC_READ | GENERIC_WRITE
								  ,FILE_SHARE_READ | FILE_SHARE_WRITE
								  ,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);

		// HID�ɑ΂��ē��o�͂��ł��Ȃ��f�o�C�X�̓n���h�������Ȃ�?
		if (handle == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		HIDD_ATTRIBUTES attr;
		// �x���ז�������if���Ŗ߂�l���g�p
		if (!HidD_GetAttributes(handle, &attr))
		{
			continue;
		}
		
		std::cout << "Index :" << index << std::endl;
		std::cout << "VendorID  : " << attr.VendorID << std::endl;
		std::cout << "ProductID : " << attr.ProductID << std::endl;
		std::cout << std::endl;

		// JoyCon L
		// vendorID 057e(16) = 1406(10)
		// productID 2006(16) = 8198(10)
		if (attr.VendorID == 1406 && attr.ProductID == 8198)
		{
			joyconL = handle;
		}
		// JoyCon R
		// vendorID 057e(16) = 1406(10)
		// productID 2007(16) = 8199(10)
		else if (attr.VendorID == 1406 && attr.ProductID == 8199)
		{
			joyconR = handle;
		}
		else
		{
			CloseHandle(handle);
		}
	}

	PHIDP_PREPARSED_DATA pp_data = NULL;
	HIDP_CAPS caps;

	HidD_GetPreparsedData(joyconL,&pp_data);
	HidP_GetCaps(pp_data, &caps);

	ULONG numReport = 0;
	HidD_GetNumInputBuffers(joyconL, &numReport);

	HidD_SetNumInputBuffers(joyconL, numReport);

	DWORD readSizeData = 0;
	DWORD readDataSize = caps.InputReportByteLength;

	BYTE joyconLData[362];
	BYTE joyconRData[362];


	bool resultJoyconL = ReadFile(joyconL, joyconLData, readDataSize, &readSizeData, &ol);
	if (resultJoyconL == false)
	{
		DWORD result = GetLastError();
		if (result == ERROR_IO_PENDING) 
		{
			GetOverlappedResult(joyconL, &ol, &getOverlappedResultSize, TRUE);
		}
		else
		{
			std::cout << "error" << std::endl;
		}
	}

	bool resultJoyconR = ReadFile(joyconR, joyconRData, readDataSize, &readSizeData, &ol);
	if (resultJoyconR == false)
	{
		DWORD result = GetLastError();
		if (result == ERROR_IO_PENDING)
		{
			GetOverlappedResult(joyconR, &ol, &getOverlappedResultSize, TRUE);
		}
		else
		{
			std::cout << "error" << std::endl;
		}
	}

	while (true)
	{
		static int a = 0;
		if (a == 100000000)break;

		if (a % 10 == 0)
		{
			if (!ReadFile(joyconL, joyconLData, readDataSize, &readSizeData, &ol))
			{
				DWORD result = GetLastError();
				if (result == ERROR_IO_PENDING)
				{
					GetOverlappedResult(joyconL, &ol, &getOverlappedResultSize, TRUE);
				}
				else
				{
					std::cout << "error" << std::endl;
				}
			}
			
			if (!ReadFile(joyconR, joyconRData, readDataSize, &readSizeData, &ol))
			{
				DWORD result = GetLastError();
				if (result == ERROR_IO_PENDING)
				{
					GetOverlappedResult(joyconR, &ol, &getOverlappedResultSize, TRUE);
				}
				else
				{
					std::cout << "error" << std::endl;
				}
			}

			int16_t acc[3], gyro[3];
			for (int i = 0; i < 3; ++i)
			{
				acc[i] = gyro[i] = 0;
			}

			int index = 13;

			for (int i = 0; i < 3; ++i,++index)
			{
				memcpy_s(&acc[i], sizeof(int16_t), &joyconLData[index], sizeof(int16_t));
			}
			for (int i = 0; i < 3; ++i,++index)
			{
				memcpy_s(&gyro[i], sizeof(int16_t), &joyconLData[index], sizeof(int16_t));
			}

			std::cout << "acc : " << acc[0] << ", " << acc[1] << ", " << acc[2];
			std::cout << "     ";
			std::cout << "gyro : " << gyro[0] << ", " << gyro[1] << ", " << gyro[2] << std::endl;
			//std::cout << a << std::endl;
		}


		++a;
	}

	system("pause");
	return 0;
}