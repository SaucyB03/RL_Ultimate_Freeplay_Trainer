#include "pch.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "Sphere.h"
#include "Line.h"
#include "Frustum.h"
#include "../Extra/RenderingMath.h"
#include "../Extra/WrapperStructsExtensions.h"
#include <vector>

RT::Sphere::Sphere()
	: location(Vector(0.0f,0.0f,0.0f)), orientation(Quat(1.0f,0.0f,0.0f,0.0f)), radius(100.0f) {}

RT::Sphere::Sphere(Vector loc, float rad)
	: location(loc), orientation(Quat()), radius(rad) {}

RT::Sphere::Sphere(Vector loc, Quat rot, float rad)
	: location(loc), orientation(rot), radius(rad) {}

void RT::Sphere::Draw(CanvasWrapper canvas, Frustum &frustum, Vector cameraLocation, LinearColor color, int32_t segments) const
{
	LinearColor darkerCol;
	int dif = 100;
	darkerCol.R = (color.R > dif) ? color.R - dif : color.R;
	darkerCol.G = (color.G > dif) ? color.G - dif : color.G;
	darkerCol.B = (color.B > dif) ? color.B - dif : color.B;
	darkerCol.A = color.A;

	LinearColor inverse({ 255.0f - color.R, 255.0f - color.G,255.0f - color.B, 255.0f });
	LinearColor darkerInverse;
	darkerInverse.R = (inverse.R > dif) ? color.R - dif : inverse.R;
	darkerInverse.G = (inverse.G > dif) ? color.G - dif : inverse.G;
	darkerInverse.B = (inverse.B > dif) ? inverse.B - dif : inverse.B;
	darkerInverse.A = color.A;

	//Occlude opposite side of sphere using IsOccludingLine on each vertex with a sphere that is slightly smaller than this one
	//Create a vertical (half?) circle and rotate it a few times. Keep track of all points at each level

	//Start with sphere center at 0,0,0
	//radius of radius*.95

	//height segs == segments/2

	if(segments < 4)
	{
		segments = 4;
	}

	int32_t maxVerticalSegs = (segments / 2);

	std::vector<std::vector<Vector>> semicircles;
	Vector start = {0.0f, 0.0f, radius * 0.95f};


	//Fill vertices
	for(int32_t i = 0; i != segments; ++i)
	{
		std::vector<Vector> semicirclePoints;
		for(int32_t j = 0; j != maxVerticalSegs+1; ++j)
		{
			Vector newPoint = start;

			//Define semicircle
			Quat circleShape = AngleAxisRotation(CONST_PI_F * j / maxVerticalSegs, Vector(1.0f,0.0f,0.0f));
			newPoint = RotateVectorWithQuat(newPoint, circleShape);

			//Rotate semicircle
			Quat semicirclePosition = AngleAxisRotation(2.f * CONST_PI_F * i / segments, Vector(0.0f,0.0f,1.0f));
			newPoint = RotateVectorWithQuat(newPoint, semicirclePosition);

			//Rotate to align to orientation
			newPoint = RotateVectorWithQuat(newPoint, orientation);

			//Translate to align to location
			newPoint = newPoint + location;

			semicirclePoints.push_back(newPoint);
		}

		semicircles.push_back(semicirclePoints);
	}

	//Draw sphere
	Sphere testSphere = *this;
	testSphere.radius *= 0.9f;
	for(size_t i = 0; i != semicircles.size(); ++i)
	{
		for(size_t j = 0; j != semicircles[i].size() - 1; ++j)
		{
			//Check if the vertical line points are visible
            Line vertLinePoints(semicircles[i][j], cameraLocation);
			if (!frustum.IsInFrustum(semicircles[i][j]))
			{
				continue;
			}

			//Draw vertical line
            Line vertLineNext(semicircles[i][j + 1], cameraLocation);
			if (frustum.IsInFrustum(semicircles[i][j + 1]))
			{
				if (testSphere.IsOccludingLine(vertLineNext)) {
					DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[i][j+1]), frustum, darkerCol, darkerInverse);

				}
				else {
					DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[i][j+1]), frustum, color, inverse);
				}
			}

			//if()

			//If it's the first line, there are no horizontal lines to draw
			if(j == 0)
			{
				continue;
			}

			//Draw horizontal lines
			if(i != semicircles.size() - 1)
			{
                Line horizontalLine(semicircles[i + 1][j], cameraLocation);
				//if(frustum.IsInFrustum(semicircles[i + 1][j]) && !testSphere.IsOccludingLine(horizontalLine))
				if(frustum.IsInFrustum(semicircles[i + 1][j]))
				{

					if (testSphere.IsOccludingLine(horizontalLine)) {
						DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[i+1][j]), frustum, darkerCol, darkerInverse);

					}
					else {
						DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[i+1][j]), frustum, color, inverse);
					}
				}
			}
			else
			{
				//Connect last semicircle to first semicircle
                Line connectLastToFirstLine(semicircles[0][j], cameraLocation);
				if(frustum.IsInFrustum(semicircles[0][j]))
				{
					DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[0][j]), frustum, darkerCol, darkerInverse);

				}
				else {
					DrawOutOfBounds(canvas, Line(semicircles[i][j], semicircles[0][j]), frustum, color, inverse);

				}
			}
		}
	}
}

bool RT::Sphere::IsOccludingLine(Line &line) const
{
	//Checks if a line drawn from a point to the camera is occluded by the sphere

	//DrawingLocation == line.lineBegin
	//CameraLocation == line.lineEnd

	Vector cameraToSphere = location - line.lineEnd;
	Vector pointToSphere = location - line.lineBegin;
	Vector pointToCamera = line.lineEnd - line.lineBegin;

	//Check if either the drawing or the camera is inside the object
	if(cameraToSphere.magnitude() <= radius || pointToSphere.magnitude() <= radius)
	{
		return true;
	}

	//Check if the distance from sphere center to nearest point on the line is greater than sphere radius
	Vector midProjection = VectorProjection(pointToSphere, pointToCamera) + line.lineBegin;
	if((location - midProjection).magnitude() > radius)
	{
		return false;
	}

	//Check if the projected point is within the drawingToCamera line segment
	Line drawingToCameraSegment(line.lineBegin, line.lineEnd);
	
	//The projected point is on the line
	if(drawingToCameraSegment.IsPointWithinLineSegment(midProjection))
	{
		return true;
	}

	float perc = drawingToCameraSegment.PointPercentageAlongLine(midProjection);
	//The projected point is in front of the line. Check if camera is within sphere.
	if(perc > 1.0f)
	{
		if(cameraToSphere.magnitude() > radius)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	//The projected point is behind the line. Check if drawing is within sphere.
	if(perc < 0.0f)
	{
		if(pointToSphere.magnitude() > radius)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

    return false;
}
