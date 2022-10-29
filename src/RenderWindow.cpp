#include "RenderWindow.h"
#include <iostream>
#include "Eigen/Dense"

RenderWindow::RenderWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // TEST: test file;
    // BVH *model = new BVH("C:\\Develop\\InverseKinematics\\models\\rest.bvh");
    BVHModel *model = new BVHModel("C:\\Develop\\InverseKinematics\\models\\05_06.bvh");
    // custom widgets init
    bvh_render = new BVHRenderWidget(this, model);
    bvh_render->resize(1000, 1000);
    bvh_render->model = new BVH("C:\\Develop\\InverseKinematics\\models\\05_06.bvh");

    // layout
    window_layout = new QGridLayout(this);

    // set widgets
    window_layout->addWidget(bvh_render);

    resetLayout();
}

RenderWindow::~RenderWindow()
{

}


void RenderWindow::resetLayout()
{
    bvh_render->nextFrame();
}
