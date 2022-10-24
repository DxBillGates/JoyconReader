#include "Joycon.h"
#include <iostream>
#include <thread>

void GE::Joycon::SendCommand(JoyconCommandID commandID, void* data, int dataSize)
{
	if (globalPacketCount >= 16)globalPacketCount = 0;

	memset(buf, 0, 0x40);
	buf[0] = 1;
	buf[1] = globalPacketCount;

	// rumble data
	buf[2] = 0x00;
	buf[3] = 0x01;
	buf[4] = 0x40;
	buf[5] = 0x40;
	buf[6] = 0x00;
	buf[7] = 0x01;
	buf[8] = 0x40;
	buf[9] = 0x40;

	// subcommand id
	buf[10] = (int)commandID;

	memcpy_s(buf + 11, 0x40 - 10, data, 1);

	SetOutputReport(joycon, buf, 48);

	// 0.016秒停止
	std::this_thread::sleep_for(std::chrono::milliseconds(16));

	++globalPacketCount;
}

void GE::Joycon::SetInputReportType(JoyconInputReportType inputReportType)
{
	byte data[1];
	data[0] = (int)inputReportType;

	SendCommand(JoyconCommandID::SET_INPUT_REPORT_MODE, data, 1);

	currentInputReportType = inputReportType;
}

bool GE::Joycon::GetTrigger(byte before, byte current, int type)
{
	if (type >= (int)JoyconButtonData::MINUS)type /= (int)JoyconButtonData::MINUS;
	else if (type >= (int)JoyconButtonData::DOWN)type /= (int)JoyconButtonData::DOWN;

	bool beforeState = before & type;
	bool currentState = current & type;
	return beforeState == false && currentState == true;
}

bool GE::Joycon::GetRelease(byte before, byte current, int type)
{
	if (type >= (int)JoyconButtonData::MINUS)type /= (int)JoyconButtonData::MINUS;
	else if (type >= (int)JoyconButtonData::DOWN)type /= (int)JoyconButtonData::DOWN;

	bool beforeState = before & type;
	bool currentState = current & type;
	return beforeState == true && currentState == false;
}

GE::Joycon::Joycon(JoyconType type)
	: joycon(HID::GetNewHidDevice(1406, (int)type))
	, joyconType(type)
	, currentInputReportType()
	, buf()
	, globalPacketCount(0)
	, beforeJoyconButtonState(0)
	, beforeJoyconShareButtonState(0)
	, currentJoyconButtonState(0)
	, currentJoyconShareButtonState(0)
	, accelerometer()
	, gyroscope()
	, stickData()
{
}

GE::Joycon::~Joycon()
{
	HID::DeleteHidDevice(joycon);
}

void GE::Joycon::Initialize()
{
	memset(buf, 0, 0x40);
	globalPacketCount = 0;
	accelerometer = { 0,0,0 };
	gyroscope = { 0,0,0 };

	HID::SetBlocking(joycon, true);
}

void GE::Joycon::Update()
{
	if (joycon == nullptr)return;

	// get state 
	if (currentInputReportType != JoyconInputReportType::STANDARD_FULL_MODE)
	{
		SetInputReportType(JoyconInputReportType::STANDARD_FULL_MODE);
	}

	// ジョイコンのデータ取得
	HID::GetInputReport(joycon);

	// 各ボタンの情報取得
	beforeJoyconButtonState = currentJoyconButtonState;
	beforeJoyconShareButtonState = currentJoyconShareButtonState;

	currentJoyconButtonState = joyconType == JoyconType::L ? joycon->readBuffer[5] : joycon->readBuffer[3];
	currentJoyconShareButtonState = joycon->readBuffer[4];

	// スティック情報取得
	const int LEFT_STICK_DATA_INDEX = 6;
	const int RIGHT_STICK_DATA_INDEX = 9;
	uint8_t* data = joycon->readBuffer + (joyconType == JoyconType::L ? LEFT_STICK_DATA_INDEX : RIGHT_STICK_DATA_INDEX);
	stickData.x = data[0] | ((data[1] & 0xf) << 8);
	stickData.y = (data[1] >> 4) | (data[2] << 4);

	// ジャイロと加速度センサーの情報を取得
	int accelIndex = 13;
	int gyroIndex = 19;
	const Vector3Int16 OFFSET = { 0,0,0 };
	for (int i = 0; i < 3; ++i, accelIndex += 2, gyroIndex += 2)
	{
		memcpy_s(&accelerometer.value[i], sizeof(int16_t), &joycon->readBuffer[accelIndex], sizeof(int16_t));
		memcpy_s(&gyroscope.value[i], sizeof(int16_t), &joycon->readBuffer[gyroIndex], sizeof(int16_t));

		accelerometer.value[i] -= OFFSET.value[i];
		gyroscope.value[i] -= OFFSET.value[i];

		accelerometer.value[i] *= 0.000244f;
		gyroscope.value[i] *= 0.06103f;
	}
}

void GE::Joycon::ResetPairing()
{
	byte data[1] = {0x02};

	SendCommand(JoyconCommandID::SET_HCI_STATE, data, 1);
}

void GE::Joycon::SetPlayerLight(JoyconLightData playerLightData)
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

GE::JoyconBatteryData GE::Joycon::GetBattery()
{
	int battery = (int)(joycon->readBuffer[2]) >> 4;
	return (JoyconBatteryData)battery;
}

bool GE::Joycon::GetButton(JoyconButtonData buttonType)
{
	if (joycon == nullptr)return false;

	int currentState = (int)buttonType >= (int)JoyconButtonData::MINUS ? currentJoyconShareButtonState : currentJoyconButtonState;
	int type = (int)buttonType;
	if (type >= (int)JoyconButtonData::MINUS)type /= (int)JoyconButtonData::MINUS;
	else if (type >= (int)JoyconButtonData::DOWN)type /= (int)JoyconButtonData::DOWN;

	return (currentState & type);
}

bool GE::Joycon::GetTriggerButton(JoyconButtonData buttonType)
{
	if (joycon == nullptr)return false;

	bool isShareInput = (int)buttonType >= (int)JoyconButtonData::MINUS;
	int beforeState = isShareInput ? beforeJoyconShareButtonState : beforeJoyconButtonState;
	int currentState = isShareInput ? currentJoyconShareButtonState : currentJoyconButtonState;

	return GetTrigger(beforeState,currentState,(int)buttonType);
}

bool GE::Joycon::GetReleaseButton(JoyconButtonData buttonType)
{
	if (joycon == nullptr)return false;

	bool isShareInput = (int)buttonType >= (int)JoyconButtonData::MINUS;
	int beforeState = isShareInput ? beforeJoyconShareButtonState : beforeJoyconButtonState;
	int currentState = isShareInput ? currentJoyconShareButtonState : currentJoyconButtonState;

	return GetRelease(beforeState,currentState,(int)buttonType);
}

GE::Vector3Int16 GE::Joycon::GetAccelerometer()
{
	return accelerometer;
}

GE::Vector3Int16 GE::Joycon::GetGyroscope()
{
	return gyroscope;
}

GE::Vector2Int16 GE::Joycon::GetStick()
{
	return stickData;
}
