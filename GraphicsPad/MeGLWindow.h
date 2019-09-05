#pragma once
#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>

class MeGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();

public:
	MeGLWindow();
	~MeGLWindow();
};


#endif
