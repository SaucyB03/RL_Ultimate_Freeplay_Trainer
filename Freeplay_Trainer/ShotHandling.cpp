#include "pch.h"
#include "Freeplay_Trainer.h"

//Retrieves shot index mapped to current preset and then runs the shot handler on that index
void Freeplay_Trainer::InputHandler() {
	if (inputs["XboxTypeS_DPad_Left"].pressed || inputs["One"].pressed) {
		ShotHandler(inputs["One"].shotIndex);

	}else if (inputs["XboxTypeS_DPad_Right"].pressed || inputs["Two"].pressed) {
		ShotHandler(inputs["Two"].shotIndex);

	}else if (inputs["XboxTypeS_DPad_Up"].pressed || inputs["Three"].pressed) {
		ShotHandler(inputs["Three"].shotIndex);

	}else if (inputs["XboxTypeS_DPad_Down"].pressed || inputs["Four"].pressed) {
		ShotHandler(inputs["Four"].shotIndex);
	}
}

// Recieves the index of the shot to run. Then spawns the ball ingame according to these values
void Freeplay_Trainer::ShotHandler(int shotIndex) {
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	BallWrapper ball = server.GetBall();
	if (!ball) { return; }
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }

	vector<float> loc = initPosAll.at(shotIndex);
	Vector ballLoc = { loc.at(0),loc.at(1),loc.at(2) };
	ball.SetLocation(ballLoc);
}
