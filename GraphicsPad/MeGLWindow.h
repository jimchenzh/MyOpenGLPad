#pragma once
#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>

class MeGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent * e);
	void wheelEvent(QWheelEvent * w);
	void update();
	

public:
	MeGLWindow();
	~MeGLWindow();
};


#endif
