#include "CameraSystem.h"

int CameraSystem::init() {
	devices = ps3eye::PS3EYECam::getDevices(true);

	cameraFrames = new cv::Mat[devices.size()]

	numberOfCams = devices.size();

	if (cameraFrames) {
		for (int i = 0; i < numberOfCams; i++) {
			cameraFrames[i] = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3)
		}
	}

	for (int i = 0; i < devices.size(); i++) {

		if (!devices[i]->init(WIDTH, HEIGHT, FPS)) {
			std::cout << "could not init camera" << i << "\n";
		}
		devices[i]->start();
		devices[i]->setAutogain(true);
		devices[i]->setAutoWhiteBalance(true);
	}
}

CameraSystem::~CameraSystem() {
	for (int i = 0; i < devices.size(); i++) {
		if (devices[i].isInitialized()) {
			devices[i]->stop();
		}
	}

	if (cameraFrames) {
		delete[] cameraFrames;
	}
}

int CameraSystem::getNumberOfCameras() {
	return numberOfCams;
}

cv::Mat* CameraSystem::getCameraFrames() {
	return cameraFrames;
}