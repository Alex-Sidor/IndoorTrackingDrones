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
   

    cv::Mat frame1 = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);
    cv::Mat frame2 = cv::Mat::zeros(cv::Size(WIDTH, HEIGHT), CV_8UC3);

    Window main;

    if (main.init()){
        return 0;
    }

    Mocap cap(2);

    CameraSystem sys;
    sys.init();

    if (sys.getNumberOfCameras() < 2) {
        std::cout << "Not enough cameras connected need " << (2 - sys.getNumberOfCameras()) << " more\n";
        return 0;
    }

    GLuint camTex1, camTex2;
    glGenTextures(1, &camTex1);
    glGenTextures(1, &camTex2);

    while (!main.shouldWindowClose()) {
        glfwPollEvents();

        cv::Mat* frames = sys.getCameraFrames();

        if (frames) {
            bool allTracked = true;

            std::vector<glm::vec2> trackedPositions;

            for (int i = 0; i < sys.getNumberOfCameras(); i++) {
                glm::vec2 out = TrackerDetection::findTracker(frames[i]);

                if (out.x == NAN) {
                    allTracked = false;
                    break;
                }

                TrackerDetection::placeTrackerMarker(frames[i], out);

                trackedPositions.push_back(out);
            }
        }


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

    return 0;
}