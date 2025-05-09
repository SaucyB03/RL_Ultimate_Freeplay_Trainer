#include "pch.h"
#include "Freeplay_Trainer.h"


BAKKESMOD_PLUGIN(Freeplay_Trainer, "Ultimate Freeplay Trainer", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Freeplay_Trainer::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerNotifier(NOTIFIER_KEY_PRESETS, [this](std::vector<std::string> params) {InputHandler(params);}, "Determines key press", PERMISSION_ALL);

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {Render(canvas);});

	cvarManager->registerCvar("enable_UTF_plugin", "0", "Enables the plugin", true, true, 0, true, 1, false).bindTo(enable);
	cvarManager->registerCvar("preset_choice", "0", "Chosen group preset to use", true, true, 0, true, 3, true).bindTo(chosenPres);
	
	loadPresets();

	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&Freeplay_Trainer::onTick, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [&](std::string eventName) {hold_ball = false;});

	signs->at(0) = 1.0;
	signs->at(1) = 1.0;
	
	initRand();
}

void Freeplay_Trainer::onUnload() {
	savePresets();
}


void Freeplay_Trainer::onTick(string eventName) {
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	BallWrapper ball = server.GetBall();
	if (!ball) { return; }
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }

	CheckBallLock(ball, car);
}

//Opens json save file, then converts and stores each associated vector
void Freeplay_Trainer::loadPresets() {
	ifstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "presetscfg.json");
	if (!file.is_open()) {
		LOG("Failed to load presetscfg.json");
		onUnload();
		return;
	}
	nlohmann::json j;
	file >> j;
	
	names = j["names"].get<vector<string>>();
	initPosAll = j["position"].get<vector<vector<float>>>();
	rel_to = j["relation"].get<vector<int>>();
	speeds = j["speeds"].get<vector<float>>();
	willFreeze = j["freeze"].get<vector<bool>>();
	addVel = j["addVelocity"].get<vector<bool>>();
	initDir = j["direction"].get<vector<vector<float>>>();
	dirMode = j["directionMode"].get<vector<bool>>();
	shootAt = j["shootAt"].get<vector<int>>();
	timeTo = j["time"].get<vector<float>>();
	leadOff = j["leadOff"].get<vector<float>>();
	mirror = j["mirror"].get< vector<vector<int>>>();
	usingDirVar = j["usingDirVar"].get<vector<bool>>();
	variance = j["variance"].get<vector<float>>();
	usingPosVar = j["usingPosVar"].get<vector<bool>>();
	posVarShape = j["posVarShape"].get<vector<int>>();
	cuboid = j["cuboid"].get<vector<vector<float>>>();
	sphere = j["sphere"].get<vector<float>>();
	groupIndices = j["groupIndices"].get<vector<vector<int>>>();
	groupNames = j["groupNames"].get<vector<string>>();
	colors = VecFloatToVecLinearColor(j["colors"].get<vector<vector<float>>>());
}

//Opens json save file then dumps contents of vectors into it
void Freeplay_Trainer::savePresets() {
	nlohmann::json j;

	j["names"] = names;
	j["position"] = initPosAll;
	j["relation"] = rel_to;
	j["speeds"] = speeds;
	j["freeze"] = willFreeze;
	j["addVelocity"] = addVel;
	j["directionMode"] = dirMode;
	j["shootAt"] = shootAt;
	j["time"] = timeTo;
	j["leadOff"] = leadOff;
	j["direction"] = initDir;
	j["mirror"] = mirror;
	j["usingDirVar"] = usingDirVar;
	j["variance"] = variance;
	j["usingPosVar"] = usingPosVar;
	j["posVarShape"] = posVarShape;
	j["cuboid"] = cuboid;
	j["sphere"] = sphere;

	j["groupIndices"] = groupIndices;
	j["groupNames"] = groupNames;

	j["colors"] = VecLinearColorToVecFloat(colors);

	ofstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "presetscfg.json");
	file << j.dump(4);
}

void Freeplay_Trainer::resetPreset(int shotIndex) {
	ifstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "defaultcfg.json");
	if (!file.is_open()) {
		LOG("Failed to load defaultcfg.json");
		return;
	}
	nlohmann::json j;
	file >> j;

	if (shotIndex == -1) {
		//Resets all presets
		names = j["names"].get<vector<string>>();
		initPosAll = j["position"].get<vector<vector<float>>>();
		rel_to = j["relation"].get<vector<int>>();
		speeds = j["speeds"].get<vector<float>>();
		willFreeze = j["freeze"].get<vector<bool>>();
		addVel = j["addVelocity"].get<vector<bool>>();
		initDir = j["direction"].get<vector<vector<float>>>();
		dirMode = j["directionMode"].get<vector<bool>>();
		shootAt = j["shootAt"].get<vector<int>>();
		timeTo = j["time"].get<vector<float>>();
		leadOff = j["leadOff"].get<vector<float>>();
		mirror = j["mirror"].get< vector<vector<int>>>();
		usingDirVar = j["usingDirVar"].get<vector<bool>>();
		variance = j["variance"].get<vector<float>>();
		usingPosVar = j["usingPosVar"].get<vector<bool>>();
		posVarShape = j["posVarShape"].get<vector<int>>();
		cuboid = j["cuboid"].get<vector<vector<float>>>();
		sphere = j["sphere"].get<vector<float>>();
		groupIndices = j["groupIndices"].get<vector<vector<int>>>();
		groupNames = j["groupNames"].get<vector<string>>();
		colors = VecFloatToVecLinearColor(j["colors"].get<vector<vector<float>>>());
	}
	else {
		//Only resets the cur_shot preset
		names.at(shotIndex) = j["names"].get<vector<string>>().at(shotIndex);
		initPosAll.at(shotIndex) = j["position"].get<vector<vector<float>>>().at(shotIndex);
		rel_to.at(shotIndex) = j["relation"].get<vector<int>>().at(shotIndex);
		speeds.at(shotIndex) = j["speeds"].get<vector<float>>().at(shotIndex);
		willFreeze.at(shotIndex) = j["freeze"].get<vector<bool>>().at(shotIndex);
		addVel.at(shotIndex) = j["addVelocity"].get<vector<bool>>().at(shotIndex);
		initDir.at(shotIndex) = j["direction"].get<vector<vector<float>>>().at(shotIndex);
		dirMode.at(shotIndex) = j["directionMode"].get<vector<bool>>().at(shotIndex);
		shootAt.at(shotIndex) = j["shootAt"].get<vector<int>>().at(shotIndex);
		timeTo.at(shotIndex) = j["time"].get<vector<float>>().at(shotIndex);
		leadOff.at(shotIndex) = j["leadOff"].get<vector<float>>().at(shotIndex);
		mirror.at(shotIndex) = j["mirror"].get< vector<vector<int>>>().at(shotIndex);
		usingDirVar.at(shotIndex) = j["usingDirVar"].get<vector<bool>>().at(shotIndex);
		variance.at(shotIndex) = j["variance"].get<vector<float>>().at(shotIndex);
		usingPosVar.at(shotIndex) = j["usingPosVar"].get<vector<bool>>().at(shotIndex);
		posVarShape.at(shotIndex) = j["posVarShape"].get<vector<int>>().at(shotIndex);
		cuboid.at(shotIndex) = j["cuboid"].get<vector<vector<float>>>().at(shotIndex);
		sphere.at(shotIndex) = j["sphere"].get<vector<float>>().at(shotIndex);
		groupIndices.at(shotIndex) = j["groupIndices"].get<vector<vector<int>>>().at(shotIndex);
		groupNames.at(shotIndex) = j["groupNames"].get<vector<string>>().at(shotIndex);
		colors.at(shotIndex) = VecFloatToVecLinearColor(j["colors"].get<vector<vector<float>>>()).at(shotIndex);
	}

	savePresets();
}


void Freeplay_Trainer::initRand() {
	std::random_device rd;
	gen.seed(rd());
}

float Freeplay_Trainer::getRandFloat(float min, float max) {
	if (max - min == 0) {
		return 0;
	}
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}
