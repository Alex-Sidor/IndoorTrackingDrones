#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {

	mxCameras = maxCameras;
	mxLines = maxLines;
	mxPoints = maxPoints;

	// 2 vectors for a line, a-b
	// 1 vector for a point, position
	// 12 vectors for a camera, 4 triangles 3 points each

	sizeOfVectorBuffer = maxLines*2 + maxPoints* 1 + maxCameras*12;

	vertexBuffer = new Vec3[sizeOfVectorBuffer];

	size_t amountOfBytes = sizeOfVectorBuffer * sizeof(Vec3); 

	// create VBO and VAO

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER, amountOfBytes, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Create render target / texture

	glGenTextures(1, &colourBuffer);
	glBindTexture(GL_TEXTURE_2D, colourBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// bind fbo to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourBuffer, 0);


	shader = new Shader("shaders/project.glsl", "shaders/colour.glsl");
}

Scene::~Scene() {
	if (shader)
		delete shader;

	if (vertexBuffer)
		delete[] vertexBuffer;
}

GLuint Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target to texture

	// render everything put in stack

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	rotationMat = Mat::createMatrixFromEuler(sceneCamera.rotation);

	size_t index = 0;

	shader->use();

	for (size_t i = 0; i < cameraStack.size(); i++) {
		Camera tmp = cameraStack[i];

		Vec3 corners[4];
		Mat3x3 matrixes[4];


		/*
		0-----1
		|     |
		2-----3
		*/

		corners[0] = tmp.rotation + Vec3{ tmp.xyFov.y, 0, -tmp.xyFov.x };
		corners[1] = tmp.rotation + Vec3{ tmp.xyFov.y, 0, tmp.xyFov.x };
		corners[2] = tmp.rotation + Vec3{ -tmp.xyFov.y, 0, -tmp.xyFov.x };
		corners[3] = tmp.rotation + Vec3{ -tmp.xyFov.y, 0, tmp.xyFov.x };

		matrixes[0] = Mat::createMatrixFromEuler(corners[0]);
		matrixes[1] = Mat::createMatrixFromEuler(corners[1]);
		matrixes[2] = Mat::createMatrixFromEuler(corners[2]);
		matrixes[3] = Mat::createMatrixFromEuler(corners[3]);

		for (size_t i = 0; i < 4; i++) {
			corners[i] = Mat::multiplyMat3x3(defaultCameraFacing, matrixes[i]) + tmp.position;
		}

		Vec3 tempBuffer[12] = { corners[0],corners[1],tmp.position ,
								corners[1],corners[3],tmp.position ,
								corners[3],corners[2],tmp.position ,
								corners[2],corners[0],tmp.position };



		for (size_t i = 0; i < 12; i++) {
			vertexBuffer[index + i] = tempBuffer[i];
		}


		index += 12;
	}

	for (size_t i = 0; i < lineStack.size(); i++) {
		Line tmp = lineStack[i];

		vertexBuffer[index + 0] = tmp.a;
		vertexBuffer[index + 1] = tmp.b;

		index += 2;
	}

	for (size_t i = 0; i < pointStack.size(); i++) {
		vertexBuffer[index] = pointStack[i];
		index += 1;
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(Vec3), (void*)vertexBuffer); // send data to gpu

	// render buffer

	glBindVertexArray(VAO);



	glDrawArrays(GL_TRIANGLES, 0, cameraStack.size() * 12);


	glDrawArrays(GL_LINES, cameraStack.size() * 12, lineStack.size());


	glDrawArrays(GL_POINTS, (cameraStack.size() * 12) + (lineStack.size() * 2), pointStack.size());




	glBindFramebuffer(GL_FRAMEBUFFER, 0); // set render target back to default

	return colourBuffer;
}

// these can be called whenever, only drawn when scene is updated

void Scene::drawCamera(Camera c) {
	if (cameraStack.size() == mxCameras) {
		return;
	}
	
	cameraStack.push_back(c);
}

void Scene::drawLine(Line l) {
	if (lineStack.size() == mxLines) {
		return;
	}
	
	lineStack.push_back(l);
}

void Scene::drawPoint(Vec3 p) {
	if (pointStack.size() == mxPoints) {
		return;
	}
	
	pointStack.push_back(p);
}

void Scene::clearDraws() {
	cameraStack.clear();
	lineStack.clear();
	pointStack.clear();
}
//

void Scene::setColour() {
	// for all draw calls after this, draw them in this shader
}