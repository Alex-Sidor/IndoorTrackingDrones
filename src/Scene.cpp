#include "Scene.h"

Scene::Scene() {
	
	glGenTextures(1, &colourBuffer);
	glBindTexture(GL_TEXTURE_2D, colourBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// bind fbo to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);
}

unsigned int Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target

	// render everything put in stack

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return colourBuffer;
}

void Scene::drawCamera() {
	//draw camera wirefram
}

void Scene::drawLine() {
	//draw wireframe line
}

void Scene::drawPoint() {
	// draw 2d plane facing the observe, paint with a circle shader
}

void Scene::setColour() {
	// for all draw calls after this, draw them in this shader
}