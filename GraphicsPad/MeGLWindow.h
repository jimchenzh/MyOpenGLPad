#pragma once
#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>

class MeGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void keyPressEvent(QKeyEvent * e);
	void update(int triNum);

public:
	MeGLWindow();
	~MeGLWindow();
};


#endif
