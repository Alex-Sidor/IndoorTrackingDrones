#pragma once

#include <vector>
#include <string>

#include "ps3eye.h"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

class CameraSystem {
public:
	int init();

	~CameraSystem();

	int getNumberOfCameras();

	cv::Mat* getCameraFrames();

private:
	std::vector<PS3EYECam::PS3EYERef>& devices;

	int numberOfCams = 0;

	cv::Mat* cameraFrames = nullptr;
};