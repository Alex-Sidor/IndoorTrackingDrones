#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h> 

#include <vector>
#include "Vector.h"
#include "Shader.h"

struct Line {
	Vec3 a;
	Vec3 b;
};

struct Camera {
	Vec3 position;
	Vec3 forward;
	Vec3 up;
};

class Scene {
public:

	Scene();

	~Scene();

	unsigned int update();

	void drawCamera(Camera c);

	void drawLine(Line l);

	void drawPoint(Vec3 p);

	void setColour();

private:

	unsigned int colourBuffer;
	unsigned int fbo;

	std::vector<Camera> cameraStack;
	std::vector<Line> lineStack;
	std::vector<Vec3> pointStack;


	Shader* triOutline;
	
	Shader* point;

};