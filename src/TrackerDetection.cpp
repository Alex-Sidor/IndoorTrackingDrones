#include "TrackerDetection.h"

#include <algorithm>
#include <iostream>

void TrackerDetection::findTrackers(cv::Mat* frame, size_t numberOfCams) {

    if(mode == MODE::Pause){
        return;
    }

    if (mode == MODE::Detecting) {
        clearTrackerAccumulation();
    }
    else {
        if (accumulatedPoints.size() > numOfSamples) {
            mode = MODE::Pause;
        }
    }

    if (!frame) {
        std::cout << "TrackerDetection::findTracker - passed a nullptr, no frame\n";
        return;
    }

    for (size_t i = 0; i < numberOfCams; i++) {

        cv::Mat gray; // convert into 1 color space so you can find bright spots (will make this weighted towards certain colours since trackers are not always white)
        cv::cvtColor(frame[i], gray, cv::COLOR_BGR2GRAY);

        cv::Mat thresh;

        cv::threshold(gray, thresh, 240, 255, cv::THRESH_BINARY); // make this be adaptive 

        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;

        cv::findContours(thresh, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (size_t j = 0; j < contours.size(); j++) {
            double area = cv::contourArea(contours[j]);
            if (area > 10) { // expose to settings

                cv::Rect rect = cv::boundingRect(contours[j]);

                cv::Point center(rect.x + rect.width / 2, rect.y + rect.height / 2);

                cv::circle(frame[i], center, 10, cv::Scalar(0, 0, 255), -1);

                RawPoint point;
                point.originCamera = i;
                point.xyAngleDirection = Vec2{(float)center.x, (float)center.y};

                accumulatedPoints.push_back(point);
            }
        }
    }
}

void TrackerDetection::clearTrackerAccumulation() {
    accumulatedPoints.clear();
}

void TrackerDetection::startSampling(size_t numSamplesToCapture) {
    mode = MODE::Sampling;
    numOfSamples = numSamplesToCapture;
}

void TrackerDetection::stop() {
    mode = MODE::Pause;
}

void TrackerDetection::startDetecting() {
    mode = MODE::Detecting;
}