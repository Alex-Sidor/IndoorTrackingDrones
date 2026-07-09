#pragma once

#include "Vector.h"
#include <vector>
#include "glm/glm.hpp"
#include <opencv2/opencv.hpp>

struct RawPoint {
	size_t originCamera;
	Vec2 xyAngleDirection; 
};

class TrackerDetection {
public:

	void clearTrackerAccumulation();

	void findTrackers(cv::Mat* frame, size_t numberOfCams);

private:

	std::vector<RawPoint> accumulatedPoints;
};

	

	
