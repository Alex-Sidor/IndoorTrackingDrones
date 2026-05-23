#include "MocapManager.h"

bool Mocap::isRecordingPoints() {
	if (pointsToRecord == 0) {
		return false;
	}
	return true;
}

void Mocap::recordPoint() {
	if (pointsToRecord != 0) {
		pointsToRecord -= 1;
	}

	if (pointsToRecord == 0) {
		std::cout << "all points captured\n";
	}
}

void Mocap::startPointRecording(int n) {
	pointsToRecord = n;
}