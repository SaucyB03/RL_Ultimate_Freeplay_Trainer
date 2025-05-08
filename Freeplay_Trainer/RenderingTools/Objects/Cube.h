#pragma once
#include "bakkesmod/wrappers/wrapperstructs.h"
#include "Extra/CanvasExtensions.h"

class CanvasWrapper;

namespace RT
{
	class Cube
	{
	public:
		Vector location;
		Quat orientation;
		Vector sideLength;
		float lineThickness;

		// CONSTRUCTORS
		explicit Cube();
        explicit Cube(Vector loc);
        explicit Cube(Vector loc, float sLength);
		explicit Cube(Vector loc, Quat rot, float sLength);
		explicit Cube(Vector loc, Quat rot, Vector length);

		// FUNCTIONS
		void Draw(CanvasWrapper canvas) const;
		void Draw(CanvasWrapper canvas, RT::Frustum frustum, LinearColor color) const;
	};
}
