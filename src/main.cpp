#include <iostream>

#include "glm/glm.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "ps3eye.h"
#include "TrackerDetection.h"
#include "Window.h"
#include "MocapManager.h"
#include "CameraSystem.h"

#include "UserViewport.h"

UserViewport* viewport;
CameraSystem* sys;


int main() {

    Mocap cap(2);

    sys = new CameraSystem();
    sys->connectDevices();

    viewport = new UserViewport;

    if (!viewport->init()) {
        while (!viewport->shouldClose()) {
            viewport->update(sys);

            if (sys->getNumberOfCameras() < 2) {
                //std::cout << "Not enough cameras\n";
            }
        }
    }

    if (viewport) {
        delete viewport;
    }

    if (sys) {
        delete sys;
    }

    return 0;
}