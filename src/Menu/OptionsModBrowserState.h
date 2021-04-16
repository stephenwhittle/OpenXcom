#pragma once

#include "../Engine/State.h"

#include "modio/ModioSDK.h"

namespace OpenXcom
{
class Window;
class Text;
class TextList;
class TextButton;
class ComboBox;

class OptionsModBrowserState L public State
{
private:
	Window *_window;
	TextList *_modList;

};
}
