#include "pch.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "Cone.h"
#include "Matrix3.h"
#include "Frustum.h"
#include "../Extra/RenderingMath.h"
#include "../Extra/WrapperStructsExtensions.h"
#include "../Extra/CanvasExtensions.h"
#include <vector>

RT::Cone::Cone()
	: location(Vector()), direction(Vector(0,0,1)), radius(5.0f), height(20.0f), rollAmount(0.0f), segments(8), thickness(1.0f) {}

RT::Cone::Cone(Vector loc, Vector dir)
	: Cone() {
	location = loc; direction = dir;
}

RT::Cone::Cone(Vector loc, Vector dir, float rad, float length)
	: Cone() {
	location = loc; direction = dir; radius = rad; height = length;
}

void RT::Cone::Draw(CanvasWrapper canvas) const
{
	Vector dir = direction;
	dir.normalize();
	Quat orientation = LookAt(location, location + dir, LookAtAxis::AXIS_UP).ToQuat();

	//Create base circle
	std::vector<Vector> basePoints;
	Vector start = { 1.0f,0.0f,0.0f };

	//Get all the vertices that comprise the circle
	for (int32_t i = 0; i < segments; ++i)
	{
		Vector newPoint = start;
		float angle = ((2.f * CONST_PI_F) / segments * i) + rollAmount;
		Quat rotAmount = AngleAxisRotation(angle, Vector{ 0.0f,0.0f,1.0f });
		newPoint = RotateVectorWithQuat(newPoint, rotAmount);
		basePoints.push_back(newPoint);
	}

	Vector2F tip = canvas.ProjectF(location + (dir * height));

	//Orient circle and project to canvas
	std::vector<Vector2F> canvasPoints;
	for (size_t i = 0; i != basePoints.size(); ++i)
	{
		basePoints[i] = basePoints[i] * radius;
		basePoints[i] = RotateVectorWithQuat(basePoints[i], orientation);
		basePoints[i] = basePoints[i] + location;
		canvasPoints.push_back(canvas.ProjectF(basePoints[i]));

	}

	//Draw lines
	Vector2F startPoint = { 0.0f,0.0f }, endPoint = { 0.0f,0.0f };
	for (size_t i = 0; i != canvasPoints.size(); ++i)
	{
		if (i < canvasPoints.size() - 1)
		{
			startPoint = canvasPoints[i];
			endPoint = canvasPoints[i + 1];
		}
		if (i == canvasPoints.size() - 1)
		{
			startPoint = canvasPoints[i];
			endPoint = canvasPoints[0];
		}

		if (thickness == 1.0f)
		{
			canvas.DrawLine(startPoint, endPoint);
			canvas.DrawLine(startPoint, tip);
		}
		else
		{
			canvas.DrawLine(startPoint, endPoint, thickness);
			canvas.DrawLine(startPoint, tip, thickness);
		}
	}
}

void RT::Cone::Draw(CanvasWrapper canvas, Frustum frustum, LinearColor color) const
{	
	LinearColor inverse({ 255.0f - color.R,255.0f - color.G,255.0f - color.B,255.0f });
	Vector dir = direction;
	dir.normalize();
	Quat orientation = LookAt(location, location + dir, LookAtAxis::AXIS_UP).ToQuat();
	
	//Create base circle
	std::vector<Vector> basePoints;
	Vector start = {1.0f,0.0f,0.0f};
	std::vector<bool> inbounds;

	//Get all the vertices that comprise the circle
	for(int32_t i = 0; i < segments; ++i)
	{
		Vector newPoint = start;
		float angle = ((2.f * CONST_PI_F) / segments * i) + rollAmount;
		Quat rotAmount = AngleAxisRotation(angle, Vector{0.0f,0.0f,1.0f});
		newPoint = RotateVectorWithQuat(newPoint, rotAmount);
		basePoints.push_back(newPoint);
		inbounds.push_back(CheckInBounds(newPoint));
	}

	Vector2F tip = canvas.ProjectF(location + (dir * height));
	
	//Orient circle and project to canvas
	std::vector<Vector2F> canvasPoints;

	for(size_t i = 0; i != basePoints.size(); ++i)
	{
		basePoints[i] = basePoints[i] * radius;
		basePoints[i] = RotateVectorWithQuat(basePoints[i], orientation);
		basePoints[i] = basePoints[i] + location;
		canvasPoints.push_back(canvas.ProjectF(basePoints[i]));

	}

	//Draw lines
	Vector2F startPoint = {0.0f,0.0f}, endPoint = {0.0f,0.0f};
	int startInd = 0;
	int endInd = 0;
	for(size_t i = 0; i != canvasPoints.size(); ++i)
	{
		if(i < canvasPoints.size()-1)
		{
			startPoint = canvasPoints[i];
			endPoint = canvasPoints[i+1];
			startInd = i;
			endInd = i + 1;
		}
		if(i == canvasPoints.size()-1)
		{
			startPoint = canvasPoints[i];
			endPoint = canvasPoints[0];
			startInd = i;
			endInd = 0;
		}
		Vector start = basePoints.at(i);
		Vector end = basePoints.at(i);

		if (!(frustum.IsInFrustum(start) && frustum.IsInFrustum(end))) {
			continue;
		}

		//Changes the color if out of bounds
		if (inbounds.at(startInd) && inbounds.at(endInd)) {
			canvas.SetColor(color);
		}
		else {
			canvas.SetColor(inverse);
		}

		if(thickness == 1.0f)
		{	
			canvas.DrawLine(startPoint, endPoint);
			canvas.DrawLine(startPoint, tip);
		}
		else
		{
			canvas.DrawLine(startPoint, endPoint, thickness);
			canvas.DrawLine(startPoint, tip, thickness);
		}
	}
}
