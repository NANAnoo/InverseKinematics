#ifndef BVHRENDERWIDGET_H
#define BVHRENDERWIDGET_H

#include <QOpenGLWidget>
#include "BVHModelMacros.h"

class BVHRenderWidget : public QOpenGLWidget
{
public:
    BVHRenderWidget(QWidget *parent) : QOpenGLWidget(parent){
        QRect size = parent->geometry();
        setGeometry(size.width() / 4, size.height() / 8, size.width() / 2, size.width() / 2);
    }

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
