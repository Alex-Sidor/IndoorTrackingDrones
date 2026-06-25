#pragma once

#include "Screen.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	
	void use();

	GLuint ID;
};