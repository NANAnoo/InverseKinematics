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

    // draw model
    if (model != nullptr) {
        glColor3f(1, 1, 1);
        glPushMatrix();
        glTranslatef(5, 0, 0);
        model->RenderFigure(currentTick, 0.3f);
        glPopMatrix();
    }
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
    if (bvh_model != nullptr) {
        BVHModel::boneRenderHandler boneRender = [&](Eigen::Vector4f start, Eigen::Vector4f end, unsigned int color, float radius){
            glLineWidth(3);
            glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
            glBegin(GL_LINES);
            glVertex3f(start.x(), start.y(), start.z());
            glVertex3f(end.x(), end.y(), end.z());
            glEnd();
        };
        BVHModel::jointRenderHandler jointRender = [&](Eigen::Vector4f centor, unsigned int color, float radius){
            glPointSize(10);
            glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
            glBegin(GL_POINTS);
            glVertex3f(centor.x(), centor.y(), centor.z());
            glEnd();

        };
        bvh_model->renderModelWith(boneRender, jointRender, currentTick, 0.3f);
    }
}

void BVHRenderWidget::nextFrame()
{
    update();
    currentTick  = (currentTick + 1) %  totalTick;
}

