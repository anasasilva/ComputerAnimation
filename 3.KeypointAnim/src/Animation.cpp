#include "Animation.h"

Animation::Animation()
{
}

void Animation::AddKeyPoint(float duration, const std::vector<vec3> newPoints)
{
	if (keyPointPositions.size() != 0) //Ignore duration for the first position
		keyPointDurations.push_back(duration);
	keyPointPositions.push_back(std::vector<vec3>(newPoints));
}

bool Animation::Interpolate(float t, std::vector<vec3> &points)
{
	if (curTime + t > keyPointDurations[curKP]){
		curTime = fmod(curTime + t, keyPointDurations[curKP]);
		curKP++;
		if (curKP == keyPointDurations.size()) {
			curKP = 0;
			curTime = 0.f;
			return false;
		}
	}
	else {
		curTime += t;
	}

	for (size_t i = 0; i < points.size(); ++i) {
		vec3 t = keyPointPositions[curKP][i] + (curTime / keyPointDurations[curKP]) * (keyPointPositions[curKP+1][i] - keyPointPositions[curKP][i]);
		points[i] = keyPointPositions[curKP][i] + (curTime / keyPointDurations[curKP]) * (keyPointPositions[curKP+1][i] - keyPointPositions[curKP][i]);
	}
	return true;	
}

