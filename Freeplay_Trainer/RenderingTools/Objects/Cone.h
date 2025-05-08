#pragma once
#include "bakkesmod/wrappers/wrapperstructs.h"

class CanvasWrapper;

namespace RT
{
	class Cone
	{
	public:
		Vector location;
		Vector direction;
		float radius;
		float height;
		float rollAmount;
		float thickness;
		int32_t segments;

		// CONSTRUCTORS
		explicit Cone();
		explicit Cone(Vector loc, Vector dir);
		explicit Cone(Vector loc, Vector dir, float rad, float length);

		// FUNCTIONS
		void Draw(CanvasWrapper canvas) const;
		void Draw(CanvasWrapper canvas, Frustum frustum, LinearColor color) const;
	};
}
