#ifndef BVHRENDERWIDGET_H
#define BVHRENDERWIDGET_H

#include <QOpenGLWidget>
#include "BVH.h"
#include "BVHModel.h"


class BVHRenderWidget : public QOpenGLWidget
{
public:
    BVH *model;
    BVHModel *bvh_model;
    // init with model
    BVHRenderWidget(QWidget *parent, BVH *bvh):QOpenGLWidget(parent), model(bvh), currentTick(0), totalTick(bvh->num_frame){bvh_model = nullptr;}
    BVHRenderWidget(QWidget *parent, BVHModel *bvh):QOpenGLWidget(parent), bvh_model(bvh), currentTick(0), totalTick(bvh_model->allFrameCount()){model = nullptr;}

    void nextFrame();

    // dealloc
    ~BVHRenderWidget();
protected:
    unsigned int currentTick;
    unsigned int totalTick;
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

#endif // BVHRENDERWIDGET_H
