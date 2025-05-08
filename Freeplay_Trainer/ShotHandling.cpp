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


	if (dirMode.at(shotIndex) || (speeds.at(shotIndex) != 0 && !willFreeze.at(shotIndex))) {
		//Calculates offsets: includes calculating variance
		RelativeOffset *rel = CalculateOffsets(car, ball, shotIndex, false);
		VaryInitialDir(*rel, shotIndex);

		ball.SetLocation(rel->offPos);
		Vector ballVel = rel->unitVec * cur_speed * KPH_TO_BALL_VEL;
		if (addVel.at(shotIndex) && !dirMode.at(shotIndex)) {
			ballVel += car.GetVelocity();
		}
		ball.SetVelocity(ballVel);
		ball.SetAngularVelocity({ 0,0,0 }, false);
		
	}
	else {
		RelativeOffset* rel = CalculateOffsets(car, ball, shotIndex, false);
		ball.SetLocation(rel->offPos);
		curPos = rel->offPos;
		ball.SetVelocity({ 0,0,0 });
		ball.SetAngularVelocity({ 0,0,0 }, false);
		//Lock ball in location
		hold_ball = true;
		cur_speed = 0.0;
	}

}

// Holds the ball in place when appropriate
void Freeplay_Trainer::CheckBallLock(BallWrapper ball, CarWrapper car) {
	if (willFreeze.at(cur_shot_index) && hold_ball) {
		ball.SetLocation(curPos);
		ball.SetVelocity({ 0,0,0 });
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

	float newRadius = cur_speed * IND_ARR_RATIO * tan(randAngle);

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
			//Cuboid position
			float x[2] = { -cuboid.at(shotIndex).at(0) / 2,cuboid.at(shotIndex).at(0) / 2 };
			float y[2] = { -cuboid.at(shotIndex).at(1) / 2,cuboid.at(shotIndex).at(1) / 2 };
			float z[2] = { -cuboid.at(shotIndex).at(2) / 2,cuboid.at(shotIndex).at(2) / 2 };

		/*	if (rel_to.at(shotIndex) != 2) {
				float z[2] = { -cuboid.at(shotIndex).at(1) / 2,cuboid.at(shotIndex).at(1) / 2 };
				float y[2] = { -cuboid.at(shotIndex).at(2) / 2,cuboid.at(shotIndex).at(2) / 2 };
			}*/
			


			Vector randOff = { getRandFloat(x[0], x[1]), getRandFloat(y[0], y[1]), getRandFloat(z[0], z[1]) };

			rel.offPos += randOff;
		}
		else {
			//Sphere position
			float x[2] = { -1.0,1.0};
			float y[2] = { -1.0,1.0 };
			float z[2] = { -1.0,1.0 };

			//Get random unit vector from center and a random scale
			Vector randOff = Vector({ getRandFloat(x[0], x[1]), getRandFloat(y[0], y[1]), getRandFloat(z[0], z[1]) }).getNormalized();
			float randomScale = getRandFloat(0.0, sphere.at(shotIndex));

			//Multiply random unit vector by random scale to get position from center, then add to original pos
			rel.offPos += randOff * randomScale;

		}

	}
}

vector<float> Freeplay_Trainer::MirrorHandler(RelativeOffset& rel, CarWrapper car, bool isRender, int shotIndex) {

	vector<float> sign = { signs->at(0), signs->at(1) };

	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();

	if (mirror.at(shotIndex).at(0) == 0 && mirror.at(shotIndex).at(1) == 0) {
		if (rel_to.at(shotIndex) == 2) {
			rel.offPos = ConvertWorldToLocal(carLoc, carRot, rel.offPos, false);
		}
		else if (rel_to.at(shotIndex) == 1) {
			rel.offPos.Y += FIELD_LENGTH;
		}
		
		return sign;
	}



	if (mirror.at(shotIndex).at(1) == 1) {
		if (carLoc.Y != 0) {
			sign.at(1) = carLoc.Y / abs(carLoc.Y);
		}

		if (rel_to.at(shotIndex) == 1) {
			rel.offPos.Y *= -sign.at(1);
			rel.offPos.Y += sign.at(1) * FIELD_LENGTH;
		}
		else if(rel_to.at(shotIndex) != 3){
			rel.offPos.Y *= sign.at(1);
		}
		rel.unitVec.Y *= sign.at(1);
	}
	else if (mirror.at(shotIndex).at(1) == 2) {
		if (carRot.Yaw != 0) {
			sign.at(1) = carRot.Yaw / abs(carRot.Yaw);
		}

		if (rel_to.at(shotIndex) == 1) {
			rel.offPos.Y *= -sign.at(1);
			rel.offPos.Y += sign.at(1) * FIELD_LENGTH;
		}
		else if (rel_to.at(shotIndex) != 3) {
			rel.offPos.Y *= sign.at(1);
		}
		rel.unitVec.Y *= sign.at(1);


	}
	else if (mirror.at(shotIndex).at(1) == 3 && !isRender) {
		float rand = getRandFloat(-50, 49);
		sign.at(1) = (rand < 0) ? -1 : 1;

		if (rel_to.at(shotIndex) == 1) {
			rel.offPos.Y *= -sign.at(1);
			rel.offPos.Y += sign.at(1) * FIELD_LENGTH;
		}
		else {
			rel.offPos.Y *= sign.at(1);
		}
		rel.unitVec.Y *= sign.at(1);
	}
	else if (mirror.at(shotIndex).at(1) == 3 && isRender && shotIndex == cur_shot) {
		if (rel_to.at(shotIndex) == 1) {
			rel.offPos.Y *= -signs->at(1);
			rel.offPos.Y += signs->at(1)*FIELD_LENGTH;
		}
		else {
			rel.offPos.Y *= signs->at(1);
		}
		rel.unitVec.Y *= signs->at(1);
	}


	// X mirroring:
	if (mirror.at(shotIndex).at(0) == 1) {
		if (carLoc.X != 0) {
			sign.at(0) = carLoc.X / abs(carLoc.X);
		}


		if (rel_to.at(shotIndex) == 2) {
			rel.offPos.Y *= sign.at(0);
			rel.unitVec.Y *= sign.at(0);
		}else if (rel_to.at(shotIndex) == 3) {
			rel.unitVec.X *= sign.at(0);

		}else{
			rel.offPos.X *= sign.at(0);
			rel.unitVec.X *= sign.at(0);
		}
	}
	else if (mirror.at(shotIndex).at(0) == 2) {
		float yaw = PI_ROT / 2 - abs(carRot.Yaw);
		if (yaw != 0) {
			sign.at(0) = yaw / abs(yaw);
		}

		if (rel_to.at(shotIndex) == 2) {
			rel.offPos.Y *= sign.at(0);
			rel.unitVec.Y *= sign.at(0);

		}else if (rel_to.at(shotIndex) == 3) {
			rel.unitVec.X *= sign.at(0);
		}
		else {
			rel.offPos.X *= sign.at(0);
			rel.unitVec.X *= sign.at(0);
		}
	}
	else if (mirror.at(shotIndex).at(0) == 3 && !isRender) {
		float rand = getRandFloat(-50, 49);
		sign.at(0) = (rand < 0) ? -1 : 1;

		//assign random side

		if (rel_to.at(shotIndex) == 2) {
			rel.offPos.Y *= sign.at(0);
			rel.unitVec.Y *= sign.at(0);
		}
		else {
			rel.offPos.X *= sign.at(0);
			rel.unitVec.X *= sign.at(0);
		}
	}
	else if (mirror.at(shotIndex).at(0) == 3 && isRender && shotIndex == cur_shot) {

		if (rel_to.at(shotIndex) == 2) {
			rel.offPos.Y *= signs->at(0);
			rel.unitVec.Y *= signs->at(0);
		}
		else {
			rel.offPos.X *= signs->at(0);
			rel.unitVec.X *= signs->at(0);
		}
	}


	if (rel_to.at(shotIndex) == 2) {
		rel.offPos = ConvertWorldToLocal(carLoc, carRot, rel.offPos, false);
	}

	signs->at(0) = sign.at(0);
	signs->at(1) = sign.at(1);

	return sign;
}
