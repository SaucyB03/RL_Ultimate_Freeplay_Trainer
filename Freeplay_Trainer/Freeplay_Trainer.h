#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "RenderingTools/Objects/Frustum.h"
#include "json.hpp"

#include "constants.h"

using json = nlohmann::json;


#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <random>

#include "version.h"

#define NOTIFIER_KEY_PRESETS "ShotBind"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


using namespace std;

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

	//Freplay_Trainer_Settings.cpp
	bool ErrorCheck();

	//Rendering.cpp
	void RenderShotIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, RelativeOffset rel);
	void RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, RelativeOffset rel);

	//ShotHandling.cpp
	void InputHandler(std::vector<std::string> params);
	void ShotHandler(int shotIndex);
	void CheckBallLock(BallWrapper ball, CarWrapper car);
	void VaryInitialDir(RelativeOffset& rel, int shotIndex);
	void VaryInitialPos(RelativeOffset& rel, int shotIndex);
	vector<float> MirrorHandler(RelativeOffset& rel, CarWrapper car, bool isRender, int shotIndex);

	//Conversions_Calculations.cpp
	RelativeOffset* CalculateOffsets(CarWrapper car, BallWrapper ball, int shotIndex, bool isRender);
	Vector ConvertWorldToLocal(Vector A_pos, Rotator A_rot, Vector B_pos, bool locked);
	Vector CalcKinematics(Vector start, Vector end, int shotIndex);
	Vector CalcKinematics(Vector start, CarWrapper car, int shotIndex);
	void CalcShootAt(RelativeOffset& rel, CarWrapper car, vector<float> sign, int shotIndex);
	float DegToRad(float degrees);
	float RotYawToRad(Rotator rot);
	Vector VecToVector(vector<float> vec);
	Vector VecToVector(vector<vector<float>> vec, int index);
	Vector2F VecToVector2(vector<float> vector);
	Vector2F VecToVector2(vector<vector<float>> vec, int index);
	vector<LinearColor> VecFloatToVecLinearColor(vector<vector<float>> vec);
	vector<vector<float>> VecLinearColorToVecFloat(vector<LinearColor> vec);
	ImVec4 VecLinearColorToVec4(vector<LinearColor> color, int index);
	void AssignLinearColorFromVec4(ImVec4 color, int index);
	
	// Randomness handling
	void initRand();
	float getRandFloat(float min, float max);
	std::mt19937 gen;
	
	//All necessary saveable variables
	vector<string> names;
	vector<vector<float>> initPosAll;
	vector<int> rel_to;
	vector<float> speeds;
	vector<bool> willFreeze;
	vector<bool> addVel;
	vector<bool> dirMode;
	vector<int> shootAt;
	vector<float> timeTo;
	vector<float> leadOff;
	vector<vector<float>> initDir;
	vector<vector<int>> mirror;
	vector<bool> usingDirVar;
	vector<float> variance;
	vector<bool> usingPosVar;
	vector<int> posVarShape;
	vector<vector<float>> cuboid;
	vector<float> sphere;
	vector<LinearColor> colors;

	//Flags for indicators
	bool togAll = false;
	bool posVarInd = false;
	bool dirVarInd = false;
	bool ball_indicator = false;
	bool line_indicator = false;
	bool ballLocked = false;
	bool arrowLocked = false;
	bool freeze = false;

	//Temp variables during run (avoids overwriting saved variables)
	Vector targetPos = {0,0,0};
	Vector curPos = { 0,0,0 };
	int cur_shot = 0;
	float cur_speed = 0.0f;
	vector<float>* signs = new vector<float>(2);


	//When true locks ball in place
	bool hold_ball = false;
	int cur_shot_index = 0;


	// For group Presets: Index/keyboard keybind:D-pad keybind 
	//  1:Left | 2:Right | 3:Up | 4:Down
	vector<vector<int>> groupIndices;
	vector<string> groupNames;

	shared_ptr<int> chosenPres = make_shared<int>(0);
	shared_ptr<bool> enable = make_shared<bool>(false);

};
