#include <gl\glew.h>
#include "MeGLWindow.h"
#include "qkeyevent.h"
#include <QtCore\qtimer.h>
#include <fstream>
#include <iostream>
#include <random>
#include <glm\glm.hpp>
#include <glm\gtx\perpendicular.hpp>
#include "Vertex.h"
#include <ShapeGenerator.h>;
using namespace std;

GLuint programID;
GLint startLocation;
GLint moveLocation;
GLint colors;
GLfloat posLeft[2];
GLfloat posRight[2];
GLfloat randVelocity[2];
const float diamond_Z = 0.5f;
const float shape_Z = 0.0f;
//Need to set projection and model view matrix to get the correct z direction. 
//Right now the default clip space is a left-handed coordinate system.
/*Vertex verts[] =
{
	glm::vec3(-1.0f, 0.0f, diamond_Z),
	glm::vec3(0.0f, -1.0f, diamond_Z),
	glm::vec3(1.0f, 0.0f, diamond_Z),
	glm::vec3(0.0f, 1.0f, diamond_Z),

	glm::vec3(0.0f, 0.3f, shape_Z),
	glm::vec3(-0.05f,0.0f, shape_Z),
	glm::vec3(-0.02f, -0.1f, shape_Z),
	glm::vec3(-0.03f, 0.0f, shape_Z),
	glm::vec3(0.0f, 0.1f, shape_Z),
	glm::vec3(0.03f,0.0f, shape_Z),
	glm::vec3(0.02f,-0.1f, shape_Z),
	glm::vec3(0.05f,0.0f, shape_Z),

};*/

MeGLWindow::MeGLWindow()
{
	//Update per frame
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(15);

	setWindowTitle(tr("Update and Velocity"));

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



bool edgeDetection(int edgeNum, Vertex verts[], float offset[])
{
	for (int i = 0; i < edgeNum; i++)
	{
		glm::vec3 edge = verts[(i+1) % edgeNum].position - verts[i].position;
		glm::vec3 normal = glm::cross(edge, glm::vec3(0, 0, -1));  
		glm::vec3 NormalizedNormal = glm::normalize(normal);
		glm::vec3 planePos = verts[5].position + glm::vec3(offset[0], offset[1], 0) - verts[i].position;
		if (glm::dot(planePos, normal) < 0)
		{
			cout << "Hit" << endl;
			glm::vec3 speed = glm::vec3(randVelocity[0], randVelocity[1], 0.0f);			
			speed = 2.0f * (speed + NormalizedNormal * (glm::dot(-speed, NormalizedNormal))) - speed;
			randVelocity[0] = speed.x;
			randVelocity[1] = speed.y;
			return false;
			//break;
			
		}		
	}
	return true;
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
	shapeData tri = ShapeGenerator::makeTriangle();
	

	

	//Initialize the position offset
	posLeft[0] = 0;
	posLeft[1] = 0;
	posRight[0] = 0;
	posRight[1] = 0;

	GLuint myBufferID;
	glGenBuffers(1, &myBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, myBufferID);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	//Getting data from the ShapeGenerator
	glBufferData(GL_ARRAY_BUFFER, tri.vertexBufferSize(), tri.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	/*GLushort indicies[] = { 0,1,2,3 };
	GLuint indexBufferID;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), 
		indicies, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tri.indexBufferSize(), 
		tri.indicies, GL_STATIC_DRAW);
	*/
	

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
	
	random_device rd;	
	mt19937 eng(rd());	
	uniform_real_distribution<> distr(-0.02f, 0.02f);

	randVelocity[0] = distr(eng);
	randVelocity[1] = distr(eng);
}



void Draw(int shapeNum)
{		
	//glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, 0);
	if(shapeNum==1)
		//glDrawArrays(GL_QUADS, 0, 4);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	//if(shapeNum==2)
		//glDrawArrays(GL_LINE_LOOP, 4, 8);
}

void MeGLWindow::paintGL()
{
	glViewport(0, 0, width(), height());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
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
		glUniform3f(startLocation, posLeft[0], posLeft[1], 0);
		glUniform3f(colors, 0.0f, 1.0f, 0.0f);		
	}
	if (triNum == 2)
	{
		posRight[0] += randVelocity[0];
		posRight[1] += randVelocity[1];
		glUniform3f(startLocation, posRight[0], posRight[1], 0);
		glUniform3f(colors, 0.0f, 0.0f, 1.0f);
		//edgeDetection(4, verts, posRight);		
	}	

}



