#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {

	sceneCamera.position = Vec3{ 0,0,0 };
	sceneCamera.rotation = Vec3{ 0,0,0 };


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


	shader = new Shader("../shaders/project.v", "../shaders/colour.f");

	rotLocation = glGetUniformLocation(shader->ID, "rotation");
	camLocation = glGetUniformLocation(shader->ID, "camPos");
}

Scene::~Scene() {
	if (shader)
		delete shader;

	if (vertexBuffer)
		delete[] vertexBuffer;
}

GLuint Scene::update() {
	// setup camera position and rotation
	
	rotationMat = Mat::createMatrixFromEuler(sceneCamera.rotation);

	Mat3x3 camMat = Mat::createMatrixFromEuler(sceneCamera.rotation * -1);
	
	sceneCamera.position = Mat::multiplyMat3x3(Vec3{ 0,0,distance }, camMat);


	size_t index = 0;

	for (size_t i = 0; i < cameraStack.size(); i++) {
		Camera tmp = cameraStack[i];

		Mat3x3 cameraMat = Mat::createMatrixFromEuler(tmp.rotation);

		Vec3 corners[4];
		Mat3x3 matrixes[4];


		/*
		0-----1
		|     |
		2-----3
		*/

		tmp.xyFov = Vec2{ tmp.xyFov.x/2,tmp.xyFov.y/2};

		corners[0] = Vec3{ tmp.xyFov.y, -tmp.xyFov.x,0 };
		corners[1] = Vec3{ tmp.xyFov.y, tmp.xyFov.x,0 };
		corners[2] = Vec3{ -tmp.xyFov.y, -tmp.xyFov.x,0 };
		corners[3] = Vec3{ -tmp.xyFov.y, tmp.xyFov.x,0 };

		matrixes[0] = Mat::createMatrixFromEuler(corners[0]);
		matrixes[1] = Mat::createMatrixFromEuler(corners[1]);
		matrixes[2] = Mat::createMatrixFromEuler(corners[2]);
		matrixes[3] = Mat::createMatrixFromEuler(corners[3]);

		const Vec3 defaultCameraFacing = { 0,0,1 }; // facing z positive, this represents the euler rotation transform of 0

		for (size_t j = 0; j < 4; j++) {
			corners[j] = Mat::multiplyMat3x3(Mat::multiplyMat3x3(defaultCameraFacing, matrixes[j]), cameraMat) + tmp.position;
		}

		Vec3 tempBuffer[12] = { corners[0],corners[1],tmp.position ,
								corners[1],corners[3],tmp.position ,
								corners[3],corners[2],tmp.position ,
								corners[2],corners[0],tmp.position };



		for (size_t j = 0; j < 12; j++) {
			vertexBuffer[index + j] = tempBuffer[j];
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

	
	// render

	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target to texture

	glViewport(0, 0, 500, 500);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(Vec3), (void*)vertexBuffer); // send data to gpu

	shader->use();

	glUniformMatrix3fv(rotLocation, 1, GL_FALSE, &rotationMat.m[0][0]);
	glUniform3fv(camLocation, 1, (float*)&sceneCamera.position);

	glBindVertexArray(VAO);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, cameraStack.size() * 12); // camera triangles

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawArrays(GL_LINES, cameraStack.size() * 12, lineStack.size() * 2); // lines


	glDrawArrays(GL_POINTS, (cameraStack.size() * 12) + (lineStack.size() * 2), pointStack.size()); // points

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

void Scene::viewportInput(Vec2 mouse, float scroll) {
	distance += scroll;

	sceneCamera.rotation += Vec3{ mouse.y,mouse.x,0 } /50;
}

void Scene::setColour() {
	// for all draw calls after this, draw them in this shader
}