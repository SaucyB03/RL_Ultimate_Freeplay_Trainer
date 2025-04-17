#include "pch.h"
#include "Freeplay_Trainer.h"

void Freeplay_Trainer::Render(CanvasWrapper canvas) {
	if (!gameWrapper->IsInFreeplay()) { return; }
	if (!*enable) { return; }

	CameraWrapper camera = gameWrapper->GetCamera();
	if (!camera) { return; }
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) { return; }
	RT::Frustum frustum(canvas, RotatorToQuat(camera.GetRotation()), camera.GetLocation());

	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();


	canvas.SetColor(LinearColor(255.0, 255.0, 0.0, 255.0));

	Vector offsetPos = VecToVector(initPosAll,cur_shot);
	Vector offsetDir = VecToVector(initDir, cur_shot);
	Quat rotation(1, 0, 0, 0);
	if (rel_to.at(cur_shot) == 1) {
		if (carLoc.Y >= 0) {
			offsetPos.Y += 5150;
		}
		else {
			offsetPos.Y -= 5150;
		}
	}
	else if (rel_to.at(cur_shot) == 2) {
		offsetPos = ConvertWorldToLocal(carLoc, carRot, VecToVector(initPosAll, cur_shot), ballLocked);
		offsetDir = ConvertWorldToLocal(offsetPos, carRot, VecToVector(initDir, cur_shot), arrowLocked);
		rotation = RotatorToQuat(carRot);
	}

	//Render applicable indicators:
	RenderShotIndicators(canvas, camera, frustum, offsetPos, offsetDir, rotation);
	
	//Changing color to distinguish indicators better
	canvas.SetColor(LinearColor(255.0, 0, 0.0, 255.0));

	RenderVarianceIndicators(canvas, camera, frustum, offsetPos, offsetDir);
}

void Freeplay_Trainer::RenderShotIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir, Quat rotation) {

	if (ball_indicator) {
		RT::Sphere b(offsetPos, 15);
		RT::Sphere ball(offsetPos, rotation, BALL_RADIUS);
		ball.Draw(canvas, frustum, camera.GetLocation(), 16);
	}

	if (line_indicator) {
		// Calc Arrow
		//Vector offsetDir = dir + offsetPos;
		Vector dirRad = { offsetDir.X, offsetDir.Y, offsetDir.Z };
		Vector unitVec = { sin(dirRad.X), cos(dirRad.Y), sin(dirRad.Z) };
		Vector endPos = (unitVec * speeds.at(cur_shot) * IND_ARR_RATIO) + offsetPos;

		//Display GUI
		RT::Line line(offsetPos, endPos);
		line.DrawWithinFrustum(canvas, frustum);
		RT::Cone point(endPos, unitVec, 30, 50);
		point.Draw(canvas, frustum);
	}
}

void Freeplay_Trainer::RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir) {
	//Displays the directional variance
	if (usingDirVar.at(cur_shot)) {
		//calc unit vector and retrieve start position for cone
		Vector dirRad = { offsetDir.X, offsetDir.Y, offsetDir.Z };
		Vector unitVec = { sin(dirRad.X), cos(dirRad.Y), sin(dirRad.Z) };
		Vector startPos = (unitVec * speeds.at(cur_shot) * IND_ARR_RATIO/2) + offsetPos;

		//Convert angle to cone height
		float vRad = DegToRad(variance.at(cur_shot));
		float l = speeds.at(cur_shot) * IND_ARR_RATIO / 2;
		float h = l*tan(vRad);

		//Create and Draw Cone
		RT::Cone varCone(startPos, -1*unitVec, h, l);
		varCone.Draw(canvas, frustum);
	}

	//Displays the positional variance
	if (usingPosVar.at(cur_shot)) {
		if (posVarShape.at(cur_shot) == 0) {
			//Create and draw the cuboid
			RT::Cube varCube(offsetPos, Quat(1,1,0,0), VecToVector(cuboid,cur_shot));
			varCube.Draw(canvas, frustum);
		}
		else {
			//Create and draw the sphere
			RT::Sphere varSphere(offsetPos, sphere.at(cur_shot));
			varSphere.Draw(canvas, frustum, camera.GetLocation(), 16);
		}
	}
}

