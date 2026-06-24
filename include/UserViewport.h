#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "Window.h"
#include "CameraSystem.h"
#include "Scene.h"

class UserViewport {
public:
	int init();

	~UserViewport();

	bool shouldClose();

	void update(CameraSystem* sys);

private:

	void updateTrackedCameras(CameraSystem* sys);

	void updateOpenGLTexture(GLuint textureId, const cv::Mat& frame);

	Window* main = nullptr;

	Scene* wireframeScene = nullptr;

	GLuint* camTextures = nullptr;
	int numberOfTrackedCams = 0;
};