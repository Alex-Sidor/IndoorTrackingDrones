#pragma once

#include "Vector.h"
#include <vector>
#include "glm/glm.hpp"
#include <opencv2/opencv.hpp>

namespace TrackerDetection{
	struct RawPoint {
		size_t originCamera;
		Vec2 xyAngleDirection;
	};
	
	std::vector<RawPoint> findTrackers(cv::Mat* frame, size_t numberOfCams, int detectionArea, int detectionBrightness);
};