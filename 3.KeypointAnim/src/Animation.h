#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
using namespace ci;
using namespace ci::app;
using namespace std;


class Animation {
public:
	
	Animation();
	void AddKeyPoint(float duration,const std::vector<vec3> newPoints);
	bool Interpolate(float t, std::vector<vec3> &points);

	float curTime = 0;
	int	  curKP = 0;
	
	vector<vector<vec3>> keyPointPositions;
	vector<float> keyPointDurations;

private:
};