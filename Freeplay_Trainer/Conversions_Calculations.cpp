#include "pch.h"
#include "Freeplay_Trainer.h"

//Calculations

// Determines the position of the ball and direction based of selected settings
RelativeOffset* Freeplay_Trainer::CalculateOffsets(CarWrapper car, int shotIndex, bool isRender) {
	cur_speed = speeds.at(shotIndex);

	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();

	//Convert types for calculations
	Vector offsetPos = VecToVector(initPosAll, shotIndex);
	Vector2F dirRad = VecToVector2(initDir, shotIndex);
	dirRad.X = DegToRad(dirRad.X);
	dirRad.Y = DegToRad(dirRad.Y);
	Vector unitVec = { cos(dirRad.X) * cos(dirRad.Y), sin(dirRad.X) * cos(dirRad.Y), sin(dirRad.Y) };
	Vector offsetDir = unitVec;

	Quat rotation(1, 0, 0, 0);
	
	RelativeOffset* rel = new RelativeOffset({ offsetPos, offsetDir, unitVec, rotation });

	if (!isRender && usingPosVar.at(shotIndex)) {
		VaryInitialPos(*rel, shotIndex);
	}

	float* sign = MirrorHandler(*rel, car, shotIndex);
	
	// Calculates trajectory to the selected object
	if (dirMode.at(shotIndex)) {
		CalcShootAt(*rel, car, sign, shotIndex);

	}
	else if (rel_to.at(shotIndex) == 2) {
		//Updates direction to be a child of the car (locks it to the car)
		rel->unitVec = ConvertWorldToLocal(carLoc, carRot, rel->unitVec, arrowLocked) - carLoc;
		rel->rotation = RotatorToQuat(carRot);
	}
	rel->offDir = (rel->unitVec * cur_speed * IND_ARR_RATIO) + rel->offPos;


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

	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();
	// Based off the cars current velocity, determine where it will be in the given time:
	Vector carVel = car.GetVelocity();
	Vector carVelN = carVel.getNormalized();

	//If car isn't moving, we don't want normal vector pointing in a direction
	if (carVel.X < 1) {
		carVelN.X = 0;
	}
	if (carVel.Y < 1) {
		carVelN.Y = 0;
	}
	if (carVel.Z < 1) {
		carVelN.Z = 0;
	}

	Vector carLeadPos = {leadOff.at(shotIndex),0,0 };
	carLeadPos = ConvertWorldToLocal(carLoc, carRot, carLeadPos, false);

	Vector futureCarLoc = carVelN * timeTo.at(shotIndex) + carLeadPos;

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

void Freeplay_Trainer::CalcShootAt(RelativeOffset& rel, CarWrapper car, float* sign, int shotIndex) {
	Vector dir = {};
	//Adjusts for the fact that I changed z == 0 to be z == BALL_RADIUS so ball doesnt clip
	Vector posAdj = rel.offPos + Vector({0, 0, -BALL_RADIUS});

	if (shootAt.at(shotIndex) == 0) {
		//Shoot at center of car
		dir = CalcKinematics(posAdj, car.GetLocation(), shotIndex);
	}
	else if(shootAt.at(shotIndex) == 1){
		//Shoot infront of car
		dir = CalcKinematics(posAdj, car, shotIndex);
	}
	else if (shootAt.at(shotIndex) == 2) {
		//Shoot at goal
		dir = CalcKinematics(posAdj, {0,sign[1]*FIELD_LENGTH,642.775/2}, shotIndex);

		LOG("sign: (" + to_string(sign[0]) + ", " + to_string(sign[1]) + ")" + "| y pos: "+to_string(sign[1] * FIELD_LENGTH));

		
	}
	else {
		//Shoot at Backboard
		dir = CalcKinematics(posAdj, { 0,sign[1]*FIELD_LENGTH,2044 / 2 }, shotIndex);
	}


	rel.unitVec = dir.getNormalized();
	
	cur_speed = dir.magnitude() / KPH_TO_BALL_VEL;
}


//Conversions: 
float Freeplay_Trainer::DegToRad(float degrees) {
	return degrees * 3.1415 / 180;
}
float Freeplay_Trainer::RotYawToRad(Rotator rot) {
	float yaw = rot.Yaw;

	if (yaw < 0) {
		yaw += (2 * PI_ROT);
	}

	yaw = yaw * PI / PI_ROT;

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