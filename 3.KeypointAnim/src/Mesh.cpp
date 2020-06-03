#include "Mesh.h"
#include "glm/gtx/intersect.hpp"
#include <algorithm>

void Mesh::draw(float time)
{
	batchRef->getVao()->bind();
	batchRef->getGlslProg()->uniform("t", time);
	batchRef->draw();
}

void Mesh::SetMode(int mode)
{
	batchRef->getVao()->bind();
	batchRef->getGlslProg()->uniform("deformMode", mode);
}


Mesh::Mesh(geom::Source* geomSrc)
{
	progRef = Shaders::GetDeformationShader();
	batchRef = gl::Batch::create(*geomSrc, progRef);

	auto vborefs = batchRef->getVboMesh()->getVertexArrayVbos();
	std::vector<vec3> vertices;
	vertices.resize(vborefs.front()->getSize()/(sizeof(float)*3));
	vborefs.front()->getBufferSubData(0, vborefs.front()->getSize(), vertices.data());
	min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = vec3(FLT_MIN, FLT_MIN, FLT_MIN);

	for_each(vertices.begin(), vertices.end(), [&](vec3 vec) {
		min.x = vec.x < min.x ? vec.x : min.x;
		min.y = vec.y < min.y ? vec.y : min.y;
		min.z = vec.z < min.z ? vec.z : min.z;

		max.x = vec.x > max.x ? vec.x : max.x;
		max.y = vec.y > max.y ? vec.y : max.y;
		max.z = vec.z > max.z ? vec.z : max.z;
	});

	batchRef->getGlslProg()->uniform("lightDir", normalize(vec3(-3, 10, 0)));
	batchRef->getGlslProg()->uniform("min", min);
	batchRef->getGlslProg()->uniform("max", max);
	batchRef->getGlslProg()->uniform("t", 0.f);

}

