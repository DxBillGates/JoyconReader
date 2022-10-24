#include "Joycon.h"
#include <iostream>
#include <crtdbg.h>

int main()
{
	// メモリリーク確認
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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
		if (a == 10000)break;

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