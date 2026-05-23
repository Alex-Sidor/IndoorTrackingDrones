#pragma once

#include "glm/glm.hpp"

#include <opencv2/opencv.hpp>

namespace TrackerDetection {
	glm::vec2 findAndDrawBrightestPixel(cv::Mat& frame);

	void placeTrackerMarker(cv::Mat& frame, glm::vec2 p);
}