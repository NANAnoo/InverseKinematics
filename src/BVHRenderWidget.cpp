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
    glOrtho(-10, 10, -10, 10, -10, 10);
}

void BVHRenderWidget::paintGL()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // set model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw model
    if (model != nullptr) {
        model->RenderFigure(currentTick, 0.3);
    }
}

void BVHRenderWidget::nextFrame()
{
    update();
    currentTick  = (currentTick + 1) %  totalTick;
}

