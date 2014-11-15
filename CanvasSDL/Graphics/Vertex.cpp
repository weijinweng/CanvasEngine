#include "Vertex.h"

namespace Canvas
{
	Vertex::Vertex() :position(glm::vec3()), uv(glm::vec2()), normal(glm::vec3())
	{}

	Vertex::Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal) : position(position), uv(uv), normal(normal)
	{}
}