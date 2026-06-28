#include "Scene.h"

Scene::Scene(size_t maxLines, size_t maxPoints, size_t maxCameras) {

	// lines take 2 vectors
	// points take 1 vector
	// cameras take 3 vectors

	mxCameras = maxCameras;
	mxLines = maxLines;
	maxPoints = maxPoints;

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

GLuint Scene::update() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); // set render target

	// render everything put in stack

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	rotationMat = Mat::createMatrixFromEuler(sceneCamera.rotation);

	size_t index = 0;

	if (cameraStack.size() > 0 || lineStack.size() > 0) {
		triOutline->use();

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

			for (size_t i = 0; i < 4; i++){
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
			vertexBuffer[index + 2] = tmp.b;
			
			index += 3;
		}
	}

	if (pointStack.size() > 0) {
		point->use();

		for (size_t i = 0; i < pointStack.size(); i++) {
			vertexBuffer[index] = pointStack[i]
			index += 6;
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
	if(cameraStack.size() == )
	
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