#include "pch.h"
#include "Freeplay_Trainer.h"

void Freeplay_Trainer::Render(CanvasWrapper canvas) {
	if (!gameWrapper->IsInFreeplay()) { return; }
	if (!*enable) { return; }

	CameraWrapper camera = gameWrapper->GetCamera();
	CarWrapper car = gameWrapper->GetLocalCar();
	RT::Frustum frustum(canvas, RotatorToQuat(camera.GetRotation()), camera.GetLocation());

	Vector carLoc = car.GetLocation();
	Rotator carRot = car.GetRotation();
	


	canvas.SetColor(LinearColor(255.0, 255.0, 0.0, 255.0));

	Vector offsetPos = pos;
	Vector offsetDir = dir;
	Quat rotation(1, 0, 0, 0);
	if (rel == 1) {
		if (carLoc.Y >= 0) {
			offsetPos.Y += 5150;
		}
		else {
			offsetPos.Y -= 5150;
		}
	}
	else if (rel == 2) {
		offsetPos = ConvertWorldToLocal(carLoc, carRot, pos, ballLocked);
		offsetDir = ConvertWorldToLocal(offsetPos, carRot, dir, arrowLocked);
		rotation = RotatorToQuat(carRot);
	}

	RenderShotIndicators(canvas, camera, frustum, offsetPos, offsetDir, rotation);

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
		Vector endPos = (unitVec * init_speed * IND_ARR_RATIO) + offsetPos;

		//Display GUI
		RT::Line line(offsetPos, endPos);
		line.DrawWithinFrustum(canvas, frustum);
		RT::Cone point(endPos, unitVec, 30, 50);
		point.Draw(canvas, frustum);
	}
}

void Freeplay_Trainer::RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, Vector offsetPos, Vector offsetDir) {
	if (dirV) {
		Vector dirRad = { offsetDir.X, offsetDir.Y, offsetDir.Z };
		Vector unitVec = { sin(dirRad.X), cos(dirRad.Y), sin(dirRad.Z) };
		Vector startPos = (unitVec * init_speed * IND_ARR_RATIO/2) + offsetPos;

		float vRad = DegToRad(curVar);
		float l = init_speed * IND_ARR_RATIO / 2;
		float h = l*tan(vRad);

		RT::Cone varCone(startPos, -1*unitVec, h, l);
		varCone.Draw(canvas, frustum);
	}
}

