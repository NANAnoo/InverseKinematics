#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

#include "BVHRenderWidget.h"

class RenderWindow : public QMainWindow
{
    Q_OBJECT

public:
    RenderWindow(QWidget *parent = 0);
    ~RenderWindow();

    // set layout
    void resetLayout();
private:
    // layout
    QGridLayout *window_layout;

    // custome widgets
    BVHRenderWidget *bvh_render;


};

#endif // MAINWINDOW_H
