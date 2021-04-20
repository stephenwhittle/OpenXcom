#pragma once

#include "../Engine/State.h"


namespace OpenXcom
{
class Window;
class Text;
class TextButton;
class TextEdit;
class LayoutGroup;
class OptionsModBrowserAuthState : public State
{
private:
	Window* _window;
	InteractiveSurface* _modioLogo;
	Text* _authRequiredText;
	TextEdit* _emailAddrInput;
	TextButton* _sendCodeRequestBtn;
	TextEdit* _authCodeInput;
	TextButton* _submitAuthBtn;
	LayoutGroup* _emailWidgetGroup;
	LayoutGroup* _codeWidgetGroup;
	/// Handler for requesting an email auth code.
	void submitEmailClick(Action* action);
	/// Handler for submitting an email auth code.
	void submitAuthCodeClick(Action* action);

public:
	void think() override;
	OptionsModBrowserAuthState();
};
}
