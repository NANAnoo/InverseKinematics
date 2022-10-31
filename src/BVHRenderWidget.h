#ifndef BVHRENDERWIDGET_H
#define BVHRENDERWIDGET_H

#include <QOpenGLWidget>
#include "BVHModelMacros.h"

class BVHRenderWidget : public QOpenGLWidget
{
public:
    BVHRenderWidget(QWidget *parent) : QOpenGLWidget(parent) { setGeometry(300, 100, 600, 600); }

    void repaintWithRenderCallback(std::function<void(void)> callback) {storedRenderCallback = callback; update();}

    BVH::boneRenderHandler getBoneRender();
    BVH::jointRenderHandler getJointRender();

    // dealloc
    ~BVHRenderWidget();

protected:
    std::function<void(void)> storedRenderCallback;
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

#endif // BVHRENDERWIDGET_H
