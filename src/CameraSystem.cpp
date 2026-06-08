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

	uint8_t* buf = new uint8_t[frameSize];
	

	while (cameraThreadShouldRun) {

		auto start = steady_clock::now();

		if (cameraFrames0 && cameraFrames1) {
			for (int i = 0; i < devices.size(); i++) {
				std::cout << getTime(start) << " ";
				if (devices[i]->isInitialized()) {
					std::cout << getTime(start) << " ";
					devices[i]->getFrame(buf);
					std::cout << getTime(start) << "\n\n";

					if (writingToBuf0) {
						devices[i]->debayerWrapper(WIDTH, HEIGHT, buf, cameraFrames0[i].data, true); 
						// sorry for the magic "true" but all ps3 eye cameras ive encountered are bgr
						// In later stages ill make it so you can configure per camera, as of right now I don't have a clue how to do that
					}
					else {
						devices[i]->debayerWrapper(WIDTH, HEIGHT, buf, cameraFrames1[i].data, true);
					}
				}
			}
		}
		else {
			std::cout << "Camera Frames not initialised\n";
		}

		writingToBuf0 = !writingToBuf0;
	}

	if (buf) {
		delete[] buf;
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

		if (!currentDevices[i]->init(WIDTH, HEIGHT, FPS, PS3EYECam::EOutputFormat::Bayer)) {
			cout << "could not init camera: " << i << "\n";
		}
		currentDevices[i]->start();
		currentDevices[i]->setAutogain(true);
		currentDevices[i]->setAutoWhiteBalance(true);
	}

	devices.reserve(devices.size() + currentDevices.size());
	devices.insert(devices.end(), currentDevices.begin(), currentDevices.end());

	numberOfCams += currentDevices.size();

	if (cameraFrames0) {
		delete[] cameraFrames0;
		cameraFrames0 = nullptr;
	}

	if (cameraFrames1) {
		delete[] cameraFrames1;
		cameraFrames1 = nullptr;
	}

	if (numberOfCams == 0) {
		cout << "No Cameras connected\n";
		return;
	}
	else {
		cout << numberOfCams << " Cameras connected\n";
	}

	cameraFrames0 = new cv::Mat[numberOfCams];
	cameraFrames1 = new cv::Mat[numberOfCams];

	if (cameraFrames0) {
		for (int i = 0; i < numberOfCams; i++) {
			cameraFrames0[i] = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
		}
	}

	if (cameraFrames1) {
		for (int i = 0; i < numberOfCams; i++) {
			cameraFrames1[i] = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
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

	if (cameraFrames0) {
		delete[] cameraFrames0;
		cameraFrames0 = nullptr;
	}

	if (cameraFrames1) {
		delete[] cameraFrames1;
		cameraFrames1 = nullptr;
	}
}

int CameraSystem::getNumberOfCameras() {
	return numberOfCams;
}

cv::Mat* CameraSystem::getCameraFrames() {
	if (writingToBuf0) {
		return cameraFrames1;
	}
	else {
		return cameraFrames0;
	}
}