#include <gl\glew.h>
#include "MeGLWindow.h"
#include "qkeyevent.h"
#include <QtCore\qtimer.h>
#include <fstream>
#include <iostream>
#include <random>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\perpendicular.hpp>
#include "Vertex.h"
#include <ShapeGenerator.h>;
using namespace std;
using glm::vec3;
using glm::mat4;

GLuint programID;
GLuint numIndices;




MeGLWindow::MeGLWindow()
{
	//Update per frame
	/*QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);*/

	setWindowTitle(tr("Draw 3D Cubes"));

}


MeGLWindow::~MeGLWindow()
{
}

bool checkStatus(GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint Status;
	objectPropertyGetterFunc(objectID, statusType, &Status);
	if (Status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{	
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void MeGLWindow::initializeGL()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);

	//Send data to OpenGL
	shapeData shape = ShapeGenerator::makeCube();


	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	//Getting data from the ShapeGenerator
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));
	
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, 
		GL_STATIC_DRAW);	
	
	numIndices = shape.numIndices;

	shape.cleanUp(); //Clean up memory

	//Install shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
	{
		return;
	}

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
	{
		return;
	}

	glUseProgram(programID);
}

void MeGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	
	mat4 projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 10.0f);
	mat4 projectionTranslationMatrix = glm::translate(projectionMatrix, vec3(0.0f, 0.0f, -3.0f));
	mat4 fullTransformMatrix = glm::rotate(projectionTranslationMatrix, 0.0f, vec3(1.0f, 0.0f, 0.0f));
			
	GLint fullTransformMatrixUniformLocatioon = glGetUniformLocation(programID,
		"fullTransformMatrix");
	
	glm::vec4 vec(0.0f, 0.0f, -1.0f, 1.0f);
	vec = fullTransformMatrix * vec;
	cout << vec.x << "," << vec.y <<","<< vec.z << endl;

	glUniformMatrix4fv(fullTransformMatrixUniformLocatioon, 1,
		GL_FALSE, &fullTransformMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

	//glFlush();
}

void MeGLWindow::keyPressEvent(QKeyEvent *e)
{
	/*if (e->key() == Qt::Key_W)
	{
		posLeft[1] += 0.1f;		
	}	
	QWidget::keyPressEvent(e);*/
}

void MeGLWindow::update(int triNum)
{	
		

}



