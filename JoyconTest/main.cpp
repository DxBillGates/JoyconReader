#include "HidDevice.h"
#include "Joycon.h"
#include <iostream>

int main()
{
	//// 接続されているhidデバイスにアクセスするためのGUIDを取得
	//GUID hidGuid;
	//HidD_GetHidGuid(&hidGuid);

	//// hidデバイス情報のハンドルを取得
	//HDEVINFO devinfo;
	//// DIGCF_PRESENT : 接続されているHIDデバイスだけ
	//// DIGCF_DEVICEINTERFACE : ?
	//devinfo = SetupDiGetClassDevs(&hidGuid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	//// 接続されているHIDデバイスを一つずつ取得
	//SP_DEVICE_INTERFACE_DATA spid;
	//spid.cbSize = sizeof(spid);

	//HANDLE joyconL = nullptr,joyconR = nullptr;

	//OVERLAPPED ol = {};
	//ol.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//DWORD getOverlappedResultSize = 0;

	//for (int index = 0; ; ++index)
	//{
	//	// 取得できなくなったらループを終わる
	//	if (!SetupDiEnumDeviceInterfaces(devinfo, NULL, &hidGuid, index, &spid))
	//	{
	//		break;
	//	}

	//	// 接続されているHIDデバイスの情報があるであろうアドレスを取得?
	//	unsigned long size;
	//	SetupDiGetDeviceInterfaceDetail(devinfo, &spid, NULL, 0, &size, 0);
	//	PSP_INTERFACE_DEVICE_DETAIL_DATA deviceDetailData = PSP_INTERFACE_DEVICE_DETAIL_DATA(new char[size]);
	//	deviceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	//	SetupDiGetDeviceInterfaceDetail(devinfo, &spid, deviceDetailData, size, &size, 0);
	//	//std::cout << deviceDetailData->DevicePath << std::endl;

	//	// HIDデバイスにアクセスするために必要なハンドルの作成
	//	HANDLE handle = CreateFile(deviceDetailData->DevicePath
	//							  ,GENERIC_READ | GENERIC_WRITE
	//							  ,FILE_SHARE_READ | FILE_SHARE_WRITE
	//							  ,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);

	//	// HIDに対して入出力ができないデバイスはハンドルを作れない?
	//	if (handle == INVALID_HANDLE_VALUE)
	//	{
	//		continue;
	//	}

	//	HIDD_ATTRIBUTES attr;
	//	// 警告邪魔だからif文で戻り値を使用
	//	if (!HidD_GetAttributes(handle, &attr))
	//	{
	//		continue;
	//	}
	//	
	//	std::cout << "Index :" << index << std::endl;
	//	std::cout << "VendorID  : " << attr.VendorID << std::endl;
	//	std::cout << "ProductID : " << attr.ProductID << std::endl;
	//	std::cout << std::endl;

	//	// JoyCon L
	//	// vendorID 057e(16) = 1406(10)
	//	// productID 2006(16) = 8198(10)
	//	if (attr.VendorID == 1406 && attr.ProductID == 8198)
	//	{
	//		joyconL = handle;
	//	}
	//	// JoyCon R
	//	// vendorID 057e(16) = 1406(10)
	//	// productID 2007(16) = 8199(10)
	//	else if (attr.VendorID == 1406 && attr.ProductID == 8199)
	//	{
	//		joyconR = handle;
	//	}
	//	else
	//	{
	//		CloseHandle(handle);
	//	}
	//}

	//PHIDP_PREPARSED_DATA pp_data = NULL;
	//HIDP_CAPS caps;

	//HidD_GetPreparsedData(joyconL,&pp_data);
	//HidP_GetCaps(pp_data, &caps);

	//ULONG numReport = 0;
	//HidD_GetNumInputBuffers(joyconL, &numReport);

	////HidD_SetNumInputBuffers(joyconL, numReport);

	//DWORD readSizeData = 0;
	//DWORD readDataSize = caps.InputReportByteLength;

	//BYTE* joyconLData = new BYTE[readDataSize];
	//BYTE* joyconRData = new BYTE[readDataSize];

	//bool resultJoyconL = ReadFile(joyconL, joyconLData, readDataSize, &readSizeData, &ol);
	//if (resultJoyconL == false)
	//{
	//	DWORD result = GetLastError();
	//	if (result == ERROR_IO_PENDING) 
	//	{
	//		GetOverlappedResult(joyconL, &ol, &getOverlappedResultSize, TRUE);
	//	}
	//	else
	//	{
	//		std::cout << "error" << std::endl;
	//	}
	//}

	//bool resultJoyconR = ReadFile(joyconR, joyconRData, readDataSize, &readSizeData, &ol);
	//if (resultJoyconR == false)
	//{
	//	DWORD result = GetLastError();
	//	if (result == ERROR_IO_PENDING)
	//	{
	//		GetOverlappedResult(joyconR, &ol, &getOverlappedResultSize, TRUE);
	//	}
	//	else
	//	{
	//		std::cout << "error" << std::endl;
	//	}
	//}
	//while (true)
	//{
	//	static int a = 0;
	//	if (a == 100000000)break;

	//	if (a % 10 == 0)
	//	{
	//		if (!ReadFile(joyconL, joyconLData, readDataSize, &readSizeData, &ol))
	//		{
	//			DWORD result = GetLastError();
	//			if (result == ERROR_IO_PENDING)
	//			{
	//				GetOverlappedResult(joyconL, &ol, &getOverlappedResultSize, TRUE);
	//			}
	//			else
	//			{
	//				std::cout << "error" << std::endl;
	//			}
	//		}
	//		
	//		if (!ReadFile(joyconR, joyconRData, readDataSize, &readSizeData, &ol))
	//		{
	//			DWORD result = GetLastError();
	//			if (result == ERROR_IO_PENDING)
	//			{
	//				GetOverlappedResult(joyconR, &ol, &getOverlappedResultSize, TRUE);
	//			}
	//			else
	//			{
	//				std::cout << "error" << std::endl;
	//			}
	//		}

	//		{
	//			int16_t acc[3], gyro[3];
	//			for (int i = 0; i < 3; ++i)
	//			{
	//				acc[i] = gyro[i] = 0;
	//			}

	//			int index = 13;

	//			for (int i = 0; i < 3; ++i, ++index)
	//			{
	//				memcpy_s(&acc[i], sizeof(int16_t), &joyconLData[index], sizeof(int16_t));
	//			}
	//			for (int i = 0; i < 3; ++i, ++index)
	//			{
	//				memcpy_s(&gyro[i], sizeof(int16_t), &joyconLData[index], sizeof(int16_t));
	//			}

	//			std::cout << "L acc : " << acc[0] << ", " << acc[1] << ", " << acc[2];
	//			std::cout << "     ";
	//			std::cout << "gyro : " << gyro[0] << ", " << gyro[1] << ", " << gyro[2] << std::endl;
	//		}

	//		{
	//			int16_t acc[3], gyro[3];
	//			for (int i = 0; i < 3; ++i)
	//			{
	//				acc[i] = gyro[i] = 0;
	//			}

	//			int index = 13;

	//			for (int i = 0; i < 3; ++i, ++index)
	//			{
	//				memcpy_s(&acc[i], sizeof(int16_t), &joyconRData[index], sizeof(int16_t));
	//			}
	//			for (int i = 0; i < 3; ++i, ++index)
	//			{
	//				memcpy_s(&gyro[i], sizeof(int16_t), &joyconRData[index], sizeof(int16_t));
	//			}

	//			std::cout << "R acc : " << acc[0] << ", " << acc[1] << ", " << acc[2];
	//			std::cout << "     ";
	//			std::cout << "gyro : " << gyro[0] << ", " << gyro[1] << ", " << gyro[2] << std::endl;
	//		}
	//	}


	//	++a;
	//}

	//delete[] joyconLData;
	//delete[] joyconRData;

	//// https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md

	//using namespace GE::HID;
	//HidDevice* joyconL = GetNewHidDevice(1406, 8198);
	//HidDevice* joyconR = GetNewHidDevice(1406, 8199);

	//// LEDライトをいじってみる
	//int globalCount = 0;
	//int command = 0x40;

	//byte data[1];
	//data[0] = 0x01;

	//byte buf[0x40];
	//memset(buf, 0, 0x40);
	//buf[0] = 1;
	//buf[1] = globalCount;

	//buf[10] = command;
	//memcpy_s(buf + 11, 0x40 - 10, data, 1);

	//SetOutputReport(joyconL, buf, 0x40);
	//SetOutputReport(joyconR, buf, 0x40);


	//command = 0x03;
	//data[0] = 0x30;
	//buf[0] = 1;
	//buf[1] = ++globalCount;
	//buf[10] = command;
	//memcpy_s(buf + 11, 0x40 - 10, data, 1);

	//SetOutputReport(joyconL, buf, 0x40);
	//SetOutputReport(joyconR, buf, 0x40);


	//SetBlocking(joyconL, true);
	//SetBlocking(joyconR, true);

	//while (true)
	//{
	//	static int a = 0;
	//	if (a == 100000000)break;

	//	if (a % 10 == 0)
	//	{
	//		GetInputReport(joyconL);
	//		GetInputReport(joyconR);

	//		{
	//			int16_t acc[3], gyro[3];
	//			for (int i = 0; i < 3; ++i)
	//			{
	//				acc[i] = gyro[i] = 0;
	//			}

	//			int index = 13;

	//			for (int i = 0; i < 3; ++i, ++index,++index)
	//			{
	//				memcpy_s(&acc[i], sizeof(int16_t), &joyconL->readBuffer[index], sizeof(int16_t));
	//				acc[i] *= 0.000244f;
	//			}
	//			for (int i = 0; i < 3; ++i, ++index, ++index)
	//			{
	//				memcpy_s(&gyro[i], sizeof(int16_t), &joyconL->readBuffer[index], sizeof(int16_t));
	//				gyro[i] *= 0.06103f;
	//			}

	//			//std::cout << "L acc : " << acc[0] << ", " << acc[1] << ", " << acc[2];
	//			//std::cout << "  ";
	//			//std::cout << "gyro : " << gyro[0] << ", " << gyro[1] << ", " << gyro[2];

	//			//if (joyconL->readBuffer[0] == 0x3f)
	//			//{
	//			//	std::cout << "JoyconLButton" << (int)joyconL->readBuffer[1] << std::endl;
	//			//	std::cout << "JoyconRButton" << (int)joyconR->readBuffer[1] << std::endl;
	//			//}
	//			//else
	//			//{
	//				//std::cout << "JoyconLButton" << (int)joyconL->readBuffer[3] << std::endl;
	//				//std::cout << "JoyconRButton" << (int)joyconR->readBuffer[5] << std::endl;
	//			//}
	//		}

	//		{
	//			int16_t acc[3], gyro[3];
	//			for (int i = 0; i < 3; ++i)
	//			{
	//				acc[i] = gyro[i] = 0;
	//			}

	//			int index = 13;

	//			for (int i = 0; i < 3; ++i, ++index, ++index)
	//			{
	//				memcpy_s(&acc[i], sizeof(int16_t), &joyconR->readBuffer[index], sizeof(int16_t));
	//				acc[i] *= 0.000244f;
	//			}
	//			for (int i = 0; i < 3; ++i, ++index, ++index)
	//			{
	//				memcpy_s(&gyro[i], sizeof(int16_t), &joyconR->readBuffer[index], sizeof(int16_t));
	//				gyro[i] *= 0.06103f;
	//			}

	//			//std::cout << "   R acc : " << acc[0] << ", " << acc[1] << ", " << acc[2];
	//			//std::cout << "  ";
	//			//std::cout << "gyro : " << gyro[0] << ", " << gyro[1] << ", " << gyro[2] << std::endl;
	//		}
	//	}
	//	++a;
	//}

	//DeleteHidDevice(joyconL);
	//DeleteHidDevice(joyconR);

	GE::Joycon joyconL(GE::JoyconType::L);
	joyconL.Initialize();
	joyconL.SetIMU(true);
	joyconL.SetPlayerLight(GE::PlayerLightData::ONE_PLAYER);

	GE::Joycon joyconR(GE::JoyconType::R);
	joyconR.Initialize();
	joyconR.SetIMU(true);
	joyconR.SetPlayerLight(GE::PlayerLightData::ONE_PLAYER);


	while (true)
	{
		static int a = 0;
		if (a == 100000000)break;

		if (a % 10 == 0)
		{
			joyconL.Update();
			joyconR.Update();
			//std::cout << "stick x : " << joycon.GetStick().x << std::endl;
			//std::cout << "stick y : " << joycon.GetStick().y << std::endl;

			std::cout << "minus" << joyconL.GetButton(GE::JoyconButtonData::MINUS) << std::endl;
			std::cout << " plus" << joyconR.GetButton(GE::JoyconButtonData::PLUS) <<  std::endl;

			if (joyconL.GetReleaseButton(GE::JoyconButtonData::LSTICK))
			{
				std::cout << "release" << std::endl;
			}

			if (joyconR.GetTriggerButton(GE::JoyconButtonData::RSTICK))
			{
				std::cout << "trigger" << std::endl;
			}
		}

		++a;
	}

	system("pause");
	return 0;
}