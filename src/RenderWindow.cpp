#include "RenderWindow.h"
#include "BVHModel.h"
#include <iostream>

RenderWindow::RenderWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // TEST: test file;
    BVHModel *model = new BVHModel("/Users/nanorth/Develop/AssignmentsInLeedsHPGS1/AnimationAndSimulation/InverseKinematics/models/05_06.bvh");
    model->writeToFile("/Users/nanorth/Develop/AssignmentsInLeedsHPGS1/AnimationAndSimulation/InverseKinematics/output/test.bvh");
    // custom widgets init
    bvh_render = new BVHRenderWidget(this, model);
    bvh_render->resize(600, 600);

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
