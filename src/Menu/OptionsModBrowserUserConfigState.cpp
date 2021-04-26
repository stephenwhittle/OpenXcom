#include "OptionsModBrowserUserConfigState.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "modio/ModioSDK.h"
#include <memory>

OpenXcom::OptionsModBrowserUserConfigState::OptionsModBrowserUserConfigState()
{
	_window = new Window(this, 320, 200);
	_currentUserLabel = new Text(320, 16);
	_currentUserIcon = new Surface(50, 50, 0, 0, 8);
	_currentUserValue = new Text(320, 16);
	_currentUserMods = new TextList(320, 100);
	_currentModsLabel = new Text(320, 16);
	_unsubscribeButton = new TextButton(50, 16);
	_logoutButton = new TextButton(50, 16);
	_switchUserButton = new TextButton(50, 16);
	_updateCheckButton = new TextButton(50, 16);
	_ButtonSeparator = new Surface(48, 48);
	_backButton = new TextButton(50, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_currentUserLabel, "text", "modsMenu");
	add(_currentUserIcon);
	add(_currentUserValue);
	add(_currentUserMods, "optionLists", "controlsMenu");
	add(_currentModsLabel, "text", "modsMenu");
	add(_unsubscribeButton, "button", "optionsMenu");
	add(_logoutButton, "button", "optionsMenu");
	add(_switchUserButton, "button", "optionsMenu");
	add(_updateCheckButton, "button", "optionsMenu");
	add(_ButtonSeparator);
	add(_backButton, "button", "optionsMenu");

	_currentModsLabel->setText("Subscribed Mods:");
	_unsubscribeButton->setText("Unsubscribe");
	_modListGroup = LayoutGroup::Horizontal(320, 200,
											LayoutParam(_currentUserMods).Proportional(3, 1).OtherAxisStretch(),
											LayoutParam(_unsubscribeButton).Proportional(1, 1));

	_currentUserLabel->setText("Current User: ");
	_userProfileGroup = LayoutGroup::Horizontal(320, 200,
												LayoutParam(_currentUserLabel).Proportional(2, 1),
												LayoutParam(_currentUserIcon).Absolute(50, 50).KeepSize(),
												LayoutParam(_currentUserValue).Proportional(3, 1));

	_userGroup = LayoutGroup::Vertical(320, 200,
									   LayoutParam(_userProfileGroup).Absolute(1, 16).OtherAxisStretch(),
									   LayoutParam(_currentModsLabel).Absolute(1, 16).OtherAxisStretch(),
									   LayoutParam(_modListGroup).Proportional(1, 1).OtherAxisStretch());

	_logoutButton->setText("Log Out");
	_logoutButton->setTooltip("Logs out the current Mod.io user and returns back to the options menu");
	_logoutButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onLogoutClicked);
	_switchUserButton->setText("Switch User");
	_switchUserButton->setTooltip("Changes the actively logged in Mod.io user");
	_switchUserButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onSwitchUserClicked);
	_updateCheckButton->setText("Check Updates");
	_updateCheckButton->setTooltip("Checks for external changes to your subscriptions");
	_updateCheckButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onUpdateClicked);
	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onBackClicked);

	_buttonGroup = LayoutGroup::Vertical(320, 100,
										 LayoutParam(_logoutButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_switchUserButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_updateCheckButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_ButtonSeparator).Proportional(1, 1).OtherAxisStretch(),
										 LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());

	LayoutDriver topLevelLayout = LayoutDriver(LayoutDirection::Horizontal, _window,
											   LayoutParam(_userGroup).Proportional(4, 1).OtherAxisStretch(),
											   LayoutParam(_buttonGroup).Proportional(1, 1).OtherAxisStretch())
									  .Padding(4);
	topLevelLayout.ApplyLayout();

	Modio::GetUserMediaAsync(Modio::AvatarSize::Thumb50, [this](Modio::ErrorCode ec, Modio::Optional<Modio::filesystem::path> AvatarPath) {
		if (!ec)
		{
			Surface *tmpSurface = new Surface(1, 1, 0, 0, 24);
			tmpSurface->loadImage(AvatarPath->string());
			tmpSurface->blit(_currentUserIcon);
			//_currentUserIcon->loadImage(AvatarPath->string());
		}
	});

	Modio::Optional<Modio::User> CurrentUser = Modio::QueryUserProfile();
	if (CurrentUser.has_value())
	{
		_currentUserValue->setText(CurrentUser->Username);
	}

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserUserConfigState::onLogoutClicked(Action *action)
{
	Modio::ClearUserDataAsync([](Modio::ErrorCode ec) {});
	_game->popState();
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::onSwitchUserClicked(Action *action)
{
	Modio::ClearUserDataAsync([](Modio::ErrorCode ec) {});
	_game->popState();
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::onUpdateClicked(Action *action)
{
	Modio::FetchExternalUpdatesAsync([](Modio::ErrorCode ec) {});
}

void OpenXcom::OptionsModBrowserUserConfigState::onBackClicked(Action *action)
{
	_game->popState();
}
