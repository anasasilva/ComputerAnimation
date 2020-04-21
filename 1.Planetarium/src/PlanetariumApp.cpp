#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/params/Params.h"
#include "cinder/Easing.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <limits>

using namespace ci;
using namespace ci::app;
using namespace std;
 

//You can use this class for objects you want to render
class AstronomicalObject {

public:

	float orbit = 0.f;				// Running Variable for orbit
	float orbitRotationSpeed = 0.f; // Speed of the orbit Rotation
	vec3 orbitRotationVector;		// Rotation Vector of this Objects orbit
	vec3 orbitOffset;				// Relative Offset for this Object

	float axisRotation = 0.f;		// Variable for Rotation around own axis
	float axisRotationSpeed = 0.f;	// Speed of rotation
	vec3 axisRotationVector;		// Axis to rotate around

	float relativeScale = 1.f;		// Relative Scale of this object
	float custom = 0.f;				// Use for special values

	cinder::Color color;			// Color

	gl::TextureRef textureRef;		// Reference to the Texture this object uses
	gl::GlslProgRef shaderRef;		// Reference to the shader this object uses
	gl::BatchRef batchRef;			// Reference to the batch this geometry uses
    
    Sphere objectsBound;            // Sphere that bounds the object for picking purposes
    vec3 position;                  // Position of the object
    
    int direction = 1;              // Direction of the axis rotation of the object

	void drawTexture() {
		textureRef->bind();
		batchRef->draw();
	}
	void drawColor() {
		gl::color(color);
		batchRef->draw();
	}
    
    void setupRotation (vec3 vector, float speed);
    void setupOrbit (vec3 vector, float speed, vec3 offset);
    void setupScale (float scale);
    void setBounds (vec3 center, float radius = 0.f);
    bool testIntersection (Ray ray, float *minIntersection);
    void changeDirection ();
    void update (float deltaTime);
};

void AstronomicalObject::setupRotation(vec3 vector, float speed) {
    axisRotationVector = vector;
    axisRotationSpeed = speed;
}

void AstronomicalObject::setupOrbit(vec3 vector, float speed, vec3 offset) {
    orbitRotationVector = vector;
    orbitRotationSpeed = speed;
    orbitOffset = offset;
}

void AstronomicalObject::setupScale(float scale) {
    relativeScale = scale;
}

void AstronomicalObject::setBounds(vec3 center, float radius) {
    objectsBound.setCenter(center);
    
    if (radius != 0) {
        objectsBound.setRadius(radius);
    }
}

bool AstronomicalObject::testIntersection(Ray ray, float *minIntersection) {
    float maxIntersection;
    
    objectsBound.setCenter(position);
    return objectsBound.intersect(ray, minIntersection, &maxIntersection);
}

void AstronomicalObject::changeDirection() {
    direction *= -1;
}


void AstronomicalObject::update(float deltaTime) {
    axisRotation += axisRotationSpeed * deltaTime * direction;
    orbit += orbitRotationSpeed;
}

class PlanetariumApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void resize() override;
	vec3 getPosFromMat(mat4 matrix);
    vec3 getCurrPosition();

private:
	
	CameraPersp cam;
	enum Eobj {sun, jupiter, earth, moon, EobjFinal};
	std::vector<AstronomicalObject> astroObjects;

	bool animate = true;
	bool drawRay = false;
	bool lookAtMoon = false;
    bool prevFrame = false;
	double lastTime = 0;
	double deltaTime = 0.1;
    double prevCamDistanceSun = 1.5;
    double camDistanceSun = 1.5;

	params::InterfaceGlRef interfaceRef;

	float avgFPS = 0.f;
    
    double getYCamera();
    double getZCamera();
    
};

double PlanetariumApp::getYCamera () {
    return 8/1.5 * camDistanceSun;
};

double PlanetariumApp::getZCamera () {
    return -8/1.5 * camDistanceSun;
};

void PlanetariumApp::setup()
{
	// Camera Setup
	cam.setEyePoint(vec3(0, 8, -8));
    cam.lookAt(vec3(0, 0, 0));

	// Load Shaders
	auto textureShader = gl::ShaderDef().texture().lambert();
	auto colorShader = gl::ShaderDef().color();
	auto texShaderRef = gl::getStockShader(textureShader);
	auto colorShaderRef = gl::getStockShader(colorShader);

	// Setup Celestial Objects
	astroObjects.resize(EobjFinal);
    
    // Setup Sun
    astroObjects[sun].setupRotation(vec3(0, -1, 0), 1.f);
    astroObjects[sun].setupScale(1.5f);
    astroObjects[sun].setBounds(vec3(0, 0, 0), 1.5f);
    
    // Setup Jupiter
    astroObjects[jupiter].setupRotation(vec3(0, -1, 0), 1.f);
    astroObjects[jupiter].setupOrbit(vec3(0, 1, 0), 0.005f, vec3(4, 0, 4));
    astroObjects[jupiter].setupScale(2.f);
    astroObjects[jupiter].setBounds(vec3(0, 0, 0), 2.f);
    
    // Setup earth
    astroObjects[earth].setupRotation(vec3(0, -1, 0), 1.f);
    astroObjects[earth].setupOrbit(vec3(0, 1, 0), -0.005f, vec3(2, 0, 2));
    astroObjects[earth].setupScale(0.4f);
    astroObjects[earth].setBounds(vec3(0, 0, 0), 0.6f);
    
    // Setup moon
    astroObjects[moon].setupRotation(vec3(0, -1, 0), 1.f);
    astroObjects[moon].setupOrbit(vec3(0, 1, 0), 0.01f, vec3(0.5, 0, 0.5));
    astroObjects[moon].setupScale(0.2f);
    astroObjects[moon].setBounds(vec3(0, 0, 0), 0.2f);

	// Setup geometry
	auto sphere = geom::Sphere().subdivisions(40);
	astroObjects[sun].batchRef = gl::Batch::create(sphere, texShaderRef);
    astroObjects[jupiter].batchRef = gl::Batch::create(sphere, texShaderRef);
    astroObjects[earth].batchRef = gl::Batch::create(sphere, texShaderRef);
    astroObjects[moon].batchRef = gl::Batch::create(sphere, texShaderRef);

	// Load Textures
	auto sunTex = loadImage(loadAsset("sun.jpg"));
	astroObjects[sun].textureRef = gl::Texture::create(sunTex);
    auto jupiterTex = loadImage(loadAsset("jupiter.jpg"));
    astroObjects[jupiter].textureRef = gl::Texture::create(jupiterTex);
    auto earthTex = loadImage(loadAsset("earth.jpg"));
    astroObjects[earth].textureRef = gl::Texture::create(earthTex);
    auto moonTex = loadImage(loadAsset("moon.jpg"));
    astroObjects[moon].textureRef = gl::Texture::create(moonTex);

	// Text Window
	interfaceRef = params::InterfaceGl::create(getWindow(), "Planetarium", toPixels(ivec2(200, 200)));
	interfaceRef->addParam("FPS", &avgFPS, true);
	interfaceRef->addSeparator();
	interfaceRef->addParam("Sun Rotation", &astroObjects[sun].axisRotation ).step(0.01f).max(0.0f).min((float)(-2.0 * M_PI));
    interfaceRef->addParam("Animate", &animate);
    interfaceRef->addParam("Earth Distance", &astroObjects[earth].orbitOffset);
    interfaceRef->addParam("Moon Size", &astroObjects[moon].relativeScale).step(0.01f).max(0.3f).min(0.1f);
    interfaceRef->addSeparator();
    interfaceRef->addParam("Camera Distance", &camDistanceSun).step(0.05f).max(15.f).min(0.5f);
    interfaceRef->addParam("Look at Moon", &lookAtMoon);
    interfaceRef->addSeparator();
    interfaceRef->addParam("Draw Ray", &drawRay);

	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableVerticalSync(false);

}

//Event fired on mousedown
void PlanetariumApp::mouseDown( MouseEvent event )
{
	vec2 mousePos = event.getPos();
	float u = mousePos.x / (float) getWindowWidth();
	float v = mousePos.y / (float) getWindowHeight();
	Ray ray = cam.generateRay(u, 1.f - v, cam.getAspectRatio());
    
    float min = std::numeric_limits<float>::max();
    float tmp_min = std::numeric_limits<float>::max();
    Eobj astroObj = EobjFinal;
    
    if (astroObjects[sun].testIntersection(ray, &min)) {
        astroObj = sun;
    }
    
    if (astroObjects[jupiter].testIntersection(ray, &tmp_min) && tmp_min < min) {
        astroObj = jupiter;
    }
    
    if (astroObjects[earth].testIntersection(ray, &tmp_min) && tmp_min < min) {
        astroObj = earth;
    }
    
    if (astroObjects[moon].testIntersection(ray, &tmp_min) && tmp_min < min) {
        astroObj = moon;
    }
    
    if (astroObj != EobjFinal){
        astroObjects[astroObj].changeDirection();
    }
}

// This function is called every frame
void PlanetariumApp::update()
{
	deltaTime = (getElapsedSeconds() - lastTime);
    
	if (animate) {
        astroObjects[sun].update(deltaTime);
        astroObjects[jupiter].update(deltaTime);
        astroObjects[earth].update(deltaTime);
        astroObjects[moon].update(deltaTime);
	}
    
	lastTime = getElapsedSeconds();
	avgFPS = getAverageFps();
}

void PlanetariumApp::resize()
{
	cam.setAspectRatio(getWindowAspectRatio());
}


vec3 PlanetariumApp::getCurrPosition() {
    return getPosFromMat(gl::getModelMatrix());
}

//extracts the position of a 4x4matrix and returns it as a vec3
vec3 PlanetariumApp::getPosFromMat(mat4 matrix)
{
	vec3 vector;
	vec3 tmpVec;
	vec4 tmpVec4;
	quat tmpQuat;
	glm::decompose(matrix, tmpVec, tmpQuat, vector, tmpVec, tmpVec4);
	return vector;
}

//Called after update()
void PlanetariumApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    if (lookAtMoon) {
        cam.lookAt(astroObjects[moon].position);
    }
    else if (!lookAtMoon && prevFrame != lookAtMoon){
        cam.lookAt(vec3(0, 0, 0));
    }
    
    if (prevCamDistanceSun != camDistanceSun) {
        cam.setEyePoint(vec3(0, getYCamera(), getZCamera()));
        prevCamDistanceSun = camDistanceSun;
    }
    
    prevFrame = lookAtMoon;
    
	gl::setMatrices(cam);

	//Draw the Sun
	gl::pushModelMatrix();
        gl::translate( 0, 0, 0 );
        gl::rotate(astroObjects[sun].axisRotation, astroObjects[sun].axisRotationVector);
        gl::scale(astroObjects[sun].relativeScale, astroObjects[sun].relativeScale, astroObjects[sun].relativeScale);
        astroObjects[sun].drawTexture();
	gl::popModelMatrix();
    
    // Draw Jupiter
    gl::pushModelMatrix();
        gl::rotate(astroObjects[jupiter].orbit, astroObjects[jupiter].orbitRotationVector);
        gl::translate(astroObjects[jupiter].orbitOffset);
        gl::rotate(astroObjects[jupiter].axisRotation, astroObjects[jupiter].axisRotationVector);
        gl::scale(astroObjects[jupiter].relativeScale, astroObjects[jupiter].relativeScale, astroObjects[jupiter].relativeScale);
        astroObjects[jupiter].drawTexture();
        astroObjects[jupiter].position = getCurrPosition();
    gl::popModelMatrix();
    
    // Draw Earth
    gl::pushModelMatrix();
        gl::rotate(astroObjects[earth].orbit, astroObjects[earth].orbitRotationVector);
        gl::translate(astroObjects[earth].orbitOffset);
        gl::pushModelMatrix();
            gl::scale(astroObjects[earth].relativeScale, astroObjects[earth].relativeScale, astroObjects[earth].relativeScale);
            gl::rotate(astroObjects[earth].axisRotation, astroObjects[earth].axisRotationVector);
            astroObjects[earth].drawTexture();
            astroObjects[earth].position = getCurrPosition();
        gl::popModelMatrix();
    
        // Draw Moon
        gl::pushModelMatrix();
            gl::rotate(astroObjects[moon].orbit, astroObjects[moon].orbitRotationVector);
            gl::translate(astroObjects[moon].orbitOffset);
            gl::rotate(astroObjects[moon].axisRotation, astroObjects[moon].axisRotationVector);
            gl::scale(astroObjects[moon].relativeScale, astroObjects[moon].relativeScale, astroObjects[moon].relativeScale);
            astroObjects[moon].drawTexture();
            astroObjects[moon].position = getCurrPosition();
        gl::popModelMatrix();
    
    gl::popModelMatrix();
    

    if (drawRay) {
        auto moon_jupiter_ray = gl::VertBatch( GL_LINES );
        moon_jupiter_ray.color( Color( 0.0f, 1.0f, 0.0f ) );
        moon_jupiter_ray.vertex( astroObjects[moon].position );
        moon_jupiter_ray.vertex( astroObjects[jupiter].position );
        moon_jupiter_ray.draw();
    }
    
	interfaceRef->draw(); //draws the interface

}

CINDER_APP( PlanetariumApp, RendererGl )
