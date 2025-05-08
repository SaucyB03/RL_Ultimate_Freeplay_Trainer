#pragma once
#include "bakkesmod/wrappers/wrapperstructs.h"
#include "../Extra/CanvasExtensions.h"

class CanvasWrapper;

namespace RT
{
	class Frustum;
    class Line;

	class Sphere
	{
	public:
		Vector location;
		Quat orientation;
		float radius;

		// CONSTRUCTORS
		explicit Sphere();
		explicit Sphere(Vector loc, float rad);
		explicit Sphere(Vector loc, Quat rot, float rad);

		// FUNCTIONS
		void Draw(CanvasWrapper canvas, Frustum &frustum, Vector cameraLocation, LinearColor color, int32_t segments) const;

		bool IsOccludingLine(Line &line) const;
	};
}
