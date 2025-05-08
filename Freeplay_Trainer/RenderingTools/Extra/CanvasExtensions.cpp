#include "pch.h"
#include "CanvasExtensions.h"
#include "../Objects/Matrix3.h"
#include "../Objects/Frustum.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "bakkesmod/wrappers/GameObject/CameraWrapper.h"

LinearColor RT::GetPercentageColor(float percent, float alpha)
{
	LinearColor color = {0.0f, 0.0f, 0.0f, 255.0f * alpha};

	if(percent <= 0.5f)
	{
		color.R = 255;
		color.G = 0.0f + (255.0f * (percent * 2.0f));
	}
	else
	{
		color.R = 255.0f - (255.0f * (percent * 2.0f));
		color.G = 255.0f;
	}
	
	return color;
}

float RT::GetVisualDistance(CanvasWrapper canvas, Frustum frustum, CameraWrapper camera, Vector objectLocation)
{
	//Concept: project a line of set 3D length to the 2D canvas.
	//Calculate the length of that 2D line to get perceived distance instead of linear distance

	Quat camQuat = RotatorToQuat(camera.GetRotation());
	Vector camUp = Matrix3(camQuat).up;

	float testScalePerspectiveLineLength = 100.0f;
	Vector2F perspScaleLineStartProjected = canvas.ProjectF(objectLocation);
	Vector2F perspScaleLineEndProjected;
	Vector testScalePerspective = objectLocation + (camUp * testScalePerspectiveLineLength);

	//check if scale projection is within the top plane of the frustum, if not then invert the scale test line
	Vector planeNormal = {frustum.planes[0].x, frustum.planes[0].y, frustum.planes[0].z}; 
	if(Vector::dot(testScalePerspective, planeNormal) + frustum.planes[0].d + 1.0f > 0)
	{
		perspScaleLineEndProjected = canvas.ProjectF(objectLocation + (camUp * testScalePerspectiveLineLength));
	}
	else
	{
		perspScaleLineEndProjected = canvas.ProjectF(objectLocation - (camUp * testScalePerspectiveLineLength));
	}

	Vector2F perspScaleLine = {perspScaleLineEndProjected.X - perspScaleLineStartProjected.X, perspScaleLineEndProjected.Y - perspScaleLineStartProjected.Y};
	float perspScale = sqrtf(perspScaleLine.X * perspScaleLine.X + perspScaleLine.Y * perspScaleLine.Y);
	//if(perspScale > 100)
	//	perspScale = 100;
	float distancePercentage = perspScale / 100.0f; //1 is close, 0 is infinitely far away
	//if(distancePercentage > 1)
	//	distancePercentage = 1;
	if(distancePercentage < 0.0f)
	{
		distancePercentage = 0.0f;
	}

	return distancePercentage;
}

bool RT::CheckInBounds(bool both, Line line) {
	Vector start(line.lineBegin);
	Vector end(line.lineEnd);

	//Determines if one point is out of bounds is the full line considered out of bounds
	if (both) {
		if (CheckInBounds(start) && CheckInBounds(end)) {
			return true;
		}
		return false;
	}
	
	if (CheckInBounds(start) || CheckInBounds(end)) {
		return true;
	}

	return false;
}

bool RT::CheckInBounds(Vector point) {
	bool inbounds = true;
	if (abs(point.X) > FIELD_WIDTH) {
		inbounds = false;
	}

	if (abs(point.Y) > FIELD_LENGTH) {
		inbounds = false;
	}

	if (point.Z > FIELD_HEIGHT || point.Z < 0.0) {
		inbounds = false;
	}

	return inbounds;
}

void RT::DrawOutOfBounds(CanvasWrapper canvas, Line line, Frustum frustum, LinearColor color, LinearColor inverse) {
	Vector p1 = line.lineBegin;
	Vector p2 = line.lineEnd;
	Vector val({ 0.0,0.0,0.0 });
	Vector off({ 0.0,0.0,0.0 });
	bool bounds[2] = {true, true};


	// Given a line segment, we must:
	//Determine which point(s) is/are out of bounds. 
	//If both are then just change the color
	//If only one is then we must make a line segment from that line to the boundry.
	// - Once we know which is out of bounds we need which direction is it out of bounds

	//Determines if the start point is out of the field
	if (p1.X > FIELD_WIDTH) {
		off.X = 1;
		val.X = FIELD_WIDTH;
		bounds[0] = false;
	}
	else if (p1.X < -FIELD_WIDTH) {
		off.X = 1;
		val.X = -FIELD_WIDTH;
		bounds[0] = false;
	}

	if (p1.Y > FIELD_LENGTH) {
		off.Y = 1;
		val.Y = FIELD_LENGTH;
		bounds[0] = false;
	}else if (p1.Y < -FIELD_LENGTH) {
		off.Y = 1;
		val.Y = -FIELD_LENGTH;
		bounds[0] = false;
	}

	if (p1.Z > FIELD_HEIGHT) {
		off.Z = 1;
		val.Z = FIELD_HEIGHT;
		bounds[0] = false;
	}
	else if(p1.Z < 0.0) {
		off.Z = 1;
		bounds[0] = false;
	}


	//Determines if the end point is out of the field
	if (p2.X > FIELD_WIDTH) {
		off.X = 1;
		val.X = FIELD_WIDTH;
		bounds[1] = false;
	}
	else if (p2.X < -FIELD_WIDTH) {
		off.X = 1;
		val.X = -FIELD_WIDTH;
		bounds[1] = false;
	}

	if (p2.Y > FIELD_LENGTH) {
		off.Y = 1;
		val.Y = FIELD_LENGTH;
		bounds[1] = false;
	}
	else if (p2.Y < -FIELD_LENGTH) {
		off.Y = 1;
		val.Y = -FIELD_LENGTH;
		bounds[1] = false;
	}

	if (p2.Z > FIELD_HEIGHT) {
		off.Z = 1;
		val.Z = FIELD_HEIGHT;
		bounds[1] = false;
	}
	else if (p2.Z < 0.0) {
		off.Z = 1;
		bounds[1] = false;
	}


	// If one of them is out of the field, display segment of line thats out of field in inverse color
	if (bounds[0] != bounds[1]) {
		Vector pointOutbounds;
		Vector delta = p2 - p1;
		Vector pointOnBounds;
		Vector pointInBounds;

		if (!bounds[0]) {
			pointOutbounds = p1;
			pointInBounds = p2;
		}
		else {
			pointOutbounds = p2;
			pointInBounds = p1;
		}

		if (delta.magnitude() == 0) {
			//This should never happen, but this ensures saftey
			return;
		}

		if (off.X != 0.0) {
			float t = (val.X - pointOutbounds.X) / delta.X;
			pointOnBounds = pointOutbounds + t * (delta);
		}
		if (off.Y != 0.0) {
			float t = (val.Y - pointOutbounds.Y) / delta.Y;
			pointOnBounds = pointOutbounds + t * (delta);
		}
		if (off.Z != 0.0) {
			float t = (val.Z - pointOutbounds.Z) / delta.Z;
			pointOnBounds = pointOutbounds + t * (delta);
		}
		//if (frustum.IsInFrustum(pointOutbounds) && frustum.IsInFrustum(pointOnBounds)) {
		canvas.SetColor(inverse);
		canvas.DrawLine(canvas.ProjectF(pointOutbounds), canvas.ProjectF(pointOnBounds));
		//}

		//if (frustum.IsInFrustum(pointInBounds) && frustum.IsInFrustum(pointOnBounds)) {
		canvas.SetColor(color);
		canvas.DrawLine(canvas.ProjectF(pointInBounds), canvas.ProjectF(pointOnBounds));
		//}
	}
	else {
		//If they are either both in the field or both out of the field we only must change the color
		if (bounds[0]) {
			canvas.SetColor(color);
		}
		else {
			canvas.SetColor(inverse);
		}

		canvas.DrawLine(canvas.ProjectF(p1), canvas.ProjectF(p2));
	}
}

void RT::SetColor(CanvasWrapper canvas, std::string colorName, float opacity)
{
	LinearColor color = {0.0f,0.0f,0.0f,opacity};

	     if(colorName == "black")   color = LinearColor{0,0,0,opacity};
	else if(colorName == "white")   color = LinearColor{255,255,255,opacity};
	else if(colorName == "red")     color = LinearColor{255,0,0,opacity};
	else if(colorName == "green")   color = LinearColor{0,255,0,opacity};
	else if(colorName == "blue")    color = LinearColor{0,0,255,opacity};
	else if(colorName == "yellow")  color = LinearColor{255,255,0,opacity};
	else if(colorName == "cyan")    color = LinearColor{0,255,255,opacity};

	canvas.SetColor(color);
}

void RT::DrawDebugStrings(CanvasWrapper canvas, const std::vector<DebugString>& drawStrings, EDebugStringBackground background, int32_t minWidth)
{
	if(drawStrings.empty()) return;

	Vector2 base = {50,50};
	if(background > EDebugStringBackground::BG_None)
	{
        //If background type is static, leave finalWidth as minWidth
		int32_t finalWidth = minWidth;

        //Determine dynamic width of background based on longest string
        if(background > EDebugStringBackground::BG_StaticWidth)
        {
            for(const DebugString& str : drawStrings)
            {
				int32_t strSize = static_cast<int32_t>(canvas.GetStringSize(str.Text).X);
				if (strSize > minWidth)
				{
					finalWidth = strSize;
				}
            }
        }

        //Give a margin at the right of the string to match the left margin
        finalWidth += 20;

        //Draw the background
        canvas.SetColor(LinearColor{0.0f,0.0f,0.0f,150.0f});
		canvas.SetPosition(base - Vector2{10,10});
		canvas.FillBox(Vector2{finalWidth, static_cast<int32_t>(20 * (drawStrings.size() + 1))});
	}

    for(const DebugString& str : drawStrings)
    {
        canvas.SetPosition(base);
        canvas.SetColor(str.Color);
        canvas.DrawString(str.Text);
        base.Y += 20;
    }
}
