#include "Volume.h"
#include "cinder/gl/gl.h"
#include <algorithm>


Volume::Volume(Mesh mesh)
{
	MakeCube();
	ffdProgRef = Shaders::GetFFDShader();
	ffdProgRef->bind();
	ffdProgRef->uniform("lightDir", normalize(vec3(2, 10, 0)));
	for (size_t i = 0; i < controlPoints.size(); ++i) {
		std::string loc = "controlPointsOrig[" + std::to_string(i) + "]";
		ffdProgRef->uniform(loc, controlPointsOrig[i]);
	}
	for (size_t i = 0; i < controlPoints.size(); ++i)
		UpdateControlPoint(i, controlPoints[i]);
	Transform(vec3(0, 0, 0),vec3(1,1,1));
}

//Draw the outlines of the Volume
void Volume::draw()
{
	outlineBatchRef->getVao()->bind();
	outlineBatchRef->draw();
}

//Draw a Mesh transformed by the volume
void Volume::draw(Mesh* mesh)
{
	mesh->batchRef->replaceGlslProg(ffdProgRef);
	mesh->batchRef->getVao()->bind();
	mesh->batchRef->draw();
	mesh->batchRef->replaceGlslProg(mesh->progRef);
	mesh->batchRef->getVao()->bind();
}

//Change the position of a control point inside the shader
void Volume::UpdateControlPoint(int i, vec3 pos)
{
	ffdProgRef->bind();
	std::string loc = "controlPoints["  + std::to_string(i) + "]";
	ffdProgRef->uniform(loc, pos);
}

//Load the current position of all control points into the shader
void Volume::RebufferCPs()
{
	vboMeshRef->bufferAttrib(geom::POSITION, sizeof(float) * 8 * 3, controlPoints.data());
	ffdProgRef->bind();
	for (int i = 0; i < 8; i++) {
		std::string loc = "controlPoints[" + std::to_string(i) + "]";
		ffdProgRef->uniform(loc, controlPoints[i]);
	}
		
}

//Unfortunately the geometry for the teapot is not generated around (0,0,0) but from 0 to 1, so this was needed to move it into the center of the box
void Volume::Transform(vec3 offset, vec3 scale)
{
	mat4 transformMat = glm::translate(offset);
	transformMat = glm::scale(transformMat, scale);
	ffdProgRef->uniform("mv", transformMat);
}


void Volume::MakeCube()
{
	controlPoints.clear();
	controlPoints.push_back(vec3(1, -1, -1));
	controlPoints.push_back(vec3(1, 1, -1));
	controlPoints.push_back(vec3(-1, 1, -1));
	controlPoints.push_back(vec3(-1, -1, -1));
	controlPoints.push_back(vec3(1, -1, 1));
	controlPoints.push_back(vec3(1, 1, 1));
	controlPoints.push_back(vec3(-1, 1, 1));
	controlPoints.push_back(vec3(-1, -1, 1));
	controlPointsOrig = std::vector<vec3>(controlPoints.begin(), controlPoints.end());


	std::vector<uint16_t> indices{ 0,1,1,2,2,3,3,0,0,4,1,5,2,6,3,7,4,5,5,6,6,7,7,4};
	

	auto layout = gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION,3);
	std::vector<gl::VboMesh::Layout> layouts = { layout };
	auto iBuf = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(uint16_t),indices.data());

	vboMeshRef = gl::VboMesh::create(8, GL_LINES, layouts,indices.size(), GL_UNSIGNED_SHORT,iBuf);
	vboMeshRef->bufferAttrib(geom::POSITION,sizeof(float)*8*3,controlPoints.data());
	auto glslprog = gl::getStockShader(gl::ShaderDef().color());
	outlineBatchRef = gl::Batch::create(vboMeshRef, glslprog);
}
