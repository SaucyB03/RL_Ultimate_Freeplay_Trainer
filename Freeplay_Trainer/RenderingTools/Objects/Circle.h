#pragma once
#include "bakkesmod/wrappers/wrapperstructs.h"

class CanvasWrapper;

namespace RT
{
	class Frustum;

	class Circle
	{
	public:
		Vector location;
		Quat orientation;
		float radius;
		float lineThickness;
		float piePercentage;
		int32_t steps;

		// CONSTRUCTORS
		explicit Circle();
		explicit Circle(Vector loc, Quat rot, float rad, float percent);

		// FUNCTIONS
		void Draw(CanvasWrapper canvas, Vector axis, Frustum &frustum, LinearColor color) const;
		void DrawSegmented(CanvasWrapper canvas, Frustum &frustum, int segments, float percentPerSeg=0.5f) const;
	};
}
