#pragma once
#include "../Engine/State.h"
#include "modio/detail/ModioDefines.h"
#include <map>
#include <memory>
#include <atomic>

namespace Modio
{
class ModID;
class ModCollectionEntry;
}

namespace OpenXcom
{
class Window;
class Text;
class Surface;
class TextButton;
class TextList;
class LayoutGroup;

/// @brief State showing the currently logged in user and a list of their mods so they can unsubscribe/uninstall from them
class OptionsModBrowserUserConfigState : public State
{
  private:
	Window *_window;
	Text *_currentUserLabel;
	Surface *_currentUserIcon;
	Text *_currentUserValue;
	LayoutGroup *_userProfileGroup;
	Text *_currentModsLabel;
	TextList *_currentUserMods;
	TextButton *_unsubscribeButton;
	Surface *_modListSeparator;
	LayoutGroup *_userGroup;
	TextButton *_logoutButton;
	TextButton *_switchUserButton;
	TextButton *_updateCheckButton;
	//Probably also want a button to bring up the full list of system installations and manage those too
	Surface *_ButtonSeparator;
	TextButton *_backButton;
	LayoutGroup *_buttonGroup;
	/// Handler for showing mod details when one is clicked in the list
	void onLogoutClicked(Action *action);
	/// Handler for showing mod details when one is clicked in the list
	void onSwitchUserClicked(Action *action);
	void onUpdateClicked(Action *action);
	void onUnsubscribeClicked(Action *action);
	/// Handler for showing mod details when one is clicked in the list
	void onBackClicked(Action *action);
	std::map<Modio::ModID, Modio::ModCollectionEntry> _userSubscriptions;
	void updateSubscriptionList();
	void updateModActionButton();
	void onModSelected(Action *action);
	int _currentModIndex = -1;
	struct StateData
	{
		std::atomic_bool subscriptionListDirty = false;
		std::string userProfileImagePath;
	};
	/// @brief Backing data/notification flags for the state.
	/// stored in a shared pointer so if the window is closed while a
	/// mod.io SDK call is in-flight, the callback won't have a
	/// dangling reference to the state itself
	std::shared_ptr<StateData> _data;

  public:
	OptionsModBrowserUserConfigState();

	void think() override;
};
}
