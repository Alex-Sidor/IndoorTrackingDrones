#pragma once

#include "Vector.h"
#include <vector>
#include "glm/glm.hpp"
#include <opencv2/opencv.hpp>

struct RawPoint {
	size_t originCamera;
	Vec2 xyAngleDirection; 
};

enum MODE {
	Detecting = 0,
	Sampling = 1,
	Pause = 2
};

class TrackerDetection {
public:

	void clearTrackerAccumulation();

	void findTrackers(cv::Mat* frame, size_t numberOfCams);

	void startSampling(size_t numSamplesToCapture);

	void stop();

	void startDetecting();

	// detection params

	int detectionArea = 300;
	int detectionBrightness = 250;

private:

	MODE mode = MODE::Pause;

	size_t numOfSamples = 0;

	std::vector<RawPoint> accumulatedPoints;
};

	

	
