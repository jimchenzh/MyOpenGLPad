#include "ShapeGenerator.h"
#include<glm\glm.hpp>
#include <Vertex.h>
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

shapeData ShapeGenerator::makeTriangle()
{
	shapeData ret;

	Vertex verts[] =
	{
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};
	ret.numVertices = NUM_ARRAY_ELEMENTS(verts);
	//Everything in verts divided by the size of the first element
	ret.vertices = new Vertex[ret.numVertices];
	memcpy(ret.vertices, verts, sizeof(verts));
	   	
	GLushort indices[] = { 0,1,2 };
	ret.numIndices = NUM_ARRAY_ELEMENTS(indices);
	ret.indices = new GLushort[ret.numIndices];
	memcpy(ret.indices, indices, sizeof(indices));

	return ret;

}

