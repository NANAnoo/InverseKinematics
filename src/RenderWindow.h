#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

#include "BVHRenderWidget.h"
#include "FilePickerWidget.h"
#include "PlayBarWidget.h"

class RenderWindow : public QMainWindow
{
    Q_OBJECT

public:
    RenderWindow(QWidget *parent = 0);
    ~RenderWindow();

    // set layout
    void loadContent();

    // custome widgets
    BVHRenderWidget *bvh_render;
    FilePickerWidget *file_picker;
    PlayBarWidget *play_bar;
};

#endif // MAINWINDOW_H
