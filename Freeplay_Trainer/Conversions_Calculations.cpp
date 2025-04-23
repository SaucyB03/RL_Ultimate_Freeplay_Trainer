#include "pch.h"
#include "Freeplay_Trainer.h"

//Calculations
RelativeOffset* Freeplay_Trainer::CalculateOffsets(CarWrapper car, int shotindex, bool isRender) {
	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();

	//Convert types for calculations
	Vector offsetPos = VecToVector(initPosAll, shotindex);
	Vector2F dirRad = VecToVector2(initDir, shotindex);
	dirRad.X = DegToRad(dirRad.X);
	dirRad.Y = DegToRad(dirRad.Y);
	Vector unitVec = { cos(dirRad.X) * cos(dirRad.Y), sin(dirRad.X) * cos(dirRad.Y), sin(dirRad.Y) };
	Vector offsetDir = (unitVec * speeds.at(shotindex) * IND_ARR_RATIO) + offsetPos;

	Quat rotation(1, 0, 0, 0);
	
	RelativeOffset* rel = new RelativeOffset({ offsetPos, offsetDir, unitVec, rotation });

	if (!isRender && usingPosVar.at(shotindex)) {
		VaryInitialPos(*rel, shotindex);
	}

	//Update the position early, so its defined for shootAt even when relative
	if (rel_to.at(shotindex) == 1) {
		if (carLoc.Y >= 0) {
			rel->offPos.Y += 5150;
			rel->offDir = (unitVec * speeds.at(shotindex) * IND_ARR_RATIO) + offsetPos;
		}
		else {
			rel->offPos.Y -= 5150;
			rel->offDir = (unitVec * speeds.at(shotindex) * -IND_ARR_RATIO) + offsetPos;
		}

	}
	
	if (dirMode.at(shotindex)) {
		CalcShootAt(*rel, car, shotindex);
	}
	if (rel_to.at(cur_shot) == 2) {
		//Updates positions to be a child of the car (locks it to the car)
		if (isRender) {
			rel->offPos = ConvertWorldToLocal(carLoc, carRot, offsetPos, arrowLocked);
			rel->offDir = ConvertWorldToLocal(carLoc, carRot, offsetDir, arrowLocked);
		}
		else {
			rel->offPos = ConvertWorldToLocal(carLoc, carRot, rel->offPos, arrowLocked);
			rel->offDir = ConvertWorldToLocal(carLoc, carRot, rel->offDir, arrowLocked);
		}
		rel->unitVec = ConvertWorldToLocal(carLoc, carRot, unitVec, arrowLocked) - carLoc;
		rel->rotation = RotatorToQuat(carRot);
	}
	rel->offPos.Z += BALL_RADIUS;
	rel->offDir.Z += BALL_RADIUS;
	
	return rel;
}


Vector Freeplay_Trainer::ConvertWorldToLocal(Vector A_pos, Rotator A_rot, Vector B_pos, bool ballLocked)
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
		float yaw = RotYawToRad(A_rot);

		//Calculates vector from parent's position pointing to position of child
		Vector rotOff = { B_pos.X * cos(yaw) - B_pos.Y * sin(yaw), B_pos.X * sin(yaw) + B_pos.Y * cos(yaw) , B_pos.Z };
		vecOff = { rotOff.X, rotOff.Y, rotOff.Z };

		vecOff += A_pos;
	}

	return vecOff;
}

Vector Freeplay_Trainer::CalcKinematics(Vector start, CarWrapper car, int shotIndex) {
	// Based off the cars current velocity, determine where it will be in the given time:
	Vector carVel = car.GetVelocity();
	Vector futureCarLoc = carVel * (timeTo.at(shotIndex)+0.75) + car.GetLocation();

	//Calculate the rest normally:
	return CalcKinematics(start, futureCarLoc, shotIndex);
}

Vector Freeplay_Trainer::CalcKinematics(Vector start, Vector end, int shotIndex) {
	//Calc each component of velocity
	float vx = (end.X - start.X) / timeTo.at(shotIndex);
	float vy = (end.Y - start.Y) / timeTo.at(shotIndex);
	float vz = ((end.Z - start.Z) - 0.5 * GRAVITY * timeTo.at(shotIndex) * timeTo.at(shotIndex)) / timeTo.at(shotIndex);
	
	//Combine into full vector
	return { vx, vy, vz};
}

void Freeplay_Trainer::CalcShootAt(RelativeOffset& rel, CarWrapper car, int shot_index) {
	Vector dir = {};
	//Adjusts for the fact that I changed z == 0 to be z == BALL_RADIUS so ball doesnt clip
	Vector posAdj = rel.offPos + Vector({0, 0, -BALL_RADIUS});
	if (shootAt.at(shot_index) == 0) {
		dir = CalcKinematics(posAdj, car.GetLocation(), shot_index);
	}
	else if(shootAt.at(shot_index) == 1){
		dir = CalcKinematics(posAdj, car, shot_index);
	}
	else if (shootAt.at(shot_index) == 2) {
		if (car.GetLocation().Y >= 0) {
			dir = CalcKinematics(posAdj, {0,5120,642.775/2}, shot_index);
		}
		else {
			dir = CalcKinematics(posAdj, { 0,-5120,642.775 / 2 }, shot_index);
		}
		
	}
	else {
		if (car.GetLocation().Y >= 0) {
			dir = CalcKinematics(posAdj, { 0,5120,2044 / 2 }, shot_index);
		}
		else {
			dir = CalcKinematics(posAdj, { 0,-5120,2044 / 2 }, shot_index);
		}
	}


	rel.unitVec = dir.getNormalized();
	
	speeds.at(shot_index) = dir.magnitude() / KPH_TO_BALL_VEL;
	rel.offDir = (dir / KPH_TO_BALL_VEL * IND_ARR_RATIO) + rel.offPos;
}


//Conversions: 
float Freeplay_Trainer::DegToRad(float degrees) {
	return degrees * 3.1415 / 180;
}
float Freeplay_Trainer::RotYawToRad(Rotator rot) {
	float yaw = rot.Yaw;

	if (yaw < 0) {
		yaw += (2 * 32768);
	}

	yaw = yaw * PI / 32768;

	LOG("yaw: " + to_string(yaw) + " orig: " + to_string(rot.Yaw));

	return yaw;
}

Vector Freeplay_Trainer::VecToVector(vector<float> vect) {
	return { vect.at(0),vect.at(1),vect.at(2) };
}

Vector Freeplay_Trainer::VecToVector(vector<vector<float>> vect, int index) {
	vector<float> atInd = vect.at(index);
	return VecToVector(atInd);
}

Vector2F Freeplay_Trainer::VecToVector2(vector<float> vect) {
	return { vect.at(0),vect.at(1) };
}

Vector2F Freeplay_Trainer::VecToVector2(vector<vector<float>> vect, int index) {
	vector<float> atInd = vect.at(index);
	return VecToVector2(atInd);
}