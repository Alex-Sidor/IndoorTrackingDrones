#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h> 

class Scene {
public:

	Scene();

	unsigned int update();

	void drawCamera();

	void drawLine();

	void drawPoint();

	void setColour();

private:

	unsigned int colourBuffer;
	unsigned int fbo;
};