#pragma once
#include "../Engine/State.h"

namespace OpenXcom
{
class Window;
class Text;
class Surface;
class TextButton;
class TextList;
class LayoutGroup;

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
	LayoutGroup *_modListGroup;
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
	/// Handler for showing mod details when one is clicked in the list
	void onBackClicked(Action *action);

  public:
	OptionsModBrowserUserConfigState();
};
}
