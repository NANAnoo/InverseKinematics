#include "BVHRenderWidget.h"

BVHRenderWidget::~BVHRenderWidget()
{
}

void BVHRenderWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
}

void BVHRenderWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 20);
}

void BVHRenderWidget::paintGL()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // set model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw axies
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();

    // draw other models
    if (storedRenderCallback != nullptr) {
        storedRenderCallback();
    }
    
}

BVH::boneRenderHandler BVHRenderWidget::getBoneRender()
{
    return [&](Eigen::Vector4f start, Eigen::Vector4f end, unsigned int color, float radius)
    {
        glLineWidth(3);
        glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
        glBegin(GL_LINES);
        glVertex3f(start.x(), start.y(), start.z());
        glVertex3f(end.x(), end.y(), end.z());
        glEnd();
    };
}

BVH::jointRenderHandler BVHRenderWidget::getJointRender()
{
    return [&](Eigen::Vector4f centor, unsigned int color, float radius)
    {
        glPointSize(10);
        glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
        glBegin(GL_POINTS);
        glVertex3f(centor.x(), centor.y(), centor.z());
        glEnd();
    };
}
