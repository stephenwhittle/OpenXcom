#pragma once

#include "../Engine/State.h"


namespace OpenXcom
{
class Window;
class Text;
class TextButton;
class TextEdit;

class OptionsModBrowserAuthState : public State
{
private:
	Window* _window;
	Text* _authRequiredText;
	TextEdit* _emailAddrInput;
	TextButton* _sendCodeRequestBtn;
	TextEdit* _authCodeInput;
	TextButton* _submitAuthBtn;

public:
	void think() override;
	OptionsModBrowserAuthState();
};
}
