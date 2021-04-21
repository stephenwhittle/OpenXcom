#pragma once

#include "../Engine/State.h"
//TODO@modio-sdk : hacky, need to remove when we work out why asio's defines aren't being set correctly
#include "modio/detail/ModioDefines.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/core/ModioStdTypes.h" //For Modio::Optional


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

	LayoutGroup* _details;
	Text* _modName;
	Text* _modUpdated;
	Text* _modCreated;
	LayoutGroup* _detailsHeader;
	Text* _modDesc;

	Modio::Optional<Modio::ModInfoList> _currentModResults;
	void UpdateModList();
	void updateModDetails(Modio::ModInfo modDetails);
public:
	OptionsModBrowserState();
	void init() override;

	void think() override;
	/// Handler for showing mod details when one is clicked in the list
	void onModSelected(Action* action);
	/// Handler for clicking the search button
	void onSearchClicked(Action* action);
	/// Handler for clicking the subscribe button for a mod
	void onSubscribeClicked(Action* action);
};
}
