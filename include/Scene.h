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

class StackBuf {
public:

	void init(size_t maxObjects, size_t sizeOfObject) {

		max = maxObjects;

		objectSize = sizeOfObject;

		buf = new  Vec3[objectSize * max];

		clear();
	}

	~StackBuf() {

		if (buf) {
			delete[] buf;
		}

	}

	void addObject(Vec3* add) {
		if (!buf) {
			std::cout << "buffer in stackbuf failed to initialise";
			return;
		}
		
		memcpy(buf + current, add, objectSize * sizeof(Vec3));

		current += objectSize;
	}

	Vec3* getObject(size_t index) {
		if (!buf) {
			std::cout << "buffer in stackbuf failed to initialise";
			return;
		}

		return buf + (objectSize * index);
	}

	void clear() {
		current = 0;
	}

	size_t getFilledSize() {
		return current;
	}

	Vec3* getBufPtr() {
		return buf;
	}

private:
	size_t max;
	size_t current;

	size_t objectSize;

	Vec3* buf;
};



class Scene {
public:

	Scene(size_t maxLines, size_t maxPoints, size_t maxCameras);

	~Scene();

	unsigned int update();

	void drawCamera(Camera c);

	void drawLine(Line l);

	void drawPoint(Vec3 p);

	void setColour();

private:

	size_t VBSize;

	StackBuf cameraStack;
	StackBuf lineStack;
	StackBuf pointStack;

	unsigned int colourBuffer;
	unsigned int fbo;

	Shader* triOutline;
	
	Shader* point;

};