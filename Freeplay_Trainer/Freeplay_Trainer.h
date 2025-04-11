#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "json.hpp"
using json = nlohmann::json;


#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include "version.h"

using namespace std;

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr int MAX_SIZE = 16;
constexpr int MAX_GROUP = 4;
constexpr int NUM_KEYB = 4;

class Freeplay_Trainer : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
{

	//shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;
	void onTick(string eventName);


public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
private:
	//Easier way to keep track of whats being saved
	enum SaveType {
		All = 0,
		Shots_Only = 1,
		Groups_Only = 2,
	};

	void loadPresets();
	void savePresets(SaveType saveType);
	bool errorCheck();

	vector<string> names;
	vector<vector<float>> initPosAll;
	vector<int> rel_to;
	vector<float> speeds;
	vector<vector<float>> initDir;
	vector<bool> usingDirVar;
	vector<float> variance;
	vector<bool> usingPosVar;
	vector<int> posVarShape;

	// For group Presets: Index/keyboard keybind:D-pad keybind 
	//  1:Left | 2:Right | 3:Up | 4:Down
	vector<vector<int>> groupIndices;
	vector<string> groupNames;

	shared_ptr<int> chosenPres = make_shared<int>(0);
	shared_ptr<bool> enable = make_shared<bool>(false);
};
