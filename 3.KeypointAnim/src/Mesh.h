#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "Shaders.h"

using namespace ci;
using namespace ci::app;

class Mesh {
public:
	Mesh(geom::Source* geomSrc);

	gl::GlslProgRef progRef;
	gl::BatchRef batchRef;
	
	void draw(float time);
	
	void SetMode(int mode);

	vec3 min,max;

	

private:

};

