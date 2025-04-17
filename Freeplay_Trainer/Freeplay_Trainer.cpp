#include "pch.h"
#include "Freeplay_Trainer.h"


BAKKESMOD_PLUGIN(Freeplay_Trainer, "Ultimate Freeplay Trainer", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Freeplay_Trainer::onLoad()
{
	_globalCvarManager = cvarManager;
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});

	cvarManager->registerCvar("enable_plugin", "0", "Enables the plugin", true, true, 0, true, 1, false).bindTo(enable);
	cvarManager->registerCvar("preset_choice", "0", "Chosen group preset to use", true, true, 0, true, 3).bindTo(chosenPres);
	loadPresets();

	inputs["XboxTypeS_DPad_Left"] = { 0, false, "DPadL", groupIndices.at(*chosenPres).at(0)};
	inputs["XboxTypeS_DPad_Right"] = { 0, false, "DPadR", groupIndices.at(*chosenPres).at(1)};
	inputs["XboxTypeS_DPad_Up"] = { 0, false, "DPadU", groupIndices.at(*chosenPres).at(2)};
	inputs["XboxTypeS_DPad_Down"] = { 0, false, "DPadD", groupIndices.at(*chosenPres).at(3)};
	inputs["One"] = { 0, false, "1", groupIndices.at(*chosenPres).at(0) };
	inputs["Two"] = { 0, false, "2", groupIndices.at(*chosenPres).at(1) };
	inputs["Three"] = { 0, false, "3", groupIndices.at(*chosenPres).at(2) };
	inputs["Four"] = { 0, false, "4", groupIndices.at(*chosenPres).at(3) };

	for (const auto& input : inputs) {
		cvarManager->registerCvar(input.first, input.first).addOnValueChanged([this](std::string old, CVarWrapper now) {
			inputs[now.getStringValue()].index = gameWrapper->GetFNameIndexByString(now.getStringValue());
			});
		cvarManager->getCvar(input.first).notify();
	}

	gameWrapper->HookEvent("Function Engine.GameViewportClient.Tick", bind(&Freeplay_Trainer::onTick, this, std::placeholders::_1));
}

void Freeplay_Trainer::onUnload() {
	savePresets(All);
}

void Freeplay_Trainer::onTick(string eventName) {
	if (!gameWrapper->IsInFreeplay()) { return; }
	if (!*enable) { return; }
	bool anyPressed = false;
	for (auto const& entry : inputs) {
		if (entry.second.index > 0) {
			inputs[entry.first].pressed = gameWrapper->IsKeyPressed(entry.second.index);
			if (inputs[entry.first].pressed) {
				anyPressed = true;
			}
		}
	}

	//Ensures that we only need to find the ball and the car when input is detected
	if (anyPressed) {
		shotHandler();
	}
}

void Freeplay_Trainer::shotHandler(){

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	BallWrapper ball = server.GetBall();
	if (!ball) { return; }
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }

	if (inputs["XboxTypeS_DPad_Left"].pressed || inputs["One"].pressed) {
		inputs["One"].shotIndex;
	}
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

Vector Freeplay_Trainer::ConvertWorldToLocal(Vector A_pos, Rotator A_rot,Vector B_pos, bool ballLocked)
{	
	Vector vecOff = {};

	//Allows user to choose type of parent relation from A to B
	if (ballLocked) {
		// Build A’s orientation basis
		Quat offset(0, B_pos.X, B_pos.Y, B_pos.Z);
		Quat AQuat = RotatorToQuat(A_rot);
		AQuat = AQuat.normalize();
		AQuat = AQuat.conjugate();

		Quat QuatOff = (RotatorToQuat(A_rot) * offset * AQuat);
		vecOff = { QuatOff.X, QuatOff.Y, QuatOff.Z };
		vecOff += A_pos;
	}
	else {
		/*Vector nonZ = { B_pos.X,B_pos.Y, 0 };
		float mag = nonZ.magnitude();*/
		
		float yaw = RotYawToRad(A_rot);
		Vector rotOff = { B_pos.X * cos(yaw) - B_pos.Y * sin(yaw), B_pos.X * sin(yaw) + B_pos.Y * cos(yaw) , B_pos.Z };
		vecOff = { rotOff.X, rotOff.Y, rotOff.Z };
		vecOff += A_pos;
	}
	
	return vecOff;
}

float Freeplay_Trainer::DegToRad(float degrees) {
	return degrees * 3.1415 / 180;
}
float Freeplay_Trainer::RotYawToRad(Rotator rot) {
	const int PI = 3.1415;
	float yaw = rot.Yaw;

	if (yaw < 0) {
		yaw += 2*32768;
	}

	yaw -= 2 * 32768;
	yaw = yaw * PI / 32768;

	return yaw ;
}