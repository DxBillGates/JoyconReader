#pragma once
#include <cstdint>
#include "HidDevice.h"

namespace GE
{
	// プロダクトID　兼　右左判別用
	enum class JoyconType
	{
		L = 8198,
		R = 8199,
	};

	enum class JoyconInputReportType
	{
		STANDARD_FULL_MODE = 0x30,
		SIMPLE_HID_MODE = 0x3f,
	};

	enum class JoyconCommandID
	{
		GET_ONLY_CONTROLLER_STATE       = 0x00,
		//BLUETOOTH_MANUAL_PAIRING        = 0x01,
		//REQUEST_DEVICE_INFO             = 0x02,

		/// <summary>
		/// data[0] : 0x30 or 0x3f / 0x30 = standard full mode, 0x3f = simple hid mode(cant read NFC/IR)
		/// </summary>
		SET_INPUT_REPORT_MODE           = 0x03,

		//TRIGGER_BUTTONS_ELAPSED_TIME    = 0x04,
		//GET_PAGE_LIST_STATE             = 0x05,

		/// <summary>
		/// data[0] : x00 = sleep mode / x01 = reboot and reconnect / x02 = reboot and pairing mode / x03 = reboot and reconnect
		/// </summary>
		SET_HCI_STATE                   = 0x06,
		//RESET_PAIRING_INFO              = 0x07,
		//SET_SHIPMMENT_LOW_POWER_STATE   = 0x08,
									    
		//SPI_FLASH_READ                  = 0x10,
		//SPI_FLASH_WRITE                 = 0x11,
		//SPI_SECTOR_ERASE                = 0x12,
									    
		/// <summary>
		/// no set data
		/// </summary>
		RESET_NFC_IR_MCU                = 0x20,

		//SET_NFC_IR_MCU_CONFIGURATION    = 0x21,
		//SET_NFC_IR_MCU_STATE            = 0x22,
		//SET_UNKNOWN_DATA                = 0x24,
		//RESET_UNKNOWN_DATA              = 0x25,
		//SET_UNKNOWN_NFC_IR_MCU_DATA     = 0x28,
		//GET_0x28_NFC_IR_MCU_DATA        = 0x29,
		//SET_GPIO_PIN_OUTPUT_VALUE_PORT2 = 0x2A,
		//GET_0x29_NFC_IR_MCU_DATA        = 0x2B,

		/// <summary>
		/// <para>data[0] : 4bit field 0x01 ~ 0x0f</para>
		/// <para>example : 0x01 = 1st light / 0x02 = 2nd light / 0x03 = 1st and 2nd light</para>
		/// </summary>
		SET_PLAYER_LIGHT                = 0x30,

		//GET_PLAYER_LIGHT                = 0x31,
		//SET_HOME_LIGHT                  = 0x38,

		/// <summary>
		/// data[0] : x00 is disable / x01 is enable
		/// </summary>
		ENABLE_IMU                      = 0x40,

		/// <summary>
		/// <para>data[0] : gyroscope sensitivity / 00 ~ 03 : 250 500 1000 2000dps</para>
		/// <para>data[1] : accelerometer sensitivity / 00 ~ 03 : 8 4 2 16G</para>
		/// <para>data[2] : gyroscope performance rate / 00 ~ 01 : 833 208Hz</para>
		/// <para>data[3] : accelerometer anti-aliasing filter bandwidth / 00 ~ 01 : 200 100Hz</para>
		/// </summary>
		SET_IMU_SENSITIVITY             = 0x41,

		//WRITE_TO_IMU_REGISTER           = 0x42,
		//READ_IMU_REGISTERS              = 0x43,

		/// <summary>
		/// data[0] : x00 is disable / x01 is enable
		/// </summary>
		ENABLE_VIVRATION                = 0x48,

		//GET_REGULATED_VOLTAGE           = 0x50,
		//SET_GPIO_PIN_OUTPUT_VALUE_PORT1 = 0x51,
	};

	enum class JoyconLightData
	{
		ONE_PLAYER   = 0x01,
		TWO_PLAYER   = 0x02,
		THREE_PLAYER = 0x04,
		FOUR_PLAYER  = 0x08,
	};

	enum class JoyconBatteryData
	{
		FULL = 8,
		MIDDLE = 4,
		LOW = 2,
		EMPTY = 0,
	};

	///// <summary>
	///// インプットレポート0x3f時のボタンフラグ判定用
	///// </summary>
	//enum class JoyconButtonData_IR0x3F
	//{
	//};

	/// <summary>
	/// インプットレポート0x30時のJoyconLのボタンフラグ判定用
	/// </summary>
	enum class JoyconButtonData
	{
		Y        = (1 << 0),
		X        = (1 << 1),
		B        = (1 << 2),
		A        = (1 << 3),
		RIGHT_SR = (1 << 4),
		RIGHT_SL = (1 << 5),
		R        = (1 << 6),
		ZR       = (1 << 7),

		DOWN    = (1 << 8),
		UP      = (1 << 9),
		RIGHT   = (1 << 10),
		LEFT    = (1 << 11),
		LEFT_SR = (1 << 12),
		LEFT_SL = (1 << 13),
		L       = (1 << 14),
		ZL      = (1 << 15),

		MINUS         = (1 << 16),
		PLUS          = (1 << 17),
		RSTICK        = (1 << 18),
		LSTICK        = (1 << 19),
		HOME          = (1 << 20),
		CAPTURE       = (1 << 21),
		//////////////////////////
		CHARGING_GRIP = (1 << 23),
	};

	struct Vector3Int16
	{
		union
		{
			struct
			{
				int16_t x, y, z;
			};

			int16_t value[3];
		};
	};

	struct Vector2Int16
	{
		union
		{
			struct
			{
				int16_t x, y;
			};

			int16_t value[2];
		};
	};

	class Joycon
	{
	private:
		HID::HidDevice* joycon;
		JoyconType joyconType;
		JoyconInputReportType currentInputReportType;

		BYTE buf[0x40];
		int globalPacketCount;

		byte beforeJoyconButtonState;
		byte beforeJoyconShareButtonState;

		byte currentJoyconButtonState;
		byte currentJoyconShareButtonState;

		Vector3Int16 accelerometer;
		Vector3Int16 gyroscope;

		Vector2Int16 stickData;
	private:
		void SendCommand(JoyconCommandID commandID, void* data, int dataSize);
		void SetInputReportType(JoyconInputReportType inputReportType);

		bool GetTrigger(byte before, byte current,int type);
		bool GetRelease(byte before, byte current,int type);
	public:
		Joycon(JoyconType type);
		~Joycon();

		void Initialize();
		void Update();

		void ResetPairing();

		void SetPlayerLight(JoyconLightData playerLightData);
		void SetIMU(bool flag);
		void SetVibration(bool flag);

		JoyconBatteryData GetBattery();
		bool GetButton(JoyconButtonData buttonType);
		bool GetTriggerButton(JoyconButtonData buttonType);
		bool GetReleaseButton(JoyconButtonData buttonType);

		Vector3Int16 GetAccelerometer();
		Vector3Int16 GetGyroscope();
		
		Vector2Int16 GetStick();
	};
}
