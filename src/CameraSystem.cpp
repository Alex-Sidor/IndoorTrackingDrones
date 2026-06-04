#include "CameraSystem.h"

using namespace std::chrono;
using namespace std;
using namespace ps3eye;

void CameraSystem::cameraReadThread() {
	
	while (cameraThreadShouldRun) {
		auto currentTime = high_resolution_clock::now();

		std::chrono::duration<double> timeTaken = currentTime - lastFrameRead;

		lastFrameRead = currentTime;

		if (timeTaken < period) {
			std::this_thread::sleep_for(period - timeTaken);
		}
		else {
			std::cout << "Camera Read Thread Cannot Keep Up\n";
		}

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
}

void CameraSystem::connectDevices() {
	lastFrameRead = high_resolution_clock::now();

	vector<ps3eye::PS3EYECam::PS3EYERef> currentDevices = PS3EYECam::getDevices(true);

	if (devices.size() == currentDevices.size()) {
		return; // no added/removed cams
	}

	cameraThreadShouldRun = false;

	if (cameraWorker.joinable()) {
		cameraWorker.join(); // if this function has already been ran, there might already be a thread listening to camera. It needs to be stopped while the pointers get moved
	}

	numberOfCams = currentDevices.size();

	devices = currentDevices;

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

	for (int i = 0; i < numberOfCams; i++) {

		if (!devices[i]->init(WIDTH, HEIGHT, FPS)) {
			cout << "could not init camera: " << i << "\n";
		}
		devices[i]->start();
		devices[i]->setAutogain(true);
		devices[i]->setAutoWhiteBalance(true);
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