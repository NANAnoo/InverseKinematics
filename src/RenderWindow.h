#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

#include "BVHRenderWidget.h"
#include "FilePickerWidget.h"
#include "PlayBarWidget.h"
#include "BVHJointViewer.h"

class RenderWindow : public QMainWindow
{
    Q_OBJECT

public:
    RenderWindow(QWidget *parent = nullptr);
    ~RenderWindow();

    // set layout
    void loadContent();

    // custome widgets
    BVHRenderWidget *bvh_render;
    FilePickerWidget *file_picker;
    PlayBarWidget *play_bar;
    BVHJointViewer *joint_viewer;
};

#endif // MAINWINDOW_H
