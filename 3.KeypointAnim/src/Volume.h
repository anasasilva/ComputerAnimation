#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Mesh.h"

using namespace ci;
using namespace ci::app;

class Volume 
{
public:
	Volume(Mesh mesh);

	gl::VboMeshRef vboMeshRef;
	gl::BatchRef outlineBatchRef;
	gl::BatchRef meshBatchRef;
	gl::GlslProgRef ffdProgRef;


	std::vector<vec3> controlPointsOrig;
	std::vector<vec3> controlPoints;
	std::vector<int> edges;

	void draw();
	void draw(Mesh* mesh);
	void UpdateControlPoint(int i, vec3 pos);
	void RebufferCPs();
	void Transform(vec3 offset, vec3 scale);

private:
	void MakeCube();
};