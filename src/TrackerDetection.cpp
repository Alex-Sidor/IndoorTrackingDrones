#include "TrackerDetection.h"

#include <algorithm>
#include <iostream>

glm::vec2 TrackerDetection::findTracker(cv::Mat* frame) {
    if (!frame) {
        return glm::vec2(NAN);
        std::cout << "TrackerDetection::findTracker - passed a nullptr, no frame\n";
    }
    
    cv::Mat gray;
    cv::cvtColor(*frame, gray, cv::COLOR_BGR2GRAY);

    double maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(gray, nullptr, &maxVal, nullptr, &maxLoc);

    glm::vec2 p = glm::vec2(maxLoc.x, maxLoc.y);

    if (maxVal > 240) {
        return p;
    }
    else {
        return glm::vec2(NAN);
    }
}

void TrackerDetection::placeTrackerMarker(cv::Mat* frame, glm::vec2 p) {
    if (!frame) {
        std::cout << "TrackerDetection::placeTrackerMarker - passed a nullptr, no frame\n";
        return;
    }

    int boxSize = 21;
    int halfSize = boxSize / 2;

    cv::Point topLeft(p.x - halfSize, p.y - halfSize);
    cv::Point bottomRight(p.x + halfSize, p.y + halfSize);

    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min((*frame).cols - 1, bottomRight.x);
    bottomRight.y = std::min((*frame).rows - 1, bottomRight.y);

    cv::rectangle(*frame, topLeft, bottomRight, cv::Scalar(0, 0, 255), -1);
}