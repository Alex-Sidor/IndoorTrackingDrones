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

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

void updateOpenGLTexture(GLuint textureId, const cv::Mat& frame) {
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgbFrame.cols, rgbFrame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbFrame.data);

    glBindTexture(GL_TEXTURE_2D, 0);
}


int main() {
    // init cameras
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

    Window main;

    if (main.init()){
        return 0;
    }

    Mocap cap(2);

    GLuint camTex1, camTex2;
    glGenTextures(1, &camTex1);
    glGenTextures(1, &camTex2);

    while (!main.shouldWindowClose()) {
        glfwPollEvents();

        cam1->getFrame(frame1.data);
        cam2->getFrame(frame2.data);

        glm::vec2 p1 = TrackerDetection::findAndDrawBrightestPixel(frame1);
        glm::vec2 p2 = TrackerDetection::findAndDrawBrightestPixel(frame2);



        TrackerDetection::placeTrackerMarker(frame1, p1);
        TrackerDetection::placeTrackerMarker(frame2, p2);

        updateOpenGLTexture(camTex1, frame1);
        updateOpenGLTexture(camTex2, frame2);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Cameras");

        if (ImGui::Button("Calibrate Cameras")) {
            if (!cap.isRecordingPoints()) {
                cap.startPointRecording(50);
            }
        }
        
        bool allVisible = true;
        std::vector<glm::vec2> vec = { p1, p2 };
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i].x < 0.0f) {
                allVisible = false;
                break;
            }
        }

        if (cap.isRecordingPoints() && allVisible) {
            cap.recordPoint(vec);
        }



        ImGui::BeginGroup();
        ImGui::Text("cam 1");
        ImGui::Image((ImTextureID)(intptr_t)camTex1, ImVec2(WIDTH, HEIGHT));
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::Text("cam 2");
        ImGui::Image((ImTextureID)(intptr_t)camTex2, ImVec2(WIDTH, HEIGHT));
        ImGui::EndGroup();

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(main.getWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.16f, 0.18f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(main.getWindow());
    }

    // clean
    glDeleteTextures(1, &camTex1);
    glDeleteTextures(1, &camTex2);

    cam1->stop();
    cam2->stop();

    return 0;
}