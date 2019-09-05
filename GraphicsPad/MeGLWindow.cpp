#include <gl\glew.h>
#include "MeGLWindow.h"



MeGLWindow::MeGLWindow()
{
}


MeGLWindow::~MeGLWindow()
{
}

void MeGLWindow::initializeGL()
{
	glewInit();

	GLfloat verts[] =
	{
		0.0f, 0.2f,
		0.5f, 0.7f,
		-0.5f,0.7f,

		-1.0f, 0.0f,
		-0.5f,-1.0f,
		-0.5f, 0.0f,

		0.5f, 0.0f,
		0.5f,-1.0f,
		1.0f, 0.0f,
	};
	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void MeGLWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	glDrawArrays(GL_TRIANGLES, 0, 9);
}

