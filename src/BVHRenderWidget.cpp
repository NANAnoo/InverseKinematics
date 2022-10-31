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
    return [&](Eigen::Vector4f start, Eigen::Vector4f end, BVH::RenderType type, float radius)
    {
        unsigned int color = WHITE_COLOR;
        float width = 20 * radius;
        switch (type) {
            case BVH::DefaultType:
                color = CYAN_COLOR;
                width = 10 * radius;
                break;
            case BVH::SelectedType:
                color = GREEN_COLOR;
                break;
            case BVH::LockedType:
                color = YELLOW_COLOR;
                width = 10 * radius;
                break;
            case BVH::LockedType | BVH::SelectedType:
                color = PURPLE_COLOR;
                break;
        }
        glLineWidth(width);
        glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
        glBegin(GL_LINES);
        glVertex3f(start.x(), start.y(), start.z());
        glVertex3f(end.x(), end.y(), end.z());
        glEnd();
    };
}

BVH::jointRenderHandler BVHRenderWidget::getJointRender()
{
    return [&](Eigen::Vector4f centor, BVH::RenderType type, float radius)
    {
        float width = 10 * radius;
        unsigned int color = WHITE_COLOR;
        switch (type) {
            case BVH::DefaultType:
                color = BLUE_COLOR;
                width = 40 * radius;
                break;
            case BVH::SelectedType:
                color = GREEN_COLOR;
                width = 50 * radius;
                break;
            case BVH::LockedType:
                color = YELLOW_COLOR;
                width = 50 * radius;
                break;
            case BVH::LockedType | BVH::SelectedType:
                color = PURPLE_COLOR;
                width = 50 * radius;
                break;
        }
        glPointSize(width);
        glColor3f(FLOAT_RED(color), FLOAT_GREEN(color), FLOAT_BLUE(color));
        glBegin(GL_POINTS);
        glVertex3f(centor.x(), centor.y(), centor.z());
        glEnd();
    };
}
