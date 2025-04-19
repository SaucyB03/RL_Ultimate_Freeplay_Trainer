#include "pch.h"
#include "Freeplay_Trainer.h"

//Calculations
RelativeOffset Freeplay_Trainer::CalculateOffsets(CarWrapper car, int shotindex) {
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
	if (rel_to.at(shotindex) == 1) {
		if (carLoc.Y >= 0) {
			offsetPos.Y += 5150;
		}
		else {
			offsetPos.Y -= 5150;
		}
		offsetDir = (unitVec * speeds.at(shotindex) * IND_ARR_RATIO) + offsetPos;
	}
	else if (rel_to.at(cur_shot) == 2) {
		//Updates positions to be a child of the car (locks it to the car)
		offsetPos = ConvertWorldToLocal(carLoc, carRot, VecToVector(initPosAll, shotindex), ballLocked);
		offsetDir = ConvertWorldToLocal(carLoc, carRot, offsetDir, arrowLocked);
		unitVec = ConvertWorldToLocal(carLoc, carRot, unitVec, arrowLocked) - carLoc;
		rotation = RotatorToQuat(carRot);
	}

	RelativeOffset rel = { offsetPos, offsetDir, unitVec, rotation };
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