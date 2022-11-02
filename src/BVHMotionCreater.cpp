#include "BVHMotionCreater.h"
#include <QDoubleValidator>
#include <iostream>


BVHMotionCreater::BVHMotionCreater(QWidget *parent) : QWidget(parent)
{
    // init sub widgets
    interval_label = new QLabel(this);
    fps_label = new QLabel(this);
    interval_editor = new QLineEdit(this);
    fps_editor = new QLineEdit(this);
    motion_create_cancel_btn = new QPushButton(this);
    is_editing = false;

    // layout setup
    int width = parent->width();
    int height = parent->height();
    this->setGeometry(0, height * 7 / 8 + 10, width / 4, height / 8 - 20);
    width = this->width();
    height = this->height();

    // set sub widgets layout
    interval_label->setGeometry(5, 5, width / 3 - 10, height / 2 - 10);
    fps_label->setGeometry(5, height / 2 + 5, width / 3 - 10, height / 2 - 10);
    interval_editor->setGeometry(width / 3 + 5, 5, width / 3 - 10, height / 2 - 10);
    fps_editor->setGeometry(width / 3 + 5, height / 2 + 5, width / 3 - 10, height / 2 - 10);
    motion_create_cancel_btn->setGeometry(width / 3 * 2 + 10, height / 5, width / 3 - 20, height / 5 * 3);

    // set editor, input double only
    interval_editor->setValidator(new QDoubleValidator(interval_editor));
    fps_editor->setValidator(new QDoubleValidator(interval_editor));

    // set label text
    interval_label->setText("Motion Interval (s):");
    fps_label->setText("Fps Interval :");
    interval_label->setAlignment(Qt::AlignCenter);
    fps_label->setAlignment(Qt::AlignCenter);

    updateInterval(1);
    updateFPS(60);

    // set up bindings
    motion_create_cancel_btn->connect(motion_create_cancel_btn, &QPushButton::released,
                                      [=](){
        is_editing = !is_editing;
        if (is_editing) {
            double interval = this->interval_editor->text().toDouble();
            double fps = this->fps_editor->text().toDouble();
            // set boundary (0.5s ~ 10s), (30 ~ 120 fps)
            interval = std::min(10.0, std::max(interval, 0.1));
            fps = std::min(120.0, std::max(fps, 1.0));
            updateInterval(interval);
            updateFPS(fps);
            emit motionStart(interval, fps);
        } else {
            emit motionCancel();
        }
        loadView();
    });

    loadView();
    setValid(false);
}

void BVHMotionCreater::loadView()
{
    if (is_editing) {
        motion_create_cancel_btn->setText("Cancel \nMotion");
    } else {
        motion_create_cancel_btn->setText("Create \nMotion");
    }
    // if is editing, disable change
    interval_editor->setReadOnly(is_editing);
    fps_editor->setReadOnly(is_editing);
}


void BVHMotionCreater::updateInterval(double interval)
{
    char str[20];
    std::snprintf(str, 30, "%.2f", interval);
    interval_editor->setText(str);
}

void BVHMotionCreater::updateFPS(double fps)
{
    char str[20];
    std::snprintf(str, 30, "%.2f", fps);
    fps_editor->setText(str);
}
