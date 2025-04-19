#include "pch.h"
#include "Freeplay_Trainer.h"

//Retrieves shot index mapped to current preset and then runs the shot handler on that index
void Freeplay_Trainer::InputHandler(std::vector<std::string> params) {
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }
	BallWrapper ball = server.GetBall();
	if (!ball) { return; }

	if (params.size() != 2 || !gameWrapper->IsInFreeplay()) { return; }

	if (params.at(1) == "left1") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(0) - 1);
		cur_shot_index = groupIndices.at(*chosenPres).at(0) - 1;

	}else if (params.at(1) == "right2") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(1) - 1);
		cur_shot_index = groupIndices.at(*chosenPres).at(1) - 1;

	}else if (params.at(1) == "up3") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(2) - 1);
		cur_shot_index = groupIndices.at(*chosenPres).at(2 - 1);

	}else if (params.at(1) == "down4") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(3) - 1);
		cur_shot_index = groupIndices.at(*chosenPres).at(3) - 1;
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

	//Retrieve position + speed for ball
	vector<float> loc = initPosAll.at(shotIndex);
	Vector ballLoc = { loc.at(0),loc.at(1),loc.at(2) };

	float speed = speeds.at(shotIndex) * KPH_TO_BALL_VEL;

	// TODO: Implement the following:
	/*
	- Relative-to ball spawns
	- positional and directional variance (do I want speed variance?)

	*/

	if (speed != 0) {
		RelativeOffset rel = CalculateOffsets(car, shotIndex);

		ball.SetLocation(rel.offPos);

		//Vector2F dirRad = { DegToRad(offsetDir.X),DegToRad(offsetDir.Y) };
		//Vector unitVec = { cos(dirRad.X) * cos(dirRad.Y), sin(dirRad.X) * cos(dirRad.Y), sin(dirRad.Y) };


		ball.SetVelocity(rel.unitVec * speed);
		ball.SetAngularVelocity({ 0,0,0 }, false);
		
	}
	else {
		//Lock ball in location
		hold_ball = true;
	}

}

void Freeplay_Trainer::CheckBallLock(BallWrapper ball) {
	if (hold_ball) {
		vector<float> loc = initPosAll.at(cur_shot_index);
		Vector ballLoc = { loc.at(0),loc.at(1),loc.at(2) };
		ball.SetLocation(ballLoc);
		ball.SetVelocity({ 0,0,0 });
		ball.SetAngularVelocity({ 0,0,0 }, false);
	}
}
