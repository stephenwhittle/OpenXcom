#include "OptionsModBrowserUserConfigState.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/LayoutDriver.h"
#include "../Engine/Surface.h"
#include "../Engine/Game.h"
#include "modio/ModioSDK.h"
#include <memory>


OpenXcom::OptionsModBrowserUserConfigState::OptionsModBrowserUserConfigState()
{
	_window = new Window(this, 320, 200);
	_currentUserLabel = new Text(320, 16);
	_currentUserIcon = new Surface(50, 50, 0,0, 8);
	_currentUserValue = new Text(320, 16);
	_logoutButton = new TextButton(50, 16);
	_switchUserButton = new TextButton(50, 16);
	_ButtonSeparator = new Surface(48, 48);
	_backButton = new TextButton(50, 16);

	setInterface("optionsMenu");
	
	add(_window, "window", "optionsMenu");
	add(_currentUserLabel, "text", "modsMenu");
	add(_currentUserIcon);
	add(_currentUserValue);
	add(_logoutButton, "button", "optionsMenu");
	add(_switchUserButton, "button", "optionsMenu");
	add(_ButtonSeparator);
	add(_backButton, "button", "optionsMenu");

	_currentUserLabel->setText("Current User: ");
	_userGroup = LayoutGroup::Horizontal(320, 100,
		LayoutParam(_currentUserLabel).Proportional(2, 1),
		LayoutParam(_currentUserIcon).Absolute(50, 50).KeepSize(),
		LayoutParam(_currentUserValue).Proportional(3, 1));

	_logoutButton->setText("Log Out");
	_logoutButton->setTooltip("Logs out the current Mod.io user and returns back to the options menu");
	_logoutButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onLogoutClicked);
	_switchUserButton->setText("Switch User");
	_switchUserButton->setTooltip("Changes the actively logged in Mod.io user");
	_switchUserButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onSwitchUserClicked);
	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onBackClicked);

	_buttonGroup = LayoutGroup::Vertical(320, 100,
		LayoutParam(_logoutButton).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_switchUserButton).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_ButtonSeparator).Proportional(1, 1).OtherAxisStretch(),
		LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());

	LayoutDriver topLevelLayout = LayoutDriver(LayoutDirection::Horizontal, _window,
		LayoutParam(_userGroup).Proportional(4, 1).OtherAxisStretch(),
		LayoutParam(_buttonGroup).Proportional(1, 1).OtherAxisStretch()).Padding(4);
	topLevelLayout.ApplyLayout();


	Modio::GetUserMediaAsync(Modio::AvatarSize::Thumb50, [this](Modio::ErrorCode ec, Modio::Optional<Modio::filesystem::path> AvatarPath)
	{
		if (!ec)
		{
			_currentUserIcon->loadImage(AvatarPath->string());
		}
	});

	centerAllSurfaces();

}



void OpenXcom::OptionsModBrowserUserConfigState::onLogoutClicked(Action* action)
{
	Modio::ClearUserDataAsync([](Modio::ErrorCode ec) {});
	_game->popState();
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::onSwitchUserClicked(Action* action)
{
	Modio::ClearUserDataAsync([](Modio::ErrorCode ec) {});
	_game->popState();
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::onBackClicked(Action* action)
{
	_game->popState();
}
