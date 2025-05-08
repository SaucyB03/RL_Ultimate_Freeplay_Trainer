#include "pch.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "Cube.h"
#include "Matrix3.h"
#include "Frustum.h"
#include "Line.h"

RT::Cube::Cube()
	: location(Vector()), orientation(Quat()), sideLength(50.0f, 50.0f, 50.0f), lineThickness(1.0f) {}

RT::Cube::Cube(Vector loc)
    : location(loc), orientation(Quat()), sideLength(50.0f, 50.0f, 50.0f), lineThickness(1.0f) {}

RT::Cube::Cube(Vector loc, float sLength)
    : location(loc), orientation(Quat()), sideLength(sLength, sLength, sLength), lineThickness(1.0f) {}

RT::Cube::Cube(Vector loc, Quat rot, float sLength)
	: location(loc), orientation(rot), sideLength(sLength, sLength, sLength), lineThickness(1.0f) {}

RT::Cube::Cube(Vector loc, Quat rot, Vector length)
	: location(loc), orientation(rot), sideLength(length), lineThickness(1.0f) {}

void RT::Cube::Draw(CanvasWrapper canvas) const
{
	Vector halfLength = (sideLength * 0.5f);

	Matrix3 matrix(orientation);
	Vector fwd = matrix.forward * halfLength.X;
	Vector right = matrix.right * halfLength.Y;
	Vector up = matrix.up * halfLength.Z;

	Vector2F points[8];
	points[0] = canvas.ProjectF(location + fwd + right + up); // Front Right Top
	points[1] = canvas.ProjectF(location + fwd + right - up); // Front Right Bottom
	points[2] = canvas.ProjectF(location + fwd - right - up); // Front Left Bottom
	points[3] = canvas.ProjectF(location + fwd - right + up); // Front Left Top
	points[4] = canvas.ProjectF(location - fwd + right + up); // Back Right Top
	points[5] = canvas.ProjectF(location - fwd + right - up); // Back Right Bottom
	points[6] = canvas.ProjectF(location - fwd - right - up); // Back Left Bottom
	points[7] = canvas.ProjectF(location - fwd - right + up); // Back Left Top

	//Rendering works differently when thickness is not 1
	if(lineThickness != 1)
	{
		for(int32_t i = 0; i < 4; ++i)
		{
			if(i == 3)
			{
				canvas.DrawLine(points[i], points[0], lineThickness);
				canvas.DrawLine(points[i + 4], points[4], lineThickness);
				canvas.DrawLine(points[0], points[4], lineThickness);
			}
			else
			{
				canvas.DrawLine(points[i], points[i + 1], lineThickness);
				canvas.DrawLine(points[i + 4], points[i + 5], lineThickness);
				canvas.DrawLine(points[i + 1], points[i + 5], lineThickness);
			}
		}
	}
	else
	{
		for(int32_t i = 0; i < 4; ++i)
		{
			if(i == 3)
			{
				canvas.DrawLine(points[i], points[0]);
				canvas.DrawLine(points[i + 4], points[4]);
				canvas.DrawLine(points[0], points[4]);
			}
			else
			{
				canvas.DrawLine(points[i], points[i + 1]);
				canvas.DrawLine(points[i + 4], points[i + 5]);
				canvas.DrawLine(points[i + 1], points[i + 5]);
			}
		}
	}
}

void RT::Cube::Draw(CanvasWrapper canvas, RT::Frustum frustum, LinearColor color) const
{
	LinearColor inverse({ 255.0f - color.R, 255.0f - color.G,255.0f - color.B, 255.0f });
	Vector halfLength = (sideLength * 0.5f);

	Matrix3 matrix(orientation);
	Vector fwd = matrix.forward * halfLength.X;
	Vector right = matrix.right * halfLength.Y;
	Vector up = matrix.up * halfLength.Z;

	Vector points[8];
	points[0] = location + fwd + right + up; // Front Right Top
	points[1] = location + fwd + right - up; // Front Right Bottom
	points[2] = location + fwd - right - up; // Front Left Bottom
	points[3] = location + fwd - right + up; // Front Left Top
	points[4] = location - fwd + right + up; // Back Right Top
	points[5] = location - fwd + right - up; // Back Right Bottom
	points[6] = location - fwd - right - up; // Back Left Bottom
	points[7] = location - fwd - right + up; // Back Left Top

	//Rendering works differently when thickness is not 1
	if (lineThickness != 1)
	{
		for (int32_t i = 0; i < 4; ++i)
		{
			if (i == 3)
			{	
				if (frustum.IsInFrustum(points[i]) && frustum.IsInFrustum(points[0])) {

					DrawOutOfBounds(canvas, Line(points[i], points[0]), frustum, color, inverse);

				}
				if (frustum.IsInFrustum(points[i+4]) && frustum.IsInFrustum(points[4])) {

					DrawOutOfBounds(canvas, Line(points[i+1], points[4]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[0]) && frustum.IsInFrustum(points[4])) {

					DrawOutOfBounds(canvas, Line(points[0], points[4]), frustum, color, inverse);
				}
			}
			else
			{
				if (frustum.IsInFrustum(points[i]) && frustum.IsInFrustum(points[i+1])) {

					DrawOutOfBounds(canvas, Line(points[i], points[i+1]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[i+4]) && frustum.IsInFrustum(points[i+5])) {

					DrawOutOfBounds(canvas, Line(points[i+4], points[5]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[i+1]) && frustum.IsInFrustum(points[i+5])) {

					DrawOutOfBounds(canvas, Line(points[i+1], points[i+5]), frustum, color, inverse);
				}
			}
		}
	}
	else
	{
		for (int32_t i = 0; i < 4; ++i)
		{
			if (i == 3)
			{
				if (frustum.IsInFrustum(points[i]) && frustum.IsInFrustum(points[0])) {

					DrawOutOfBounds(canvas, Line(points[i], points[0]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[i + 4]) && frustum.IsInFrustum(points[4])) {

					DrawOutOfBounds(canvas, Line(points[i+4], points[4]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[0]) && frustum.IsInFrustum(points[4])) {

					DrawOutOfBounds(canvas, Line(points[0], points[4]), frustum, color, inverse);
				}
			}
			else
			{
				if (frustum.IsInFrustum(points[i]) && frustum.IsInFrustum(points[i + 1])) {

					DrawOutOfBounds(canvas, Line(points[i], points[i+1]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[i + 4]) && frustum.IsInFrustum(points[i + 5])) {

					canvas.DrawLine(canvas.ProjectF(points[i + 4]), canvas.ProjectF(points[i + 5]));
					DrawOutOfBounds(canvas, Line(points[i+4], points[i+5]), frustum, color, inverse);
				}
				if (frustum.IsInFrustum(points[i + 1]) && frustum.IsInFrustum(points[i + 5])) {

					DrawOutOfBounds(canvas, Line(points[i+1], points[i+5]), frustum, color, inverse);
				}
			}
		}
	}
}

