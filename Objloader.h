#pragma once
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/vec2.hpp" // glm::vec2

#include "GL\glew.h"
#include "GL\freeglut.h"
#include <string>

// struktura danych dla wierzcho³ka
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
};

std::vector<Vertex> objload(const char* file_name);
