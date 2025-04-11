#include "pch.h"
#include "Freeplay_Trainer.h"


BAKKESMOD_PLUGIN(Freeplay_Trainer, "Ultimate Freeplay Trainer", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Freeplay_Trainer::onLoad()
{
	_globalCvarManager = cvarManager;
	cvarManager->registerCvar("enable_plugin", "0", "Enables the plugin", true, true, 0, true, 1, false).bindTo(chosenPres);
	cvarManager->registerCvar("preset_choice", "0", "Chosen group preset to use", true, true, 0, true, 3).bindTo(chosenPres);
	loadPresets();
	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&Freeplay_Trainer::onTick, this, std::placeholders::_1));
}

void Freeplay_Trainer::onUnload() {
	savePresets(All);
}

void Freeplay_Trainer::onTick(string eventName) {
	if (!gameWrapper->IsInFreeplay()) { return; }
	if (!enable) { return; }

	return;
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

	//Indices for group preset binds
	groupIndices = j["groupIndices"].get<vector<vector<int>>>();
	groupNames = j["groupNames"].get<vector<string>>();
}

void Freeplay_Trainer::savePresets(SaveType saveType) {
	nlohmann::json j;

	//Minimizes whats being saved to only whats necessary:
	if(saveType == 0 || saveType == 1){
		j["names"] = names;
		j["position"] = initPosAll;
		j["relation"] = rel_to;
		j["speeds"] = speeds;
		j["direction"] = initDir;
		j["usingDirVar"] = usingDirVar;
		j["variance"] = variance;
		j["usingPosVar"] = usingPosVar;
		j["posVarShape"] = posVarShape;
	}


	if (saveType == 0 || saveType == 2) {
		j["groupIndices"] = groupIndices;
		j["groupNames"] = groupNames;
	}

	ofstream file(gameWrapper->GetDataFolder() / "Freeplay_Trainer" / "presetscfg.json");
	file << j.dump(4);
}
