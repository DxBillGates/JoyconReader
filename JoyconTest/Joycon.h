#pragma once
#include <cstdint>
#include "HidDevice.h"

namespace GE
{
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
		//SET_HCI_STATE                   = 0x06,
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

	enum class PlayerLightData
	{
		ONE_PLAYER   = 0x01,
		TWO_PLAYER   = 0x02,
		THREE_PLAYER = 0x04,
		FOUR_PLAYER  = 0x08,
	};

	struct VectorInt16
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

	class Joycon
	{
	private:
		HID::HidDevice* joyconL;
		HID::HidDevice* joyconR;

		BYTE buf[0x40];
		int globalPacketCount;

		JoyconInputReportType currentInputReportType;

		VectorInt16 accelerometer[2];
		VectorInt16 gyroscope[2];
	private:
		void SendCommand(JoyconCommandID commandID, void* data, int dataSize);
		void SetInputReportType(JoyconInputReportType inputReportType);
	public:
		Joycon();
		~Joycon();

		void Initialize();
		void Update();

		void SetPlayerLight(PlayerLightData playerLightData);
		void SetIMU(bool flag);
		void SetVibration(bool flag);
	};
}
