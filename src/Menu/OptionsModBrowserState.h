#pragma once

#include "../Engine/State.h"


namespace OpenXcom
{
class Window;
class Text;
class TextList;
class TextEdit;
class TextButton;
class ComboBox;
class LayoutGroup;

class OptionsModBrowserState : public State
{
private:
	Window *_window;
	TextList *_modList;
	TextEdit* _searchText;
	TextButton* _searchButton;
	LayoutGroup* _searchBar;

public:
	OptionsModBrowserState();
	void init() override;

	void think() override;

};
}
