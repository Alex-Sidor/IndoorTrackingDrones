#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {
	
	nLines = maxLines;
	nPoints = maxPoints;
	nCameras = maxCameras;

	VBSize = maxLines + maxPoints + maxCameras;

	lineStack = new Line[nLines];
	pointStack = new Vec3[nPoints];
	cameraStack = new Camera[nCameras];



	glGenTextures(1, &colourBuffer);
	glBindTexture(GL_TEXTURE_2D, colourBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// bind fbo to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);


	triOutline = new Shader("shaders/project.glsl", "shaders/triOutline.glsl");

	point = new Shader("shaders/project.glsl", "shaders/point.glsl");
}

Scene::~Scene() {
	if (triOutline)
		delete triOutline;
	
	if (point) 
		delete point;

	if (lineStack)
		delete[] lineStack;

	if(pointStack)
		delete[] pointStack;

	if(cameraStack)
		delete[] cameraStack;
}

unsigned int Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target

	// render everything put in stack

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (cameraStack.size() > 0 || lineStack.size() > 0) {
		triOutline->use();

		for (size_t i = 0; i < cameraStack.size(); i++) {
			
		}

		for (size_t i = 0; i < lineStack.size(); i++) {

		}
	}

	if (pointStack.size() > 0) {
		point->use();

		for (size_t i = 0; i < pointStack.size(); i++) {

		}
	}

	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return colourBuffer;
}

// these can be called whenever, only drawn when scene is updated

void Scene::drawCamera(Camera c) {
	cameraStack.push_back(c);
}

void Scene::drawLine(Line l) {
	lineStack.push_back(l);
}

void Scene::drawPoint(Vec3 p) {
	pointStack.push_back(p);
}

//

void Scene::setColour() {
	// for all draw calls after this, draw them in this shader
}