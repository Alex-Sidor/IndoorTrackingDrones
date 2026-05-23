#pragma once

#include <iostream>

class Mocap {
public:

	void startPointRecording(int n);

	void recordPoint();

	bool isRecordingPoints();

private:

	int pointsToRecord = 0;
};