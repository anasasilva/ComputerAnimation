#include "splines.h"
#include "cinder/PolyLine.h"


std::vector<vec3> PointInterp::GetInterpolatedLine(float interval)
{
	// This function receives an interval at which the function should be sampled.
	std::vector<vec3> interpList; //The vector with the interpolated positions
    
    for (int i = 0; i < points.size() - 1; i++){
        RedefinedPoint P0 = points.at(i);
        RedefinedPoint P1 = points.at(i + 1);
                
        for (float u = 0; u < 1; u = u + interval) {
            vec3 interpolatedPos = (1 - u) * P0.pos + u * P1.pos;
            interpList.push_back(interpolatedPos);
        }
        
        interpList.push_back(P1.pos);
        
    }

    
	return interpList; //A copy of the vector is returned
}

std::vector<vec3> PointInterp::GetHermiteSpline(float interval)
{
	//todo TASK2 - begin
	/* Same as in TASK1 except this time the interpolation should be based on a cubic hermite spline
	See the lecture book on how to calculate the spline (or use whatever ressources you prefer).
	The hermite spline needs a tangent which is already part of the "Point" class (see the Point class in splines.h) as the member variable hermiteTangent.
	This problem can be solved with around 15 lines of (compact) code.
	*/
	std::vector<vec3> interpList;  //The vector you should fill with the interpolated positions
								   // Your code
    
    for (int i = 0; i < points.size() - 1; i++){
        RedefinedPoint P0 = points.at(i);
        RedefinedPoint P1 = points.at(i + 1);
    
        for (float u = 0; u <= 1; u = u + interval) {
            float u3 = u * u * u;
            float u2 = u * u;
            
            float eq1 = 2 * u3 - 3 * u2 + 1;
            float eq2 = -2 * u3 + 3 * u2;
            float eq3 = u3 - 2 * u2 + u;
            float eq4 = u3 - u2;
            
            float x = eq1 * P0.pos.x + eq2 * P1.pos.x + eq3 * P0.hermiteTangent.x + eq4 * P1.hermiteTangent.x ;
            float y = eq1 * P0.pos.y + eq2 * P1.pos.y + eq3 * P0.hermiteTangent.y + eq4 * P1.hermiteTangent.y ;
            float z = eq1 * P0.pos.z + eq2 * P1.pos.z + eq3 * P0.hermiteTangent.z + eq4 * P1.hermiteTangent.z ;
            
            interpList.push_back(vec3(x, y, z));
        }
        
        interpList.push_back(P1.pos);
    }
    
    
	return interpList; //Returns a copy of the vector
}

std::vector<vec3> PointInterp::GetParabolaInterpSpline(float interval)
{

	//todo TASK3 - begin
	/* As in the previous Task, except you should implement Parabola interpolation. See the Book on how to implement this.
	*/
	std::vector<vec3> list;
    
    for (int i = 0; i < points.size() - 1; i++){
        RedefinedPoint P0;
        RedefinedPoint P1 = points.at(i);
        RedefinedPoint P2 = points.at(i + 1);
        RedefinedPoint P3;
        
        if (i == 0)
            P0 = points.at(i);
        else
            P0 = points.at(i - 1);
        
        if (i == points.size() - 2)
            P3 = points.at(i + 1);
        else
            P3 = points.at(i + 2);
        
    
       
        for (float u = 0; u <= 1; u = u + interval) {
            float u3 = u * u * u;
            float u2 = u * u;

            float eq1 = 0.5 * (-1 * u3 + 2 * u2 - u);
            float eq2 = 0.5 * (3 * u3 - 5 * u2 + 2);
            float eq3 = 0.5 * (-3 * u3 + 4 * u2 + u);
            float eq4 = 0.5 * (u3 - u2);

            float x = eq1 * P0.pos.x + eq2 * P1.pos.x + eq3 * P2.pos.x + eq4 * P3.pos.x ;
            float y = eq1 * P0.pos.y + eq2 * P1.pos.y + eq3 * P2.pos.y + eq4 * P3.pos.y ;
            float z = eq1 * P0.pos.z + eq2 * P1.pos.z + eq3 * P2.pos.z + eq4 * P3.pos.z ;

            list.push_back(vec3(x, y, z));
            
        }
           
        list.push_back(P2.pos);
            
    }
        
	// Your code
	return list; //Returns a copy of the vector
				 //todo TASK3 - end
}

std::vector<vec3> PointInterp::GetBezierInterpSpline(float interval)
{
	/*
	TASK4 - begin
	Finally implement Bezier interpolation. The "Point" class has member variables bezierTangentF and bezierTangentB
	*/
	std::vector<vec3> interpList; //The std::vector holding the interpolated positions
								  // Your code
    
    for (int i = 0; i < points.size() - 1; i++){
        vec3 P0 = points.at(i).pos;
        vec3 P1 = points.at(i).pos + points.at(i).bezierTangentF;
        vec3 P2 = points.at(i + 1).pos + points.at(i + 1).bezierTangentB;
        vec3 P3 = points.at(i + 1).pos;
       
        for (float u = 0; u <= 1; u = u + interval) {
            float u3 = u * u * u;
            float u2 = u * u;

            float eq1 = -u3 + 3 * u2 - 3 * u + 1;
            float eq2 = 3 * u3 - 6 * u2 + 3 * u;
            float eq3 = -3 * u3 + 3 * u2;
            float eq4 = u3;

            float x = eq1 * P0.x + eq2 * P1.x + eq3 * P2.x + eq4 * P3.x ;
            float y = eq1 * P0.y + eq2 * P1.y + eq3 * P2.y + eq4 * P3.y ;
            float z = eq1 * P0.z + eq2 * P1.z + eq3 * P2.z + eq4 * P3.z ;

            interpList.push_back(vec3(x, y, z));
            
        }
           
        interpList.push_back(P3);
            
    }
    
	return interpList;
}

glm::mat4 PointInterp::ConstructHermiteB(Point p1, Point p2)
{
	//todo TASK2 Optional, you may or may not create and use this function
	return glm::mat4();

}

glm::mat4 PointInterp::ConstructParabolaB(Point p1, Point p2, Point p3, Point p4)
{
	//todo TASK3 Optional, you may or may not create and use this function
	return glm::mat4();
}

glm::mat4 PointInterp::ConstructBezierB(Point p1, Point p2)
{
	//todo TASK4 Optional, you may or may not create and use this function
	return glm::mat4();
}



void PointInterp::draw()
{
	if (points.size() <= 0) return;

	gl::pushModelMatrix();

	auto deltaPositions = getDeltaPositions();

	for (size_t i = 0; i < points.size(); ++i) {
		gl::translate(deltaPositions[i]);
		if (i == activePoint)
			DrawHandles();
		else
			gl::color(Color(1, 0, 0));
		points[i].batchRef->draw();
	}
	gl::popModelMatrix();
	gl::color(Color(1, 1, 1));
	std::vector<vec3> interpList = GetActiveSpline(0.1f);
	gl::draw(interpList);
	
}

void PointInterp::DrawHandles()
{
	gl::color(Color(1, 0, 0));
	gl::drawVector(vec3(0, 0, 0), vec3(1, 0, 0));
	gl::color(Color(0, 1, 0));
	gl::drawVector(vec3(0, 0, 0), vec3(0, 1, 0));
	gl::color(Color(0, 0, 1));
	gl::drawVector(vec3(0, 0, 0), vec3(0, 0, 1));
	//Draw the Handles for certain Modes
	if (currentInterpMode == hermite)
	{
		gl::drawLine(vec3(0,0,0),points[activePoint].hermiteTangent);
		gl::pushModelMatrix();
		gl::translate(points[activePoint].hermiteTangent);
		gl::drawStrokedCube(ci::AxisAlignedBox(vec3(-0.1, -0.1, -0.1), vec3(0.1, 0.1, 0.1)));
		gl::popModelMatrix();
	}
	else if (currentInterpMode == bezier)
	{
		if (activePoint != points.size() - 1) {
			gl::color(Color(1, 0, 0));
			gl::drawLine(vec3(0, 0, 0), points[activePoint].bezierTangentF);
			gl::pushModelMatrix();
			gl::translate(points[activePoint].bezierTangentF);
			gl::drawStrokedCube(ci::AxisAlignedBox(vec3(-0.1, -0.1, -0.1), vec3(0.1, 0.1, 0.1)));
			gl::popModelMatrix();
		}
		if (activePoint != 0) {
			gl::color(Color(0, 1, 0));
			gl::drawLine(vec3(0, 0, 0), points[activePoint].bezierTangentB);
			gl::pushModelMatrix();
			gl::translate(points[activePoint].bezierTangentB);
			gl::drawStrokedCube(ci::AxisAlignedBox(vec3(-0.1, -0.1, -0.1), vec3(0.1, 0.1, 0.1)));
			gl::popModelMatrix();
		}
	}
}

void PointInterp::MouseDown(MouseEvent event, CameraPersp cam)
{
	vec2 mousePos = event.getPos();
	float u = mousePos.x / (float)getWindowWidth();
	float v = mousePos.y / (float)getWindowHeight();
	Ray camRay = cam.generateRay(u, 1.f - v, cam.getAspectRatio());

	if (event.isLeft() && event.isLeftDown())
	{
		if (activePoint != -1 && activeXYZHandle == -1 && activeTangentHandle == -1) {
			activeXYZHandle = HandleIntersect(camRay);
		}
		if (activeXYZHandle == -1 && activePoint != -1)
			activeTangentHandle = TangentIntersect(camRay);

		if (activeTangentHandle == -1 && activeXYZHandle == -1)
			activePoint = Intersect(camRay);
	}
}

void PointInterp::MouseUp(MouseEvent event)
{
	if (event.isLeft()) {
		activeXYZHandle = -1;
		activeTangentHandle = -1;
	}
}

void PointInterp::UpdatePoint(Ray ray)
{
	if(activeXYZHandle == 0)
		points[activePoint].pos.x = GetPlaneIntersect(ray).x - 0.9f;
	if (activeXYZHandle == 1)
		points[activePoint].pos.y = GetPlaneIntersect(ray).y - 0.9f;
	if (activeXYZHandle == 2)
		points[activePoint].pos.z = GetPlaneIntersect(ray).z - 0.9f;
}

void PointInterp::UpdateTangent(Ray ray, CameraPersp cam)
{
	if(currentInterpMode == hermite)
	{
		float t;
		ray.calcPlaneIntersection(points[activePoint].pos + points[activePoint].hermiteTangent, -cam.getViewDirection(), &t);
		vec3 intersection = ray.getOrigin() + ray.getDirection() * t;
		points[activePoint].hermiteTangent = intersection - points[activePoint].pos;
	}
	if (currentInterpMode == bezier)
	{
		vec3 *handle;
		vec3 *oppositeHandle;
		if (activeTangentHandle == 0){
			handle = &points[activePoint].bezierTangentF;
			oppositeHandle = &points[activePoint].bezierTangentB;
		}
		else {
			handle = &points[activePoint].bezierTangentB;
			oppositeHandle = &points[activePoint].bezierTangentF;
		}
			
		float t;
		ray.calcPlaneIntersection(points[activePoint].pos + *handle, -cam.getViewDirection(), &t);
		vec3 intersection = ray.getOrigin() + ray.getDirection() * t;
		if (activePoint != 0 && activePoint != points.size() - 1) {
			*handle = intersection - points[activePoint].pos;
			//float oppositeMagnitude = length(*oppositeHandle);
			*oppositeHandle = -normalize(*handle) * length(*oppositeHandle);
		}
		else
			*handle = intersection - points[activePoint].pos;
	}
}

void PointInterp::MouseDrag(MouseEvent event,CameraPersp cam)
{
	vec2 mousePos = event.getPos();
	float u = mousePos.x / (float)getWindowWidth();
	float v = mousePos.y / (float)getWindowHeight();
	Ray camRay = cam.generateRay(u, 1.f - v, cam.getAspectRatio());

	if (activeXYZHandle != -1) {
		UpdatePoint(camRay);
	}
	if (activeTangentHandle != -1)
	{
		UpdateTangent(camRay, cam);
	}
}


int PointInterp::Intersect(Ray ray)
{
	for (size_t i = 0; i < points.size(); ++i) {
		Sphere boundingSphere = Sphere(points[i].pos, 0.1f);
		if (boundingSphere.intersects(ray)) return i;
	}
	return -1;
}

int PointInterp::HandleIntersect(Ray ray)
{
	Sphere boundingSphere(points[activePoint].pos + vec3(0.9f, 0, 0), 0.20f);
	if (boundingSphere.intersects(ray)) return 0;
	boundingSphere = Sphere(points[activePoint].pos + vec3(0, 0.9f, 0), 0.20f);
	if (boundingSphere.intersects(ray)) return 1;
	boundingSphere = Sphere(points[activePoint].pos + vec3(0, 0, 0.9f), 0.20f);
	if (boundingSphere.intersects(ray)) return 2;
	return -1;
}

int PointInterp::TangentIntersect(Ray ray)
{
	if (currentInterpMode == hermite) {
		AxisAlignedBox boundingBox(points[activePoint].pos + points[activePoint].hermiteTangent + vec3(-0.1, -0.1, -0.1),
			points[activePoint].pos + points[activePoint].hermiteTangent + vec3(0.1, 0.1, 0.1));
		if (boundingBox.intersects(ray))
			return 0;
	}
	else if (currentInterpMode == bezier)
	{
		AxisAlignedBox boundingBox(points[activePoint].pos + points[activePoint].bezierTangentF + vec3(-0.1, -0.1, -0.1),
			points[activePoint].pos + points[activePoint].bezierTangentF + vec3(0.1, 0.1, 0.1));
		if (boundingBox.intersects(ray))
			return 0;
		boundingBox = AxisAlignedBox(points[activePoint].pos + points[activePoint].bezierTangentB + vec3(-0.1, -0.1, -0.1),
			points[activePoint].pos + points[activePoint].bezierTangentB + vec3(0.1, 0.1, 0.1));
		if (boundingBox.intersects(ray))
			return 1;
	}
	return -1;
}

vec3 PointInterp::GetPlaneIntersect(Ray ray)
{
	float t = 0;
	if (activeXYZHandle == 0) 
		t = (points[activePoint].pos.z - ray.getOrigin().z) / ray.getDirection().z;
	if (activeXYZHandle == 1) 
		t = (points[activePoint].pos.z - ray.getOrigin().z) / ray.getDirection().z;
	if (activeXYZHandle == 2) 
		t = (points[activePoint].pos.y - ray.getOrigin().y) / ray.getDirection().y;
	return ray.getOrigin() + ray.getDirection() * t;

}


std::vector<vec3> PointInterp::getDeltaPositions()
{
	std::vector<vec3> deltaPositions;

	deltaPositions.resize(points.size());
	deltaPositions[0] = points[0].pos;
	for (size_t i = 1; i < points.size(); ++i) {
		deltaPositions[i] = points[i].pos - points[i - 1].pos;
	}
	return deltaPositions;
}



void PointInterp::InsertPoint()
{
	points.push_back(RedefinedPoint());
	int last = points.size() - 1;
	if (last >= 1)
		points[last].pos = points[last-1].pos + vec3(1, 0, 0);
	if (last >= 2)
		points[last - 1].bezierTangentB = -points[last - 1].bezierTangentF;
}
std::vector<vec3> PointInterp::GetActiveSpline(float interval)
{
    if (points.size() <= 1) return std::vector<vec3>();
        
	switch (currentInterpMode)
	{
	case(line):
	{
		return GetInterpolatedLine(interval);
		break;
	}
	case(hermite):
	{
		return GetHermiteSpline(interval);
		break;
	}
	case(parabol):
	{
		return GetParabolaInterpSpline(interval);
		break;
	}
	case(bezier):
	{
		return GetBezierInterpSpline(interval);
		break;
	}
	default:
	{
		return std::vector<vec3>();
		break;
	}
	}
}



void PointInterp::ChangeMode(int mode)
{
	currentInterpMode = (interpolationMode) mode;
}


