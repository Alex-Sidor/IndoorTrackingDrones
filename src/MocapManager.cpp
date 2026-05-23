#include "MocapManager.h"

bool Mocap::isRecordingPoints() {
	if (pointsToRecord == 0) {
		return false;
	}
	return true;
}

Mocap::Mocap(int numberOfCameras) {
	cameras = numberOfCameras;
}

void Mocap::recordPoint(std::vector<glm::vec2> data) {
	if (pointsToRecord == 0) {
		return;
	}

	if (data.size() != pointsToRecord) {
		return;
	}

	if (points.size() != 0) {
		for (int i = 0; i < data.size(); i++) {
			glm::vec2 previous = points[points.size() - cameras + i];
			glm::vec2 now = data[i];

			float dist = (previous - now).length();

			if (dist < 50) {
				std::cout << "sample rejected - too close to previous sample";
				return;
			}
		}
	}

	points.insert(points.end(), data.begin(), data.end());
	pointsToRecord -= 1;
	if (pointsToRecord == 0) {
		std::cout << "all points captured\n";
	}
}

void Mocap::startPointRecording(int n) {

	pointsToRecord = n;
}