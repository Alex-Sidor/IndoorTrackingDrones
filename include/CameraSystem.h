#pragma once

#include <vector>
#include <string>

#include <iostream>

#include <opencv2/opencv.hpp>

#include <chrono>

#include "ps3eye.h"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

#define PERIOD (1.0 / FPS)

class CameraSystem {
public:
	int init();

	~CameraSystem();

	int getNumberOfCameras();

	cv::Mat* getCameraFrames();

private:
	std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;

	std::chrono::steady_clock::time_point lastFrameRead;

	int numberOfCams = 0;

	cv::Mat* cameraFrames = nullptr;
};