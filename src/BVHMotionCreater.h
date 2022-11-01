#ifndef BVHMOTIONCREATER_H
#define BVHMOTIONCREATER_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

// create a motion from current frame

class BVHMotionCreater : public QWidget
{
    Q_OBJECT
private:
    QLabel *interval_label;
    QLabel *fps_label;
    QLineEdit *interval_editor;
    QLineEdit *fps_editor;
    QPushButton *motion_create_cancel_btn;
    void updateInterval(double interval);
    void updateFPS(double fps);
    bool is_editing;
public:
    BVHMotionCreater(QWidget *parent);
    void loadView();

signals:
    void motionStart(double interval, double fps);
    void motionCancel();
};

#endif // BVHMOTIONCREATER_H
