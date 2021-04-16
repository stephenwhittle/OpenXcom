#pragma once

#include "../Engine/State.h"
#include "OptionsModBrowserAuthState.h"


namespace OpenXcom
{
class Window;
class Text;
class TextList;
class TextButton;
class ComboBox;

class OptionsModBrowserState : public State
{
private:
	Window *_window;
	TextList *_modList;

public:
	void init() override;

	void think() override;

};
}
