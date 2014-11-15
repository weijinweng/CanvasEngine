#pragma once
#include "Canvas.h"

namespace Canvas
{
	//Vertex data. Requires offset to be correct.
	struct Vertex
	{
	public:
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		Vertex();
		Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal);
	};
}