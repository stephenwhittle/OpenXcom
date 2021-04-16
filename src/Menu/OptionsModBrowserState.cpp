#include "OptionsModBrowserState.h"
#include "../Engine/Game.h"
#include "modio/ModioSDK.h"

void OpenXcom::OptionsModBrowserState::init()
{
	State::init();

	if (!Modio::QueryUserProfile().has_value())
	{
		_game->pushState(new OptionsModBrowserAuthState());
	}

}

void OpenXcom::OptionsModBrowserState::think()
{
	State::think();
}
