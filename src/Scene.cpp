#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {

	VBSize = maxLines + maxPoints + maxCameras;

	lineStack.init(maxLines, 2);
	pointStack.init(maxPoints, 1);
	cameraStack.init(maxCameras, 3);

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
}

unsigned int Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target

	// render everything put in stack

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (cameraStack.getFilledSize() > 0 || lineStack.getFilledSize() > 0) {
		triOutline->use();

		for (size_t i = 0; i < cameraStack.getFilledSize(); i++) {
			
		}

		for (size_t i = 0; i < lineStack.getFilledSize(); i++) {

		}
	}

	if (pointStack.getFilledSize() > 0) {
		point->use();

		for (size_t i = 0; i < pointStack.getFilledSize(); i++) {

		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return colourBuffer;
}

// these can be called whenever, only drawn when scene is updated

void Scene::drawCamera(Camera c) {
	cameraStack.addObject((Vec3*)&c);
}

void Scene::drawLine(Line l) {
	lineStack.addObject((Vec3*)&l);
}

void Scene::drawPoint(Vec3 p) {
	pointStack.addObject((Vec3*)&p);
}

//

void Scene::setColour() {
	// for all draw calls after this, draw them in this shader
}