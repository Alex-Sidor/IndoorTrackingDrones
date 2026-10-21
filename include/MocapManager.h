#pragma once

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "glm/glm.hpp"
#include "CameraSystem.h"

#include "Vector.h"

#include "TrackerDetection.h"

enum MODE {
	Detecting = 0,
	Sampling = 1,
	Paused = 2
};

class Mocap {
public:
	void update(CameraSystem* sys);
	
	void startPointRecording(int n);

	void recordPoint(std::vector<glm::vec2> data);

	void getFloorSample()



	// settings for detection of trackers

	int detectionArea = 100;
	int detectionBrightness = 250;

private:
	
	// transformation from raw to world space

	Vec3 baseOffset;
	Vec3 baseNormalDirection;
	
	Vec3 distanceSamples[2];
	float rawDistance;


	MODE mode = MODE::Paused;

	int cameras = 0;
	int pointsToRecord = 0;
};