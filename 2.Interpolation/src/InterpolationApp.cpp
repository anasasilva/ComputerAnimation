#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "splines.h"
#include <functional>

using namespace ci;
using namespace ci::app;
using namespace std;


class InterpolationApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseUp(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseWheel(MouseEvent event) override;
	void resize() override;
	void update() override;
	void draw() override;


private:
	void runSplineTest(); //Called when "Run Spline Test is clicked"
	void splineTestUpdate(); //Called each frame
    double getDistance(vec3 point1, vec3 point2);

	params::InterfaceGlRef interfaceRef;
	CameraPersp cam;

	PointInterp* spline;
	vec3 camLookAtPos = vec3(0, 0, 0);
	vec2 lastMousePos;
	
	gl::BatchRef mSkyBoxBatch;
	gl::TextureCubeMapRef	mCubeMap;

	bool splineTestActive = false; //Set to true in "runSplineTest"
	vec3 splineTestCube;		   //Position of the cube rendered in the draw call.
    std::vector<vec3> interList;
    std::vector<double> arcLength;
    int index;
    double startTime = 0;
    float velocity = 0.5f;
    float interval = 0.1f;

	std::vector<string> modeStrings = {"line", "hermite", "parabol","bezier"}; 
	int modeSelected = 0;
};

void InterpolationApp::setup()
{
	//Setting up the interface
	spline = new PointInterp();
	interfaceRef = params::InterfaceGl::create(getWindow(), "Interpolation", toPixels(ivec2(200, 200)));
	interfaceRef->addButton("Add Point", std::bind(&PointInterp::InsertPoint, spline), "");
	interfaceRef->addButton("Start Spline Test", std::bind(&InterpolationApp::runSplineTest, this), "");
	interfaceRef->addSeparator();
	interfaceRef->addParam("Mode", modeStrings, &modeSelected).updateFn([this] {spline->ChangeMode(modeSelected); });

	//Setting up the Skybox. Feel free to change the background
	auto skyBoxGlsl = gl::GlslProg::create(loadAsset("sky_box.vert"), loadAsset("sky_box.frag"));
	mSkyBoxBatch = gl::Batch::create(geom::Cube(), skyBoxGlsl);
	mSkyBoxBatch->getGlslProg()->uniform("uCubeMapTex", 0);
	ImageSourceRef cMapImgs[6] = { loadImage(loadAsset("cubemap/posx.jpg")),loadImage(loadAsset("cubemap/negx.jpg")),loadImage(loadAsset("cubemap/posy.jpg")),
		loadImage(loadAsset("cubemap/negy.jpg")),loadImage(loadAsset("cubemap/posz.jpg")),loadImage(loadAsset("cubemap/negz.jpg")), };
	mCubeMap = gl::TextureCubeMap::create(cMapImgs, gl::TextureCubeMap::Format().mipmap());

	//Camera settings
	cam.setEyePoint(vec3(0, 0, 10));
	cam.lookAt(vec3(0, 0, 0));

	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableVerticalSync(false);
	
}

void InterpolationApp::mouseDown( MouseEvent event )
{
	if(event.isLeft())
		spline->MouseDown(event, cam);
	if (event.isRight() && event.isRightDown())
	{
		lastMousePos = event.getPos();
	}
	if (event.isMiddle() || event.isMiddleDown())
	{
		lastMousePos = event.getPos();
	}
}

void InterpolationApp::mouseUp(MouseEvent event)
{
	spline->MouseUp(event);
}

void InterpolationApp::mouseDrag(MouseEvent event)
{
	if(event.isLeft() || event.isLeftDown())
	{
		vec2 mousePos = event.getPos();
		float u = mousePos.x / (float)getWindowWidth();
		float v = mousePos.y / (float)getWindowHeight();
		Ray camRay = cam.generateRay(u, 1.f - v, cam.getAspectRatio());

		spline->MouseDrag(event,cam);
	}
	if (event.isRight() || event.isRightDown())
	{
		vec3 right = cross(cam.getViewDirection(), cam.getWorldUp());
		vec3 up = cross(cam.getViewDirection(), right);
		float r = (event.getPos().x - lastMousePos.x) / 180.0f;
		vec3 newCamPos =  rotate(camLookAtPos - cam.getEyePoint(),r,up);
		r = (event.getPos().y - lastMousePos.y) / 180.0f;
		float t = acosf(dot(vec3(0, -1, 0), cam.getViewDirection()));
		if (acosf(dot(vec3(0, -1, 0), cam.getViewDirection())) > M_PI -0.1)
			if (r > 0)
				r = 0;
		if(acosf(dot(vec3(0, -1, 0), cam.getViewDirection())) < 0.1)
			if (r < 0)
				r = 0;
		newCamPos = rotate(newCamPos, r, right);
		cam.setEyePoint(camLookAtPos - newCamPos);
		cam.lookAt(camLookAtPos);

		lastMousePos = event.getPos();
	}
	if (event.isMiddle() || event.isMiddleDown())
	{
		vec3 right = cross(cam.getViewDirection(), cam.getWorldUp());
		vec3 up = cross(cam.getViewDirection(), right);
		float deltaX = (lastMousePos.x - event.getPos().x) * 0.05f;
		float deltaY = (lastMousePos.y - event.getPos().y) * 0.05f;
		cam.setEyePoint(cam.getEyePoint() + right * deltaX + up * deltaY);
		camLookAtPos += right * deltaX + up * deltaY;
		lastMousePos = event.getPos();
	}
}

void InterpolationApp::mouseWheel(MouseEvent event)
{
	cam.setEyePoint(cam.getEyePoint() + normalize(cam.getViewDirection()) * event.getWheelIncrement() * 0.3f);
}

void InterpolationApp::runSplineTest()
{
    interList = spline->GetActiveSpline(interval);
    
    if (interList.size() <= 1) return;
    
    splineTestActive = true;
    index = -1;
    float distance = 0;
    
    arcLength.push_back(0);
    
    for (int i = 0; i < interList.size() - 1; i++) {
        vec3 point1 = interList.at(i);
        vec3 point2 = interList.at(i + 1);
        cout << "Point1: " << point1 << endl << "Point2: " << point2 << endl << getDistance(point1, point2) << endl << endl;
        distance += getDistance(point1, point2);
        arcLength.push_back(distance);
    }
    
	//todo TASK5 - begin
	//This function is called whenever the button "Run Spline Test" is pressed
	//Use spline->GetActiveSpline(0.1f); to get a std::vector<vec3> containing all interpolated positions
	//You will most likely have to store the interpolated positions somwhere, modify the InterpolationApp class accordingly
	 //While this is true, a cube will be drawn at the vec3 "splineTestCube"
	//Your code
}

void InterpolationApp::splineTestUpdate()
{
    if (splineTestActive) {
        
        if (index == -1) {
            startTime = getElapsedSeconds();
            index = 0;
        }
        
        double deltaTime = getElapsedSeconds() - startTime;
        double distance = velocity * deltaTime;
                
        while (index < arcLength.size() && arcLength[index] < distance) {
            index++;
        }
        
        if (index >= interList.size()){
            splineTestCube = interList[interList.size() - 1];
            splineTestActive = false;
        }
        else if (index > 0 ) {
            double delta = distance - arcLength[index - 1];
            float percentage = delta/(arcLength[index] - arcLength[index - 1]);
            splineTestCube = interList[index - 1] + percentage * (interList[index] - interList[index - 1]);
            
            cout << "Delta: " << delta << endl << "Percentage: " << percentage << endl << "Coord: " << interList[index - 1] + percentage * (interList[index] - interList[index - 1]) << endl << endl;
        }
        else {
            splineTestCube = interList[index];
        }
    }
    
    
	//This function is called once per frame
	//Your code
	 //This sets the position for the cube which will be drawn if splineTestActive is set to true

	//todo TASK5 - end
}

void InterpolationApp::resize()
{
	cam.setAspectRatio(getWindowAspectRatio());
}

void InterpolationApp::update()
{
	splineTestUpdate();
}

void InterpolationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices(cam);

	gl::pushMatrices();
	mCubeMap->bind();
		gl::scale(1000.0, 1000.0, 1000.0);
		mSkyBoxBatch->draw();
	gl::popMatrices();

	gl::pushMatrices();
		spline->draw();
		interfaceRef->draw();
	gl::popMatrices();

	if (splineTestActive) {
		gl::color(Color(0.6f, 0.6f, 0.2f));
		gl::drawCube(splineTestCube, vec3(0.3, 0.3, 0.3));
	}
}

double InterpolationApp::getDistance(vec3 point1, vec3 point2) {
    
    double deltaX = point1.x - point2.x;
    double deltaY = point1.y - point2.y;
    double deltaZ = point1.z - point2.z;
    
    return sqrt(pow(deltaX, 2) + pow(deltaY, 2) + pow(deltaZ, 2));
}

CINDER_APP( InterpolationApp, RendererGl )
