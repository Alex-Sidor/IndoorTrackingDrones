#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "Window.h"
#include "CameraSystem.h"
#include "Scene.h"
#include "TrackerDetection.h"

class UserViewport {
public:
	int init();

	~UserViewport();

	bool shouldClose();

	void update(CameraSystem* sys, TrackerDetection* filter);

	Scene* getScene();

private:

	void updateTrackedCameras(size_t numberOfNewCams);

	void updateOpenGLTexture(GLuint textureId, const cv::Mat& frame);

	Window* main = nullptr;

	Scene* wireframeScene = nullptr;

	GLuint* camTextures = nullptr;
	int numberOfTrackedCams = 0;
};