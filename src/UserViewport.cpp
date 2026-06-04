#include "UserViewport.h"

int UserViewport::init() {
    main = new Window();

    if (main->init()) {
        return 1;
    }

    return 0;
}

void UserViewport::updateOpenGLTexture(GLuint textureId, const cv::Mat& frame) {
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rgbFrame.cols, rgbFrame.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbFrame.data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

UserViewport::~UserViewport() {

    if (main) {
        delete main;
    }

    if (camTextures) {
        for (int i = 0; i < numberOfTrackedCams; i++) {
            glDeleteTextures(1, &camTextures[i]);
        }

        delete[] camTextures;
        camTextures = nullptr;
    }
}

void UserViewport::updateTrackedCameras(CameraSystem* sys) {
    
    int numberOfNewCams = sys->getNumberOfCameras();

    if (numberOfTrackedCams == numberOfNewCams) {
        return;
    }

    if (camTextures) {
        for (int i = 0; i < numberOfTrackedCams; i++) {
            glDeleteTextures(1, &camTextures[i]);
        }
        
        delete[] camTextures;
        camTextures = nullptr;
    }

    numberOfTrackedCams = numberOfNewCams;

    if (numberOfTrackedCams == 0) {
        return;
    }

    camTextures = new GLuint[numberOfTrackedCams];

    for (int i = 0; i < numberOfTrackedCams; i++) {
        glGenTextures(1, &camTextures[i]);
    }
}

void UserViewport::update(CameraSystem* sys) {
    glfwPollEvents();

    if (!sys) {
        std::cout << "no active camera system provided\n";
        return;
    }

    updateTrackedCameras(sys);

    cv::Mat* frames = sys->getCameraFrames();

    if (sys->getNumberOfCameras() != numberOfTrackedCams) {
        std::cout << "this error should never happen, something really bad happened\n";
        return;
    }

    if (camTextures && frames) {
        for (int i = 0; i < numberOfTrackedCams; i++) {
            updateOpenGLTexture(camTextures[i], frames[i]);
        }
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Cameras");

    if (ImGui::Button("Calibrate Cameras")) {
        std::cout << "need to add calibrationz\n";
    }

    if (ImGui::Button("Refresh")) {
        sys->connectDevices(); // checks for any new cameras
    }

    ImGui::BeginGroup();


    if (camTextures) {
        for (int i = 0; i < numberOfTrackedCams; i++) {
            ImGui::Text("%d", i);
            ImGui::Image((ImTextureID)(intptr_t)camTextures[i], ImVec2(WIDTH, HEIGHT));
        }
    }
    ImGui::EndGroup();

    ImGui::End();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(main->getWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.15f, 0.16f, 0.18f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(main->getWindow());
}

bool UserViewport::shouldClose() {
    if (main) {
        return main->shouldWindowClose();
    }
    return 1;
}