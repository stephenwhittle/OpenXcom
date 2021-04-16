#include "OptionsModBrowserAuthState.h"
#include "../Interface/Window.h"
#include "../Interface/TextEdit.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "modio/ModioSDK.h"

void OpenXcom::OptionsModBrowserAuthState::think()
{
	State::think();
}

OpenXcom::OptionsModBrowserAuthState::OptionsModBrowserAuthState()
{
	_window = new Window(this, 320, 200, 0, 0, POPUP_VERTICAL);
	_authRequiredText = new Text(240, 100);
	_emailAddrInput = new TextEdit(this, 240, 16);
	_sendCodeRequestBtn = new TextButton(50, 16);
	_authCodeInput = new TextEdit(this, 240, 16);
	_submitAuthBtn = new TextButton(50, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_authRequiredText, "text", "optionsMenu");
	add(_emailAddrInput, "text1", "optionsMenu");
	add(_sendCodeRequestBtn, "button", "optionsMenu");
	add(_authCodeInput, "text1", "optionsMenu");
	add(_submitAuthBtn, "button", "optionsMenu");
}
