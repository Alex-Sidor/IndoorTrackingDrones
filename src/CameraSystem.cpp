#include "CameraSystem.h"

using namespace std::chrono;
using namespace std;
using namespace ps3eye;

void CameraSystem::cameraReadThread() {

	auto nextFrameTime = high_resolution_clock::now();

	period = std::chrono::duration<double>(1.0 / FPS);

	while (cameraThreadShouldRun) {
		nextFrameTime += duration_cast<nanoseconds>(period);;

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

		auto currentTime = high_resolution_clock::now();

		if (currentTime < nextFrameTime) {
			std::this_thread::sleep_until(nextFrameTime);
			std::cout << "Camera Read Thread is on time\n";
		}
		else {
			auto late = duration_cast<milliseconds>(currentTime - nextFrameTime);
			std::cout << "Camera Read Thread Cannot Keep Up, thread is " << late.count() << "ms late\n";

			nextFrameTime = high_resolution_clock::now();
		}
	}
}

void CameraSystem::connectDevices() {
	vector<ps3eye::PS3EYECam::PS3EYERef> currentDevices = PS3EYECam::getDevices(true); // this can only see devices that are not connected


	std::cout << currentDevices.size() << " new cameras detected\n";

	if (currentDevices.size() == 0) {
		return;
	}

	cameraThreadShouldRun = false;

	if (cameraWorker.joinable()) {
		cameraWorker.join(); // if this function has already been ran, there might already be a thread listening to camera. It needs to be stopped while the pointers get moved
	}

	for (int i = 0; i < currentDevices.size(); i++) {

		if (!currentDevices[i]->init(WIDTH, HEIGHT, FPS)) {
			cout << "could not init camera: " << i << "\n";
		}
		currentDevices[i]->start();
		currentDevices[i]->setAutogain(true);
		currentDevices[i]->setAutoWhiteBalance(true);
	}

	devices.reserve(devices.size() + currentDevices.size());
	devices.insert(devices.end(), currentDevices.begin(), currentDevices.end());

	numberOfCams += currentDevices.size();

	if (cameraFrames) {
		delete[] cameraFrames;
		cameraFrames = nullptr;
	}

	if (numberOfCams == 0) {
		cout << "No Cameras found\n";
		return;
	}
	else {
		cout << numberOfCams << " Cameras found\n";
	}

	cameraFrames = new cv::Mat[numberOfCams];

	if (cameraFrames) {
		for (int i = 0; i < numberOfCams; i++) {
			cameraFrames[i] = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
		}
	}

	cameraThreadShouldRun = true;
	cameraWorker = thread(&CameraSystem::cameraReadThread, this);
}

CameraSystem::CameraSystem() {
	period = std::chrono::duration<double>(1.0 / FPS);
}

CameraSystem::~CameraSystem() {
	cameraThreadShouldRun = false; // make the camera thread do its last run
	cameraWorker.join(); // sync and close it with this thread
	
	for (int i = 0; i < numberOfCams; i++) {
		if (devices[i]->isInitialized()) {
			devices[i]->stop();
		}
	}

	if (cameraFrames) {
		delete[] cameraFrames;
		cameraFrames = nullptr;
	}
}

int CameraSystem::getNumberOfCameras() {
	return numberOfCams;
}

cv::Mat* CameraSystem::getCameraFrames() {
	return cameraFrames;
}