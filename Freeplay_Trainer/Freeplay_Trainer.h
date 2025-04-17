#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "RenderingTools/Objects/Frustum.h"
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
constexpr float BALL_RADIUS = 91.25;
constexpr float IND_ARR_RATIO = 5.12;

//Referencign Haltepunkt's / AreUThreateningme's method for input handling on keyboard / dpad:
//https://github.com/kregerl/BakkesKBMOverlay
//https://github.com/haltepunkt/ControllerOverlay/tree/master
struct Input {
	int index;
	bool pressed;
	std::string name;
	int shotIndex;
};


class Freeplay_Trainer : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
{

	//shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;
	void Render(CanvasWrapper canvas);
	void onTick(string eventName);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
private:
	//Freeplay_Trainer.cpp
	void loadPresets();
	void savePresets();
	Vector ConvertWorldToLocal(Vector A_pos, Rotator A_rot, Vector B_pos, bool locked);
	float DegToRad(float degrees);
	float RotYawToRad(Rotator rot);
	Vector VecToVector(vector<float> vector);
	Vector VecToVector(vector<vector<float>> vector, int index);

	//Freplay_Trainer_Settings.cpp
	bool ErrorCheck();

	//Rendering.cpp
	void RenderShotIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir, Quat rotation);
	void RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir);

	//ShotHandling.cpp
	void InputHandler();
	void ShotHandler(int shotIndex);

	
	//All necessary saveable variables
	vector<string> names;
	vector<vector<float>> initPosAll;
	vector<int> rel_to;
	vector<float> speeds;
	vector<vector<float>> initDir;
	vector<bool> usingDirVar;
	vector<float> variance;
	vector<bool> usingPosVar;
	vector<int> posVarShape;
	vector<vector<float>> cuboid;
	vector<float> sphere;
	bool ball_indicator = false;
	bool line_indicator = false;
	int cur_shot = 0;

	//Temp values for indicators:
	//Vector pos;
	//Vector dir;
	//float init_speed;
	//float rel;
	//bool dirV;
	//float curVar;
	//bool posV;
	//int shape;
	//Vector cuboid = {0,0,0};
	//float sphere;
	bool ballLocked;
	bool arrowLocked;


	// For group Presets: Index/keyboard keybind:D-pad keybind 
	//  1:Left | 2:Right | 3:Up | 4:Down
	vector<vector<int>> groupIndices;
	vector<string> groupNames;

	shared_ptr<int> chosenPres = make_shared<int>(0);
	shared_ptr<bool> enable = make_shared<bool>(false);

	std::map<std::string, Input> inputs;

};
