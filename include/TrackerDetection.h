#pragma once

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

namespace TrackerDetection {
	cv::Point findAndDrawBrightestPixel(cv::Mat& frame);

	void placeTrackerMarker(cv::Mat& frame);
}