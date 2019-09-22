#include <gl\glew.h>
#include "MeGLWindow.h"
#include "qkeyevent.h"
#include <QtCore\qtimer.h>
#include <fstream>
#include <iostream>
using namespace std;

GLuint programID;
GLint startLocation;
GLint moveLocation;
GLint colors;
GLfloat posLeft[2];
GLfloat posRight[2];

MeGLWindow::MeGLWindow()
{
	//Update per frame
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(32);

	setWindowTitle(tr("Two Triangles"));

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
	

	//Send data to OpenGL
	GLfloat verts[] =
	{
		-1.0f, 0.0f,
		0.0f, -1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 0.1f,
		-0.1f,-0.1f,
		0.1f, -0.1f,
	};

	//Initialize the position offset
	posLeft[0] = 0;
	posLeft[1] = 0;
	posRight[0] = 0;
	posRight[1] = 0;

	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	/*GLushort indicies[] = { 0,1,2,3 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), 
		indicies, GL_STATIC_DRAW);*/
	

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


	//Set uniforms
	startLocation = glGetUniformLocation(programID, "startOffset");	 

	colors = glGetUniformLocation(programID, "theColor");
	
}



void Draw(int shapeNum)
{		
	//glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
	if(shapeNum==1)
		glDrawArrays(GL_QUADS, 0, 4);
	if(shapeNum==2)
		glDrawArrays(GL_TRIANGLES, 4, 3);
}

void MeGLWindow::paintGL()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, width(), height());

	update(1);
	Draw(1);
	update(2);
	Draw(2);

	glFlush();
}

void MeGLWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_W)
	{
		posLeft[1] += 0.1f;		
	}
	if (e->key() == Qt::Key_S)
	{
		posLeft[1] += -0.1f;		
	}
	if (e->key() == Qt::Key_A)
	{
		posLeft[0] += -0.1f;
	}
	if (e->key() == Qt::Key_D)
	{
		posLeft[0] += 0.1f;
	}
	if (e->key() == Qt::Key_Up)
	{
		posRight[1] += 0.1f;
	}
	if (e->key() == Qt::Key_Down)
	{
		posRight[1] += -0.1f;
	}
	if (e->key() == Qt::Key_Left)
	{
		posRight[0] += -0.1f;
	}
	if (e->key() == Qt::Key_Right)
	{
		posRight[0] += 0.1f;
	}
	if (e->key() == Qt::Key_Escape)
	{
		close();
	}
	QWidget::keyPressEvent(e);
}

void MeGLWindow::update(int triNum)
{	
	if (triNum == 1)
	{
		glUniform2fv(startLocation, 1, posLeft);
		glUniform3f(colors, 0.0f, 1.0f, 0.0f);		
	}
	if (triNum == 2)
	{
		glUniform2fv(startLocation, 1, posRight);
		glUniform3f(colors, 0.0f, 0.0f, 1.0f);
	}	

}



