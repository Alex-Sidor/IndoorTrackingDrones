#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {

	// lines take 2 vectors
	// points take 1 vector
	// cameras take 3 vectors


	lineStack.init(maxLines, 2);
	pointStack.init(maxPoints, 1);
	cameraStack.init(maxCameras, 3);

	sizeOfVectorBuffer = maxLines*2 + maxPoints* 1 + maxCameras*3;

	vertexBuffer = new Vec3[sizeOfVectorBuffer];

	size_t amountOfBytes = sizeOfVectorBuffer * sizeof(Vec3); 

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);



	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, amountOfBytes, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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

	if (vertexBuffer)
		delete[] vertexBuffer;
}

GLuint Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target

	// render everything put in stack

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	size_t index = 0;

	if (cameraStack.getFilledSize() > 0 || lineStack.getFilledSize() > 0) {
		triOutline->use();

		for (size_t i = 0; i < cameraStack.getFilledSize(); i++) {
			vertexBuffer[index] = *cameraStack.getObject(i);
			index++;
		}



		for (size_t i = 0; i < lineStack.getFilledSize(); i++) {
			vertexBuffer[index] = *lineStack.getObject(i);
			index++;
		}
	}

	if (pointStack.getFilledSize() > 0) {
		point->use();

		for (size_t i = 0; i < pointStack.getFilledSize(); i++) {
			vertexBuffer[index] = *pointStack.getObject(i);
			index++;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // set render target

	// render buffer
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, index);


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