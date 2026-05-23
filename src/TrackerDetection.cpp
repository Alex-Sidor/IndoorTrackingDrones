#include "TrackerDetection.h"

#include <algorithm>


cv::Point TrackerDetection::findAndDrawBrightestPixel(cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    double maxVal;
    cv::Point maxLoc;
    cv::minMaxLoc(gray, nullptr, &maxVal, nullptr, &maxLoc);

    if (maxVal > 240) {
        return maxLoc;
    }
    else {
        return cv::Point(-1, -1);
    }
}

void placeTrackerMarker(cv::Mat& frame, cv::Point maxLoc) {

    int boxSize = 21;
    int halfSize = boxSize / 2;

    cv::Point topLeft(maxLoc.x - halfSize, maxLoc.y - halfSize);
    cv::Point bottomRight(maxLoc.x + halfSize, maxLoc.y + halfSize);

    topLeft.x = std::max(0, topLeft.x);
    topLeft.y = std::max(0, topLeft.y);
    bottomRight.x = std::min(frame.cols - 1, bottomRight.x);
    bottomRight.y = std::min(frame.rows - 1, bottomRight.y);

    cv::rectangle(frame, topLeft, bottomRight, cv::Scalar(0, 0, 255), -1);
}