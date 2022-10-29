#ifndef BVHRENDERWIDGET_H
#define BVHRENDERWIDGET_H

#include <QOpenGLWidget>
#include "BVH.h"


class BVHRenderWidget : public QOpenGLWidget
{
public:
    BVH *model;
    // init with model
    BVHRenderWidget(QWidget *parent, BVH *bvh):QOpenGLWidget(parent), model(bvh), currentTick(0), totalTick(bvh->num_frame){}

    void nextFrame();

    // dealloc
    ~BVHRenderWidget();
protected:
    int currentTick;
    int totalTick;
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

#endif // BVHRENDERWIDGET_H
