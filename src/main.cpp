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

UserViewport* viewport; // for display and interaction
CameraSystem* sys; // for reading camera input
Mocap* mocapMngr; // for doing math and getting output

int main() {

    sys = new CameraSystem();
    sys->connectDevices();

    mocapMngr = new Mocap;

    viewport = new UserViewport;

    if (!viewport->init()) {

        mocapMngr->startDetecting();

        while (!viewport->shouldClose()) {
            
            mocapMngr->update()
            
            viewport->update(sys, mocapMngr);

            if (sys->getNumberOfCameras() < 2) {
                //std::cout << "Not enough cameras\n";
            }
        }

        mocapMngr->stop();
    }

    if (viewport)
        delete viewport;

    if (sys)
        delete sys;

    if (filter)
        delete filter;

    return 0;
}