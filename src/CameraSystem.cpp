#include "CameraSystem.h"

using namespace std::chrono;
using namespace std;
using namespace ps3eye;

double getTime(steady_clock::time_point start) {
	auto now = steady_clock::now();
	return duration<double, std::micro>(now - start).count();
}

void CameraSystem::cameraReadThread() {
	
	int frameSize = WIDTH * HEIGHT;

	while (cameraThreadShouldRun.load()) {

		auto start = steady_clock::now();

		if (cameraFrames) {
			for (int i = 0; i < devices.size(); i++) {
				if (devices[i]->isInitialized()) {
					auto start = steady_clock::now();
					devices[i]->getFrame(cameraFrames[i].data);
					//std::cout << getTime(start) << "\n";
				}
			}
		}
		else {
			std::cout << "Camera Frames not initialised\n";
		}

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

		if (!currentDevices[i]->init(WIDTH, HEIGHT, FPS, PS3EYECam::EOutputFormat::BGR)) {
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
		cout << "No Cameras connected\n";
		return;
	}
	else {
		cout << numberOfCams << " Cameras connected\n";
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
	cameraThreadShouldRun = false;
	if (cameraWorker.joinable()) {
		cameraWorker.join();
	}
	
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