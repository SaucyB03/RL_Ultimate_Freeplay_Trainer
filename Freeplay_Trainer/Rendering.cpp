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
	RelativeOffset* rel = CalculateOffsets(car, cur_shot, true);

	//Render applicable indicators:
	RenderShotIndicators(canvas, camera, frustum, *rel);
	RenderVarianceIndicators(canvas, camera, frustum, *rel);
}

void Freeplay_Trainer::RenderShotIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, RelativeOffset rel) {
	if (ball_indicator) {
		//Display Ball
		RT::Sphere b(rel.offPos, 15);
		RT::Sphere ball(rel.offPos, rel.rotation, BALL_RADIUS);
		ball.Draw(canvas, frustum, camera.GetLocation(), LinearColor(255.0, 255.0, 0.0, 255.0), 16);

		if (line_indicator) {
			//Display Arrow
			RT::Line line(rel.offPos, rel.offDir);
			line.DrawWithinFrustum(canvas, frustum, LinearColor(255.0, 255.0, 0.0, 255.0));
			RT::Cone point(rel.offDir, rel.unitVec, 30, 50);
			point.Draw(canvas, frustum, LinearColor(255.0, 255.0, 0.0, 255.0));
		}
	}
}

void Freeplay_Trainer::RenderVarianceIndicators(CanvasWrapper canvas, CameraWrapper camera, RT::Frustum frustum, RelativeOffset rel) {
	//Displays the directional variance
	if (usingDirVar.at(cur_shot) && dirVarInd) {
		//calc start position for cone

		Vector startPos = (rel.unitVec * speeds.at(cur_shot) * IND_ARR_RATIO/2) + rel.offPos;

		//Convert angle to cone height
		float vRad = DegToRad(variance.at(cur_shot));
		float l = speeds.at(cur_shot) * IND_ARR_RATIO / 2;
		float h = l*tan(vRad);

		//Create and Draw Cone
		RT::Cone varCone(startPos, -1*rel.unitVec, h, l);
		varCone.Draw(canvas, frustum, LinearColor(255.0, 0.0, 0.0, 255.0));
	}

	//Displays the positional variance
	if (usingPosVar.at(cur_shot) && posVarInd) {
		if (posVarShape.at(cur_shot) == 0) {
			//Create and draw the cuboid
			if (rel_to.at(cur_shot) == 2) {
				RT::Cube varCube(rel.offPos, rel.rotation, VecToVector(cuboid, cur_shot));
				varCube.Draw(canvas, frustum, LinearColor(255.0, 0.0, 0.0, 255.0));
			}else{
				RT::Cube varCube(rel.offPos, Quat(1, 1, 0, 0), VecToVector(cuboid, cur_shot));
				varCube.Draw(canvas, frustum, LinearColor(255.0, 0.0, 0.0, 255.0));
			}

		}
		else {
			//Create and draw the sphere
			RT::Sphere varSphere(rel.offPos, sphere.at(cur_shot));
			varSphere.Draw(canvas, frustum, camera.GetLocation(), LinearColor(255.0, 0.0, 0.0, 255.0), 16);
		}
	}
}

