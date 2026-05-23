#include <iostream>
#include "ps3eye.h"
#include <opencv2/opencv.hpp>

#define WIDTH 640
#define HEIGHT 480
#define FPS 75

void findAndDrawBrightestPixel(cv::Mat& frame) {
    cv::Mat gray;

    // 1. Convert the frame to grayscale to judge brightness
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // 2. Find the location of the absolute brightest pixel
    double maxVal;
    cv::Point maxLoc;

    // minMaxLoc finds the min value, max value, and their respective coordinates
    cv::minMaxLoc(gray, nullptr, &maxVal, nullptr, &maxLoc);

    // Optional: Filter out low-light noise. 
    // If the "brightest" pixel isn't actually bright (e.g., in a pitch-black room), ignore it.
    if (maxVal > 240) {
        int boxSize = 21;
        int halfSize = boxSize / 2; // 10 pixels on each side of the center

        // 3. Define the top-left and bottom-right corners of the 21x21 box
        cv::Point topLeft(maxLoc.x - halfSize, maxLoc.y - halfSize);
        cv::Point bottomRight(maxLoc.x + halfSize, maxLoc.y + halfSize);

        // 4. Bound the coordinates so they don't draw outside the screen edges (prevents crashes)
        topLeft.x = std::max(0, topLeft.x);
        topLeft.y = std::max(0, topLeft.y);
        bottomRight.x = std::min(frame.cols - 1, bottomRight.x);
        bottomRight.y = std::min(frame.rows - 1, bottomRight.y);

        // 5. Draw the solid red block (BGR: 0, 0, 255)
        // Thickness = -1 fills the rectangle completely
        cv::rectangle(frame, topLeft, bottomRight, cv::Scalar(0, 0, 255), -1);
    }
}

int main() {
    const auto& devices = ps3eye::PS3EYECam::getDevices(true);

    if (devices.size() < 2) {
        std::cerr << "Error: Found " << devices.size() << " PS3 Eye device(s). You need at least 2 connected!" << std::endl;
        return 1;
    }

    std::cout << "Found " << devices.size() << " devices. Initializing the first two..." << std::endl;

    const auto cam1 = devices[0];
    const auto cam2 = devices[1];

    if (!cam1->init(WIDTH, HEIGHT, FPS)) {
        std::cerr << "Could not initialize PS3 Eye Camera 1" << std::endl;
        return 1;
    }
    cam1->start();
    cam1->setAutogain(false);

    if (!cam2->init(WIDTH, HEIGHT, FPS)) {
        std::cerr << "Could not initialize PS3 Eye Camera 2" << std::endl;
        cam1->stop();
        return 1;
    }
    cam2->start();
    cam2->setAutogain(false);

    cv::Mat frame1 = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
    cv::Mat frame2 = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);

    volatile bool running = true;

    while (running) {
        cam1->getFrame(frame1.data);
        cam2->getFrame(frame2.data);

        // Process frames for both cameras to locate the top hotspot
        findAndDrawBrightestPixel(frame1);
        findAndDrawBrightestPixel(frame2);

        cv::imshow("Camera 1 (Left)", frame1);
        cv::imshow("Camera 2 (Right)", frame2);

        auto key = cv::waitKey(1);
        if (key == 27) { // ESC key to exit
            running = false;
        }
    }

    cam1->stop();
    cam2->stop();

    return 0;
}