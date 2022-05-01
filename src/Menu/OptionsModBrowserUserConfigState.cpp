#include "OptionsModBrowserUserConfigState.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"
#include "../Mod/Mod.h"
#include "modio/ModioSDK.h"
#include <algorithm>
#include <memory>

OpenXcom::OptionsModBrowserUserConfigState::OptionsModBrowserUserConfigState()
{
	_data = std::make_shared<StateData>();

	_window = new Window(this, 320, 200);
	_currentUserLabel = new Text(320, 16);
	_currentUserIcon = new Surface(50, 50, 0, 0, 8);
	_currentUserValue = new Text(320, 16);
	_modListSeparator = new Surface(1, 1);
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
	add(_currentUserValue, "text", "modsMenu");
	add(_currentUserMods, "optionLists", "controlsMenu");
	add(_currentModsLabel, "text", "modsMenu");
	add(_modListSeparator);
	add(_unsubscribeButton, "button", "optionsMenu");
	add(_logoutButton, "button", "optionsMenu");
	add(_switchUserButton, "button", "optionsMenu");
	add(_updateCheckButton, "button", "optionsMenu");
	add(_ButtonSeparator);
	add(_backButton, "button", "optionsMenu");

	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_currentModsLabel->setText("Subscribed Mods:");
	_currentModsLabel->setVerticalAlign(ALIGN_MIDDLE);
	_unsubscribeButton->setText("Unsubscribe");
	_unsubscribeButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onUnsubscribeClicked);

	_currentUserLabel->setText("Current User: ");
	_currentUserLabel->setVerticalAlign(ALIGN_MIDDLE);
	_currentUserValue->setVerticalAlign(ALIGN_MIDDLE);
	_userProfileGroup = LayoutGroup::Horizontal(320, 200,
												LayoutParam(_currentUserLabel).Proportional(2, 1),
												LayoutParam(_currentUserIcon).Absolute(50, 50).KeepSize(),
												LayoutParam(_currentUserValue).Proportional(3, 1));

	_currentUserMods->setColumns(1, _currentUserMods->getWidth() - 10);
	_currentUserMods->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onModSelected);
	_currentUserMods->setSelectable(true);
	_currentUserMods->setBackground(_window);

	_userGroup = LayoutGroup::Vertical(320, 200,
									   LayoutParam(_userProfileGroup).Absolute(1, 16).OtherAxisStretch(),
									   LayoutParam(_modListSeparator).Absolute(1, 32).OtherAxisStretch(),
									   LayoutParam(_currentModsLabel).Absolute(1, 16).OtherAxisStretch(),
									   LayoutParam(_currentUserMods).Proportional(1, 1).OtherAxisStretch());

	_logoutButton->setText("Log Out");
	_logoutButton->setTooltip("Logs out the current Mod.io user and returns back to the options menu");
	_logoutButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onLogoutClicked);

	_switchUserButton->setTooltip("Changes the actively logged in Mod.io user");
	_switchUserButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onSwitchUserClicked);

	_updateCheckButton->setTooltip("Checks for external changes to your subscriptions");
	_updateCheckButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onUpdateClicked);
	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserUserConfigState::onBackClicked);

	_buttonGroup = LayoutGroup::Vertical(320, 100,
										 LayoutParam(_logoutButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_switchUserButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_updateCheckButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_unsubscribeButton).Absolute(1, 16).OtherAxisStretch(),
										 LayoutParam(_ButtonSeparator).Proportional(1, 1).OtherAxisStretch(),
										 LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());

	LayoutDriver topLevelLayout = LayoutDriver(LayoutDirection::Horizontal, _window,
											   LayoutParam(_userGroup).Proportional(4, 1).OtherAxisStretch(),
											   LayoutParam(_buttonGroup).Proportional(1, 1).OtherAxisStretch())
									  .Padding(4);
	topLevelLayout.ApplyLayout();

	//Workaround - forces the selector Surface to be resized based on the new size driven by the LayoutDriver
	_currentUserMods->setSmall();

	_switchUserButton->setText("Switch User");
	_updateCheckButton->setText("Get Updates");

	Modio::GetUserMediaAsync(Modio::AvatarSize::Thumb50, [_data = this->_data](Modio::ErrorCode ec, Modio::Optional<Modio::filesystem::path> AvatarPath) {
		if (!ec)
		{
			_data->userProfileImagePath = AvatarPath->string();
		}
	});

	Modio::Optional<Modio::User> CurrentUser = Modio::QueryUserProfile();
	if (CurrentUser.has_value())
	{
		_currentUserValue->setText(CurrentUser->Username);
		_userSubscriptions = Modio::QueryUserSubscriptions();
		updateSubscriptionList();
	}

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserUserConfigState::think()
{
	State::think();
	if (_data->subscriptionListDirty.exchange(false))
	{
		updateSubscriptionList();
	}
	if (!_data->userProfileImagePath.empty())
	{
		_currentUserIcon->invalidate(false);
		_currentUserIcon->loadImage(_data->userProfileImagePath);
		_data->userProfileImagePath = "";
	}
}

void OpenXcom::OptionsModBrowserUserConfigState::onLogoutClicked(Action *action)
{
	Modio::ClearUserDataAsync([](Modio::ErrorCode ec) {});
	_game->popState();
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::onSwitchUserClicked(Action *action)
{
	//unimplemented yet
}

void OpenXcom::OptionsModBrowserUserConfigState::onUpdateClicked(Action *action)
{
	Modio::FetchExternalUpdatesAsync([_data = this->_data](Modio::ErrorCode ec) {
		_data->subscriptionListDirty.store(true);
	});
}

void OpenXcom::OptionsModBrowserUserConfigState::onUnsubscribeClicked(Action *action)
{
	if (_currentModIndex >= 0)
	{
		//Workaround till the mod.io API returns an indexable collection of mods
		auto it = std::next(_userSubscriptions.begin(), _currentModIndex);
		Modio::UnsubscribeFromModAsync(it->second.GetModProfile().ModId, [_data = this->_data](Modio::ErrorCode ec) {
			_data->subscriptionListDirty.store(true);
		});
	}
}

void OpenXcom::OptionsModBrowserUserConfigState::onBackClicked(Action *action)
{
	_game->popState();
}

void OpenXcom::OptionsModBrowserUserConfigState::updateSubscriptionList()
{
	_currentUserMods->clearList();
	_userSubscriptions = Modio::QueryUserSubscriptions();
	for (const std::pair<Modio::ModID, Modio::ModCollectionEntry> &ModEntry : _userSubscriptions)
	{
		_currentUserMods->addRow(1, ModEntry.second.GetModProfile().ProfileName.c_str());
	}
	_currentModIndex = -1;
	updateModActionButton();
}

void OpenXcom::OptionsModBrowserUserConfigState::updateModActionButton()
{
	if (_currentModIndex >= 0)
	{
		//Workaround till the mod.io API returns an indexable collection of mods
		auto it = std::next(_userSubscriptions.begin(), _currentModIndex);
		if ((*it).second.GetModState() == Modio::ModState::Installed)
		{
			_unsubscribeButton->setText("Uninstall");
		}
		else
		{
			_unsubscribeButton->setText("Unsubscribe");
		}
		_unsubscribeButton->setHidden(false);
	}
	else
	{
		_unsubscribeButton->setText("Unsubscribe");
		_unsubscribeButton->setHidden(true);
	}
}

void OpenXcom::OptionsModBrowserUserConfigState::onModSelected(Action *action)
{
	_currentModIndex = _currentUserMods->getSelectedRow();
	updateModActionButton();
}
