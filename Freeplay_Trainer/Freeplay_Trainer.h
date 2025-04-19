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
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>

#include "version.h"

#define NOTIFIER_KEY_PRESETS "ShotBind"

using namespace std;

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr int MAX_SIZE = 16;
constexpr int MAX_GROUP = 4;
constexpr int NUM_KEYB = 4;
constexpr float BALL_RADIUS = 91.25;
constexpr float IND_ARR_RATIO = 5.12;
constexpr float KPH_TO_BALL_VEL = 27.90698;
constexpr float PI = 3.1415;

//Referencign Haltepunkt's / AreUThreateningme's method for input handling on keyboard / dpad:
//https://github.com/kregerl/BakkesKBMOverlay
//https://github.com/haltepunkt/ControllerOverlay/tree/master
struct Input {
	int index;
	bool pressed;
	std::string name;
	int shotIndex;
};

struct RelativeOffset {
	Vector offPos;
	Vector offDir;
	Vector unitVec;
	Quat rotation;
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
	void updateKeybinds();

	//Freplay_Trainer_Settings.cpp
	bool ErrorCheck();

	//Rendering.cpp
	void RenderShotIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir, Quat rotation);
	void RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir);

	//ShotHandling.cpp
	void InputHandler(std::vector<std::string> params);
	void ShotHandler(int shotIndex);
	void CheckBallLock(BallWrapper ball);
	Vector VaryInitialDir();
	Vector VaryInitialPos();

	//Conversions_Calculations.cpp
	RelativeOffset CalculateOffsets(CarWrapper car, int shotindex);
	Vector ConvertWorldToLocal(Vector A_pos, Rotator A_rot, Vector B_pos, bool locked);
	float DegToRad(float degrees);
	float RotYawToRad(Rotator rot);
	Vector VecToVector(vector<float> vector);
	Vector VecToVector(vector<vector<float>> vector, int index);
	Vector2F VecToVector2(vector<float> vector);
	Vector2F VecToVector2(vector<vector<float>> vector, int index);

	
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

	bool ballLocked;
	bool arrowLocked;

	//When true locks ball in place
	bool hold_ball = false;
	int cur_shot_index = 0;


	// For group Presets: Index/keyboard keybind:D-pad keybind 
	//  1:Left | 2:Right | 3:Up | 4:Down
	vector<vector<int>> groupIndices;
	vector<string> groupNames;

	shared_ptr<int> chosenPres = make_shared<int>(0);
	shared_ptr<bool> enable = make_shared<bool>(false);

	std::map<std::string, Input> inputs;

};
