#include "CameraSystem.h"

using namespace std::chrono;
using namespace std;
using namespace ps3eye;

int CameraSystem::init() {
	devices = PS3EYECam::getDevices(true);
	numberOfCams = devices.size();
	cameraFrames = new cv::Mat[numberOfCams];

	if (cameraFrames) {
		for (int i = 0; i < numberOfCams; i++) {
			cameraFrames[i] = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
		}
	}

	for (int i = 0; i < numberOfCams; i++) {

		if (!devices[i]->init(WIDTH, HEIGHT, FPS)) {
			cout << "could not init camera: " << i << "\n";
		}
		devices[i]->start();
		devices[i]->setAutogain(true);
		devices[i]->setAutoWhiteBalance(true);
	}

	lastFrameRead = high_resolution_clock::now();
}

CameraSystem::~CameraSystem() {
	for (int i = 0; i < numberOfCams; i++) {
		if (devices[i]->isInitialized()) {
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
	auto currentTime = high_resolution_clock::now();
	auto diff = duration_cast<microseconds>(currentTime - lastFrameRead);

	if (diff.count() < PERIOD) {
		std::cout << "Reading Frames early\n";
	}
	else {
		if (cameraFrames) {
			for (int i = 0; i < devices.size(); i++) {
				if (devices[i]->isInitialized()) {

					devices[i]->getFrame(cameraFrames[i].data);
				}
			}
		}
		else {
			std::cout << "Camera Frames not initialised\n";
		}
	}
	
	return cameraFrames;
}