#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h> 

#include <vector>
#include "Vector.h"
#include "Shader.h"

#include "Mat.h"

struct Line {
	Vec3 a;
	Vec3 b;
};

struct Camera {
	Vec3 position;
	Vec3 rotation;
	Vec2 xyFov;
};

const Vec3 defaultCameraFacing = { 0,0,1 }; // facing z positive, this represents the euler rotation transform of 0

class Scene {
public:

	Scene(size_t maxLines, size_t maxPoints, size_t maxCameras);

	GLuint update();

	void drawCamera(Camera c);

	void drawLine(Line l);

	void drawPoint(Vec3 p);

	void setColour();

private:

	// camera

	Camera sceneCamera;

	float fov = 90;

	Mat3x3 rotationMat;

	//


	size_t mxCameras;
	size_t mxLines;
	size_t mxPoints;

	size_t sizeOfVectorBuffer;

	std::vector<Camera> cameraStack;
	std::vector<Line> lineStack;
	std::vector<Vec3> pointStack;

	Shader* shader;

	GLuint colourBuffer;
	GLuint fbo;

	Vec3* vertexBuffer;

	GLuint VAO, VBO;
};