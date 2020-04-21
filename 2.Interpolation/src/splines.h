#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RedefinedPoint {
public:
	RedefinedPoint() {
		auto sphere = geom::Sphere().subdivisions(10).radius(0.1f);
		auto colorShader = gl::ShaderDef().color();
		auto colorShaderRef = gl::getStockShader(colorShader);
		batchRef = gl::Batch::create(sphere, colorShaderRef);
		hermiteTangent = vec3(0, 1.5, 0);
		bezierTangentF = vec3(0, 1, 1);
		bezierTangentB = vec3(0, -1, -1);
	};

public:
	vec3 pos;				//Position of the Control Point
	vec3 hermiteTangent;	//Relative position of the hermite tangent
	vec3 bezierTangentF;	//Relative position of the bezier tangent pointing forward
	vec3 bezierTangentB;	//Relative position of the bezier tangent pointing backward
	gl::BatchRef batchRef;
};

class PointInterp {
public:
	enum interpolationMode {line, hermite, parabol, bezier};

	std::vector<RedefinedPoint> points;
	void InsertPoint();

	std::vector<vec3> GetActiveSpline(float interval);
	std::vector<vec3> GetInterpolatedLine(float interval);
	std::vector<vec3> GetHermiteSpline(float interval);
	std::vector<vec3> GetParabolaInterpSpline(float interval);
	std::vector<vec3> GetBezierInterpSpline(float interval);

	glm::mat4 ConstructHermiteB(Point p1, Point p2);
	glm::mat4 ConstructParabolaB(Point p1, Point p2, Point p3, Point p4);
	glm::mat4 ConstructBezierB(Point p1, Point p2);
	
	interpolationMode currentInterpMode = line;
	void ChangeMode(int mode);
	void DrawHandles();
	void MouseDown(MouseEvent event, CameraPersp cam);
	void MouseUp(MouseEvent event);
	void UpdatePoint(Ray ray);
	void UpdateTangent(Ray ray, CameraPersp cam);
	void MouseDrag(MouseEvent event, CameraPersp cam);
	int Intersect(Ray ray);
	int HandleIntersect(Ray ray);
	int TangentIntersect(Ray ray);
	vec3 GetPlaneIntersect(Ray ray);
	std::vector<vec3> getDeltaPositions();
	void draw();
	int activePoint = -1;
	int activeXYZHandle = -1;
	int activeTangentHandle = -1;
	
};



