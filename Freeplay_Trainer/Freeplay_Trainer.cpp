#include "pch.h"
#include "Freeplay_Trainer.h"


BAKKESMOD_PLUGIN(Freeplay_Trainer, "Ultimate Freeplay Trainer", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Freeplay_Trainer::onLoad()
{
	_globalCvarManager = cvarManager;

	cvarManager->registerNotifier(NOTIFIER_KEY_PRESETS, [this](std::vector<std::string> params) {InputHandler(params);}, "Determines key press", PERMISSION_ALL);

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});

	cvarManager->registerCvar("enable_UTF_plugin", "0", "Enables the plugin", true, true, 0, true, 1, false).bindTo(enable);
	cvarManager->registerCvar("preset_choice", "0", "Chosen group preset to use", true, true, 0, true, 3).bindTo(chosenPres);
	loadPresets();

	updateKeybinds();

	for (const auto& input : inputs) {
		cvarManager->registerCvar(input.first, input.first).addOnValueChanged([this](std::string old, CVarWrapper now) {
			inputs[now.getStringValue()].index = gameWrapper->GetFNameIndexByString(now.getStringValue());
			});
		cvarManager->getCvar(input.first).notify();
	}

	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&Freeplay_Trainer::onTick, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnCarTouch", [&](std::string eventName) {hold_ball = false;});
}

void Freeplay_Trainer::onUnload() {
	savePresets();
}

//void DribbleTrainer::RequestToggle(std::vector<std::string> params)
//{
//	if (params.size() != 2 || !gameWrapper->IsInFreeplay())
//		return;
//
//	if (params.at(1) == "dribble")
//	{
//		cvarManager->getCvar(CVAR_TOGGLE_DRIBBLE_MODE).setValue(!(*bEnableDribbleMode));
//	}
//	if (params.at(1) == "flick")
//	{
//		cvarManager->getCvar(CVAR_TOGGLE_FLICKS_MODE).setValue(!(*bEnableFlicksMode));
//	}
//}

void Freeplay_Trainer::onTick(string eventName) {
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	BallWrapper ball = server.GetBall();
	if (!ball) { return; }

	//if (!gameWrapper->IsInFreeplay()) { return; }
	//if (!*enable) { return; }
	//bool anyPressed = false;
	//for (auto const& entry : inputs) {
	//	if (entry.second.index > 0) {
	//		inputs[entry.first].pressed = gameWrapper->IsKeyPressed(entry.second.index);
	//		if (inputs[entry.first].pressed) {
	//			anyPressed = true;
	//		}
	//	}
	//}

	//Ensures that we only need to find the ball and the car when input is detected
	//if (anyPressed) {
	//	InputHandler();
	//}
	checkBallLock(ball);
}

void Freeplay_Trainer::loadPresets() {
	ifstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "presetscfg.json");
	if (!file.is_open()) {
		LOG("Failed to load presetscfg.json");
		onUnload();
		return;
	}
	nlohmann::json j;
	file >> j;
	
	// Preset Names:
	names = j["names"].get<vector<string>>();

	// Initial Positions:
	initPosAll = j["position"].get<vector<vector<float>>>();
	
	//Positional Relations:
	rel_to = j["relation"].get<vector<int>>();

	//Speeds
	speeds = j["speeds"].get<vector<float>>();

	//Initial direction
	initDir = j["direction"].get<vector<vector<float>>>();

	//Using Direcional Variance
	usingDirVar = j["usingDirVar"].get<vector<bool>>();

	//Variance
	variance = j["variance"].get<vector<float>>();

	//usingPosVar
	usingPosVar = j["usingPosVar"].get<vector<bool>>();

	//posVarShape
	posVarShape = j["posVarShape"].get<vector<int>>();
	cuboid = j["cuboid"].get<vector<vector<float>>>();
	sphere = j["sphere"].get<vector<float>>();

	//Indices for group preset binds
	groupIndices = j["groupIndices"].get<vector<vector<int>>>();
	groupNames = j["groupNames"].get<vector<string>>();
}

void Freeplay_Trainer::savePresets() {
	nlohmann::json j;

	j["names"] = names;
	j["position"] = initPosAll;
	j["relation"] = rel_to;
	j["speeds"] = speeds;
	j["direction"] = initDir;
	j["usingDirVar"] = usingDirVar;
	j["variance"] = variance;
	j["usingPosVar"] = usingPosVar;
	j["posVarShape"] = posVarShape;
	j["cuboid"] = cuboid;
	j["sphere"] = sphere;

	j["groupIndices"] = groupIndices;
	j["groupNames"] = groupNames;

	ofstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "presetscfg.json");
	file << j.dump(4);
}

void Freeplay_Trainer::updateKeybinds() {
	inputs["XboxTypeS_DPad_Left"] = { 0, false, "DPadL", groupIndices.at(*chosenPres).at(0) };
	inputs["XboxTypeS_DPad_Right"] = { 0, false, "DPadR", groupIndices.at(*chosenPres).at(1) };
	inputs["XboxTypeS_DPad_Up"] = { 0, false, "DPadU", groupIndices.at(*chosenPres).at(2) };
	inputs["XboxTypeS_DPad_Down"] = { 0, false, "DPadD", groupIndices.at(*chosenPres).at(3) };
	inputs["One"] = { 0, false, "1", groupIndices.at(*chosenPres).at(0) };
	inputs["Two"] = { 0, false, "2", groupIndices.at(*chosenPres).at(1) };
	inputs["Three"] = { 0, false, "3", groupIndices.at(*chosenPres).at(2) };
	inputs["Four"] = { 0, false, "4", groupIndices.at(*chosenPres).at(3) };
}
