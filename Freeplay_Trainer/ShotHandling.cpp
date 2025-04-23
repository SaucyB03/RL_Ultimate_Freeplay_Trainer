#include "pch.h"
#include "Freeplay_Trainer.h"

//Retrieves shot index mapped to current preset and then runs the shot handler on that index
void Freeplay_Trainer::InputHandler(std::vector<std::string> params) {
	if (params.size() != 2 || !gameWrapper->IsInFreeplay()) { return; }

	if (params.at(1) == "left1") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(0));
		cur_shot_index = groupIndices.at(*chosenPres).at(0);

	}else if (params.at(1) == "right2") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(1));
		cur_shot_index = groupIndices.at(*chosenPres).at(1);

	}else if (params.at(1) == "up3") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(2));
		cur_shot_index = groupIndices.at(*chosenPres).at(2);

	}else if (params.at(1) == "down4") {
		hold_ball = false;
		ShotHandler(groupIndices.at(*chosenPres).at(3));
		cur_shot_index = groupIndices.at(*chosenPres).at(3);
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

	if (speed != 0) {
		//Calculates offsets: includes calculating variance
		RelativeOffset *rel = CalculateOffsets(car, shotIndex, false);
		VaryInitialDir(*rel, shotIndex);

		ball.SetLocation(rel->offPos);
		ball.SetVelocity(rel->unitVec * speed);
		ball.SetAngularVelocity({ 0,0,0 }, false);
		
	}
	else {
		//Lock ball in location
		hold_ball = true;
	}

}

// Holds the ball in place when appropriate
void Freeplay_Trainer::CheckBallLock(BallWrapper ball) {
	if (freeze && hold_ball) {
		vector<float> loc = initPosAll.at(cur_shot_index);
		Vector ballLoc = { loc.at(0),loc.at(1),loc.at(2) };
		ball.SetLocation(ballLoc);
		ball.SetVelocity({ 0,0,0 });
		ball.SetAngularVelocity({ 0,0,0 }, false);
	}
}

void Freeplay_Trainer::VaryInitialDir(RelativeOffset& rel, int shotIndex) {
	
	//Defines the circle on the end of the variance cone
	//Picks a random point on that circle and then updates the direction to that.
	float varAngle = variance.at(shotIndex);
	float randAngle = DegToRad(getRandFloat(-varAngle, varAngle));
	float randRad = getRandFloat(-2 * PI, 2 * PI);

	float newRadius = speeds.at(shotIndex) * IND_ARR_RATIO * tan(randAngle);
	LOG("angle: " + to_string(randAngle) + "radian: " + to_string(randRad) +  "radius: " + to_string(newRadius));

	//Define orthogonal plane for variance circle
	Vector unitVec = rel.unitVec.clone();
	Vector u = Vector::cross(unitVec, unitVec + Vector({0,0, 1}));
	Vector v = Vector::cross(unitVec, u.getNormalized());

	//Vector center = rel.offDir;

	rel.offDir += newRadius * (cos(randRad) * u + sin(randRad) * v);
	rel.unitVec = (rel.offDir - rel.offPos).getNormalized();

}

void Freeplay_Trainer::VaryInitialPos(RelativeOffset& rel, int shotIndex) {

	//Randomly offsets the initial shot position
	if (posVarInd == true) {
		if (posVarShape.at(cur_shot) == 0) {
			float x[2] = { -cuboid.at(shotIndex).at(0) / 2,cuboid.at(shotIndex).at(0) / 2 };
			float y[2] = { -cuboid.at(shotIndex).at(1) / 2,cuboid.at(shotIndex).at(1) / 2 };
			float z[2] = { -cuboid.at(shotIndex).at(2) / 2,cuboid.at(shotIndex).at(2) / 2 };

			if (x[0] < -4096 + BALL_RADIUS) {
				x[0] = -4096 + BALL_RADIUS;
			}
			if (x[1] > 4096 - BALL_RADIUS) {
				x[1] = 4096 - BALL_RADIUS;
			}
			if (y[0] < -5120 + BALL_RADIUS) {
				y[0] = -5120 + BALL_RADIUS;
			}
			if (y[1] > 5120 - BALL_RADIUS) {
				y[1] = 5120 - BALL_RADIUS;
			}
			if (z[0] < BALL_RADIUS) {
				z[0] = BALL_RADIUS;
			}
			if (z[1] > 2044 - BALL_RADIUS) {
				z[1] = 2044 - BALL_RADIUS;
			}

			Vector randOff = { getRandFloat(x[0], x[1]), getRandFloat(y[0], y[1]), getRandFloat(z[0], z[1]) };

			rel.offPos += randOff;
			rel.offDir += randOff;

		}
	}
}
