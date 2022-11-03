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
        isDraging = false;
        last_mouse_position_x = 0;
        last_mouse_position_y = 0;
        rotation_x = 0;
        rotation_y = 0;
    }

    void repaintWithRenderCallback(std::function<void(void)> callback) {storedRenderCallback = callback; update();}

    BVH::boneRenderHandler getBoneRender();
    BVH::jointRenderHandler getJointRender();

    // dealloc
    ~BVHRenderWidget();

protected:
    // mouse event handler
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    // render callback
    std::function<void(void)> storedRenderCallback;
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    bool isDraging;
    int last_mouse_position_x;
    int last_mouse_position_y;
    float rotation_x;
    float rotation_y;
};

#endif // BVHRENDERWIDGET_H
