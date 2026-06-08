#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <atomic>

#include <chrono>
#include <thread>
#include <atomic>

#include <opencv2/opencv.hpp>
#include "ps3eye.h"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

class CameraSystem {
public:
	CameraSystem();

	~CameraSystem();

	int getNumberOfCameras();

	cv::Mat* getCameraFrames();

	void connectDevices();

private:
	std::atomic<bool> cameraThreadShouldRun = false;
	void cameraReadThread();
	std::chrono::duration<double> period;

	std::thread cameraWorker;
	
	std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;

	std::chrono::steady_clock::time_point lastFrameRead;

	int numberOfCams = 0;

	cv::Mat* cameraFrames0 = nullptr;
	cv::Mat* cameraFrames1 = nullptr;

	std::atomic<bool> writingToBuf0 = true;
};