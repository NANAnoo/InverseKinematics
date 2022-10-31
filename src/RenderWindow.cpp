#include "RenderWindow.h"
#include "BVHModel.h"
#include <iostream>

RenderWindow::RenderWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // TEST: test file;
    bvh_render = new BVHRenderWidget(this);
    file_picker = new FilePickerWidget(this);
    play_bar = new PlayBarWidget(this);

    loadContent();
}

RenderWindow::~RenderWindow()
{

}


void RenderWindow::loadContent()
{
    bvh_render->update();
    file_picker->loadView();
}
