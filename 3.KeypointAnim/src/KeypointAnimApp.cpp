#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "Mesh.h"
#include "CamControl.h"
#include "Volume.h"
#include "Animation.h"
#include <memory>

using namespace ci;
using namespace ci::app;
using namespace std;

class KeypointAnimApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag(MouseEvent event) override;
	void update() override;
	void resize() override;
	void draw() override;

	void ChangeGeom(int geomNum);
	void SetupKeyPoints();
	CameraPersp cam;
	params::InterfaceGlRef interfaceRef;

	Mesh* mesh = nullptr;
	Volume* volume = nullptr;
	Animation animation;

	std::vector<string> modeStrings = { "taper", "twist", "bend", "other deform" };
	std::vector<string> geomStrings = { "cylinder", "cube", "teapot" };
	int mode = 0;
	int geomSelected = 0;
	float time = 0.f;
	bool ffd = false;
};

void KeypointAnimApp::setup()
{
	

	CamControl::SetCam(&cam);

	cam.setEyePoint(vec3(0, 6, -10));
	cam.lookAt(vec3(0, 0, 0));
	interfaceRef = params::InterfaceGl::create(getWindow(), "Keypoint Animation Exercise", toPixels(ivec2(200, 200)));
	interfaceRef->addParam("Mode", modeStrings, &mode).updateFn([&] {mesh->SetMode(mode); });
	interfaceRef->addParam("Geom", geomStrings, &geomSelected).updateFn([&] {ChangeGeom(geomSelected); });
	interfaceRef->addParam("K", &time).min(0.0f).max(1.0f).step(0.01f);
	interfaceRef->addParam("Enable/Disable FFD", &ffd);
	mesh = new Mesh(&geom::Cylinder().height(1).origin(vec3(0, -0.5f, 0)));
	mesh->SetMode(0);
	volume = new Volume(*mesh);
	SetupKeyPoints();

	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableVerticalSync(false);
}

void KeypointAnimApp::mouseDown( MouseEvent event )
{

}

void KeypointAnimApp::mouseDrag(MouseEvent event)
{

}

void KeypointAnimApp::update()
{
	
}

void KeypointAnimApp::resize()
{
	cam.setAspectRatio(getWindowAspectRatio());
}

void KeypointAnimApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::setMatrices(cam);
	interfaceRef->draw();
	volume->RebufferCPs();
	if (ffd) {
		animation.Interpolate(0.01f, volume->controlPoints);
		volume->draw(mesh);
		volume->draw();
	}
	else
		mesh->draw(time);
}

//Changes the active objects geometry
void KeypointAnimApp::ChangeGeom(int geomNum)
{
	if(mesh)
		delete(mesh);
	
	switch (geomNum) {
	case 0:
	{
		auto c = geom::Cylinder().height(1).origin(vec3(0, -0.5f, 0));
		mesh = new Mesh(&c);
		volume->Transform(vec3(0.f, 0.f, 0.f), vec3(1.0f, 1.0f, 1.0f));
		break;
	}
	case 1:
	{
		auto c = geom::Cube().subdivisions(10).size(vec3(1.5, 1.5, 1.5));
		mesh = new Mesh(&c);
		volume->Transform(vec3(0.f, 0.f, 0.f), vec3(1.0f, 1.0f, 1.0f));
		break;
	}
	case 2:
	{
		auto t = geom::Teapot();
		mesh = new Mesh(&t);
		volume->Transform(vec3(0.f, -0.5f, 0.f), vec3(1.0f, 1.0f, 1.0f));
		break;
	}
	}
	mesh->SetMode(mode);
}


//Helper functions to generate some control points.
void translateVector(vector<vec3> &pts, vec3 translation, int i = 0, int j = 8)
{
	for (; i < j; ++i)
		pts[i] += translation;
}

void scaleVector(vector<vec3> &pts, vec3 scale, int i = 0, int j = 8)
{
	for (; i < j; ++i)
		pts[i] *= scale;
}

void rotateVector(vector<vec3> &pts, vec3 rotationVector, float degrees,int i = 0, int j = 8)
{
	for (; i < j; ++i) {
		vec4 pt(pts[i], 1);
		mat4 rot = rotate(degrees, rotationVector);
		pts[i] =vec3(rot * pt);
	}
}


//Sets up some keypoints to move the ffd box around
void KeypointAnimApp::SetupKeyPoints()
{
	vector<vec3> kps = { vec3(1,-1,-1), vec3(1,1,-1), vec3(-1,1,-1), vec3(-1,-1,-1),
						 vec3(1,-1,1), vec3(1,1,1), vec3(-1,1,1), vec3(-1,-1,1)};
	animation.AddKeyPoint(0, kps);
	rotateVector(kps, vec3(0, 0, 1), (float)M_PI / 4);
	animation.AddKeyPoint(0.5, kps);
	scaleVector(kps, vec3(1, 1.5, 1));
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(0, 1, 0), (float)M_PI / 4,0,4);
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(1, 0, 0), (float)M_PI / 3, 2, 6);
	animation.AddKeyPoint(0.5, kps);
	scaleVector(kps, vec3(0.5, 2, 2), 2, 6);
	animation.AddKeyPoint(0.5, kps);
	translateVector(kps, vec3(0, 0.3, 0), 0, 2);
	translateVector(kps, vec3(0, 0.3, 0), 6, 8);
	animation.AddKeyPoint(0.5, kps);
	kps = { vec3(1,-1,-1), vec3(1,1,-1), vec3(-1,1,-1), vec3(-1,-1,-1),
		vec3(1,-1,1), vec3(1,1,1), vec3(-1,1,1), vec3(-1,-1,1) };
	animation.AddKeyPoint(0.5, kps);
	scaleVector(kps, vec3(2, 0.7, 0.7));
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(1, 0, 0), (float)M_PI / 3, 2, 4);
	rotateVector(kps, vec3(1, 0, 0), (float)M_PI / 3, 6, 8);
	rotateVector(kps, vec3(1, 0, 0), (float)-M_PI / 3, 0, 2);
	rotateVector(kps, vec3(1, 0, 0), (float)-M_PI / 3, 4, 6);
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(1, 0, 0), -2*(float)M_PI / 3, 2, 4);
	rotateVector(kps, vec3(1, 0, 0), -2 * (float)M_PI / 3, 6, 8);
	rotateVector(kps, vec3(1, 0, 0), -2 * (float)-M_PI / 3, 0, 2);
	rotateVector(kps, vec3(1, 0, 0), -2 * (float)-M_PI / 3, 4, 6);
	animation.AddKeyPoint(0.5, kps);
	scaleVector(kps, vec3(1, 3, 1));
	kps = { vec3(1,-1,-1), vec3(1,1,-1), vec3(-1,1,-1), vec3(-1,-1,-1),
		vec3(1,-1,1), vec3(1,1,1), vec3(-1,1,1), vec3(-1,-1,1) };
	animation.AddKeyPoint(0.5, kps);
	scaleVector(kps, vec3(0.7, 2, 0.5));
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(0, 1, 0), (float)M_PI / 3, 1, 3);
	rotateVector(kps, vec3(0, 1, 0), (float)M_PI / 3, 5, 7);
	rotateVector(kps, vec3(0, 1, 0), (float)-M_PI / 3, 0, 1);
	rotateVector(kps, vec3(0, 1, 0), (float)-M_PI / 3, 3, 5);
	rotateVector(kps, vec3(0, 1, 0), (float)-M_PI / 3, 7, 8);
	animation.AddKeyPoint(0.5, kps);
	translateVector(kps, vec3(-2, 0, 0), 1, 3);
	translateVector(kps, vec3(-2, 0, 0), 5, 7);
	translateVector(kps, vec3(2, 0, 0), 0, 1);
	translateVector(kps, vec3(2, 0, 0), 3, 5);
	translateVector(kps, vec3(2, 0, 0), 7, 8);
	animation.AddKeyPoint(0.5, kps);
	translateVector(kps, vec3(0, -1, 0), 1, 3);
	translateVector(kps, vec3(0, 1, 0), 5, 7);
	animation.AddKeyPoint(0.5, kps);
	rotateVector(kps, vec3(0, 1, 0), -2*(float)M_PI / 3, 1, 3);
	rotateVector(kps, vec3(0, 1, 0), -2 * (float)M_PI / 3, 5, 7);
	rotateVector(kps, vec3(0, 1, 0), -2 * (float)-M_PI / 3, 0, 1);
	rotateVector(kps, vec3(0, 1, 0), -2 * (float)-M_PI / 3, 3, 5);
	rotateVector(kps, vec3(0, 1, 0), -2 * (float)-M_PI / 3, 7, 8);
	animation.AddKeyPoint(0.5, kps);
	translateVector(kps, vec3(5, 0, 0), 1, 3);
	translateVector(kps, vec3(5, 0, 0), 5, 7);
	translateVector(kps, vec3(-5, 0, 0), 0, 1);
	translateVector(kps, vec3(-5, 0, 0), 3, 5);
	translateVector(kps, vec3(-5, 0, 0), 7, 8);
	animation.AddKeyPoint(0.5, kps);

	kps = { vec3(1,-1,-1), vec3(1,1,-1), vec3(-1,1,-1), vec3(-1,-1,-1),
		vec3(1,-1,1), vec3(1,1,1), vec3(-1,1,1), vec3(-1,-1,1) };
	animation.AddKeyPoint(0.5, kps);
}






CINDER_APP( KeypointAnimApp, RendererGl )
