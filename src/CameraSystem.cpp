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

int CameraSystem::init() {
	period = std::chrono::duration<double>(1.0 / FPS);
	
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

	cameraThreadShouldRun = true;
	std::thread cameraWorker(cameraReadThread);
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
	}
}

int CameraSystem::getNumberOfCameras() {
	return numberOfCams;
}

cv::Mat* CameraSystem::getCameraFrames() {
	return cameraFrames;
}