#pragma once

#include <iostream>
#include <vector>

#include "glm/glm.hpp"

class Mocap {
public:
	Mocap::Mocap(int numberOfcameras);

	void startPointRecording(int n);

	void recordPoint(std::vector<glm::vec2> data);

	bool isRecordingPoints();

private:

	std::vector<glm::vec2> points;

	int cameras = 0;

	int pointsToRecord = 0;
};