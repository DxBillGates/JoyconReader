#include "Joycon.h"
#include <iostream>

void GE::Joycon::SendCommand(JoyconCommandID commandID, void* data, int dataSize)
{
	if (globalPacketCount >= 16)globalPacketCount = 0;

	memset(buf, 0, 0x40);
	buf[0] = 1;
	buf[1] = globalPacketCount;
	buf[10] = (int)commandID;

	memcpy_s(buf + 11, 0x40 - 10, data, 1);

	SetOutputReport(joyconL, buf, 0x40);
	SetOutputReport(joyconR, buf, 0x40);

	++globalPacketCount;
}

void GE::Joycon::SetInputReportType(JoyconInputReportType inputReportType)
{
	byte data[1];
	data[0] = (int)inputReportType;

	SendCommand(JoyconCommandID::SET_INPUT_REPORT_MODE, data, 1);

	currentInputReportType = inputReportType;
}

GE::Joycon::Joycon()
	: joyconL(HID::GetNewHidDevice(1406, 8198))
	, joyconR(HID::GetNewHidDevice(1406, 8199))
	, buf()
	, globalPacketCount(0)
	, currentInputReportType()
	, accelerometer()
	, gyroscope()
{
}

GE::Joycon::~Joycon()
{
	HID::DeleteHidDevice(joyconL);
	HID::DeleteHidDevice(joyconR);
}

void GE::Joycon::Initialize()
{
	memset(buf, 0, 0x40);
	globalPacketCount = 0;
	memset(accelerometer, 0, 6);
	memset(gyroscope, 0, 6);

	HID::SetBlocking(joyconL, false);
	HID::SetBlocking(joyconR, false);
}

void GE::Joycon::Update()
{
	// get state 
	// first button state
	// second imu state
	//if (currentInputReportType != JoyconInputReportType::SIMPLE_HID_MODE)
	//{
	//	SetInputReportType(JoyconInputReportType::SIMPLE_HID_MODE);
	//}

	//HID::GetInputReport(joyconL);
	//HID::GetInputReport(joyconR);

	if (currentInputReportType != JoyconInputReportType::STANDARD_FULL_MODE)
	{
		SetInputReportType(JoyconInputReportType::STANDARD_FULL_MODE);
	}

	HID::GetInputReport(joyconL);
	HID::GetInputReport(joyconR);

	std::cout << "joyconL button state : " << (int)joyconL->readBuffer[5] << std::endl;
	std::cout << "joyconL stick state : " << (int)joyconL->readBuffer[4] << std::endl;
	std::cout << "joyconR stick state : " << (int)joyconR->readBuffer[4] << std::endl;
	std::cout << "joyconR button state : " << (int)joyconR->readBuffer[3] << std::endl;

	int index = 13;
	int accelIndex = 13;
	int gyroIndex = 19;

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j, accelIndex += 2, gyroIndex += 2)
		{
			memcpy_s(&accelerometer[i].value[j], sizeof(int16_t), &joyconL->readBuffer[accelIndex], sizeof(int16_t));
			memcpy_s(&gyroscope[i].value[j], sizeof(int16_t), &joyconL->readBuffer[gyroIndex], sizeof(int16_t));

			accelerometer[i].value[j] *= 0.000244f;
			gyroscope[i].value[j] *= 0.06103f;
		}

		accelIndex = 13;
		gyroIndex = 19;
	}

	std::cout << "L acc : " << accelerometer[0].value[0] << ", " << accelerometer[0].value[1] << ", " << accelerometer[0].value[2];
	std::cout << "  ";
	std::cout << "gyro : " << gyroscope[0].value[0] << ", " << gyroscope[0].value[1] << ", " << gyroscope[0].value[2] << std::endl;
}

void GE::Joycon::SetPlayerLight(PlayerLightData playerLightData)
{
	byte data[1];
	data[0] = (int)playerLightData;

	SendCommand(JoyconCommandID::SET_PLAYER_LIGHT, data, 1);
}

void GE::Joycon::SetIMU(bool flag)
{
	byte data[1];
	data[0] = flag;

	SendCommand(JoyconCommandID::ENABLE_IMU, data, 1);
}

void GE::Joycon::SetVibration(bool flag)
{
	byte data[1];
	data[0] = flag;

	SendCommand(JoyconCommandID::ENABLE_VIVRATION, data, 1);
}
