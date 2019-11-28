#include <gl\glew.h>
#include "MeGLWindow.h"
#include "qkeyevent.h"
#include <QtCore\qtimer.h>
#include <fstream>
#include <iostream>
#include <random>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\perpendicular.hpp>
#include "Vertex.h"
#include <ShapeGenerator.h>
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::mat4;

Camera camera;
GLuint programID;
GLuint numIndices;
float angle = 0;
vec3 cameraMove = vec3(0, 0, 0);
vec3 lookDirection = vec3(0, 0, -1);
mat4 projectionMatrix;
mat4 lookMatrix;
mat4 projectionTranslationMatrix;
mat4 rotateMatrix;
mat4 worldSpaceMatrix;
mat4 fullTransformMatrix;

//Load texture file
const char * texName = "jamie.png";
QImage JamieImg = QGLWidget::convertToGLFormat(QImage(texName));


float ambientLight = 0.3f;
vec3 lightPosition(0.0f, 20.0f, 5.0f);

GLint fullTransformMatrixUniformLocation;
GLint worldSpaceTransformUniformLocation;


MeGLWindow::MeGLWindow()
{
	//Update per frame
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);

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
	//Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);


	//Send data to OpenGL
	//ShapeData shape = ShapeGenerator::makeTriangle();
	ShapeData shape = ShapeGenerator::makeTeapot();
	
	//Cope file to OpenGL
	glActiveTexture(GL_TEXTURE0);
	GLuint tid;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, JamieImg.width(),
		JamieImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		JamieImg.bits());
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	//Getting data from the ShapeGenerator
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (char*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (char*)(sizeof(float) * 6));
	
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

	programID = glCreateProgram();
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
	
	projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 20.0f);
	lookMatrix = glm::lookAt(cameraMove, cameraMove + lookDirection, vec3(0, 1, 0));
	
	worldSpaceMatrix = glm::translate(vec3(0, -1.5f, -5.0f)) * glm::rotate(angle, vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
	fullTransformMatrix = projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(0, -1.5f, -5.0f)) * glm::rotate(angle, vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(-90.0f, vec3(1.0f, 0.0f, 0.0f));
	
	//set up lighting
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambient");	
	glUniform1f(ambientLightUniformLocation, ambientLight);

	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPosition");	
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	GLint cameraPositionUniformLocation = glGetUniformLocation(programID, "cameraPosition");
	glUniform3fv(cameraPositionUniformLocation, 1, &camera.position[0]);


	worldSpaceTransformUniformLocation = glGetUniformLocation(programID, "worldSpaceMatrix");
	glUniformMatrix4fv(worldSpaceTransformUniformLocation, 1, GL_FALSE, &worldSpaceMatrix[0][0]);

	fullTransformMatrixUniformLocation = glGetUniformLocation(programID,
		"fullTransformMatrix");	   	
	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1,
		GL_FALSE, &fullTransformMatrix[0][0]);	


	//Set the texture sampler uniform to refer to texture unit 0
	GLint textureLocation = glGetUniformLocation(programID, "JTexture");
	glUniform1i(textureLocation, 0);


	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);


	update();
	glFlush();
}

void MeGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	camera.mouseUpdate(glm::vec2(e->x(), e->y()));	
}

void MeGLWindow::wheelEvent(QWheelEvent *w)
{
	if (w->delta() > 0)
		camera.moveForward();
	if (w->delta() < 0)
		camera.moveBackward();
}

void MeGLWindow::keyPressEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key::Key_W:
		camera.moveUp();
		break;
	case Qt::Key::Key_S:
		camera.moveDown();
		break;
	case Qt::Key::Key_A:
		camera.strafeLeft();
		break;
	case Qt::Key::Key_D:
		camera.strafeRight();
		break;
	case Qt::Key::Key_Escape:
		close();
		break;
	case Qt::Key::Key_Up:
		lightPosition.y += 0.5f;
		break;
	case Qt::Key::Key_Down:
		lightPosition.y -= 0.5f;
		break;
	case Qt::Key::Key_Left:
		lightPosition.x -= 0.5f;
		break;
	case Qt::Key::Key_Right:
		lightPosition.x += 0.5f;
		break;	
	}
	QWidget::keyPressEvent(e);
}

void MeGLWindow::update()
{	
	angle += 0.5;
	if (angle > 360) angle = 0;

}



