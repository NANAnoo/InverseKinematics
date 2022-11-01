#include "RenderWindow.h"
#include "BVHModel.h"
#include <iostream>

RenderWindow::RenderWindow(QWidget *parent)
    : QMainWindow(parent)
{

}

RenderWindow::~RenderWindow()
{

}


void RenderWindow::loadContent()
{
    // initialize
    bvh_render = new BVHRenderWidget(this);
    file_picker = new FilePickerWidget(this);
    play_bar = new PlayBarWidget(this);
    joint_viewer = new BVHJointViewer(this);
    joint_editor = new BVHJointEidtor(this);
    motion_creator = new BVHMotionCreater(this);
}
