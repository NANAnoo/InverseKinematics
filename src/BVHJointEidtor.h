#ifndef BVHJOINTEIDTER_H
#define BVHJOINTEIDTER_H

#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include "BVHModelMacros.h"
#include "Eigen/Core"

class BVHJointEidtor : public QWidget
{
    // use Bezier Curve to control the motion
    // joint position (control direction 1), destination position (control point 2)
    Q_OBJECT

public:
    enum EditedValueType {
        JointTranslationType,
        JointRotationType,
        DestinationType,
        BeginControlType,
        EndControlType
    };
    BVHJointEidtor(QWidget *parent = nullptr);

    void loadView();

    void setIsAddingMotion(bool value) {is_adding_motion = value; loadView();}

private:
    enum SliderType {
        XSlider,
        YSlider,
        ZSlider,
        WSlider
    };
    struct MotionControlInfo
    {
        int desitination[4];
        int begin[4];
        int end[4];
    };

    QLabel *jointDetial;
    // controllers
    QSlider *x_silder;
    QLabel *x_silder_label;
    QSlider *y_silder;
    QLabel *y_silder_label;
    QSlider *z_silder;
    QLabel *z_silder_label;
    QSlider *w_silder;
    QLabel *w_silder_label;
    // translation  / rotation;
    QButtonGroup *silder_mode_group;
    // motion control
    QButtonGroup *motion_control_group;
    // preview motion
    QPushButton *preview_btn;
    // accept motion
    QPushButton *accept_btn;

    // motion information
    BVH::BVHMotionInfo *motionInfo;

    // displayed joint
    BVH::BVHJoint *joint;

    // control info
    MotionControlInfo motion_info;

    //  flags
    EditedValueType type;
    bool is_loading_view;
    // disable editing
    bool is_previewing;
    // disable slider
    bool is_adding_motion;
    void sliderdataChanged(SliderType type, int value);
    void controlTypeChanged(bool is_rotation);
    void motionControlTypeChanged(EditedValueType type);
    int getValueFromSlider(QSlider *slider, int value);
    void setSliderText(SliderType type, double value);

signals:
    void editDataChanged(EditedValueType type, int x, int y, int z, int w);
    // preview clicked
    void motionCommited();
    // stop preview
    void motionRollback();
    // accept current motion
    void motionAccepted();
    // delegate method:
    double getMotionValueAtIndex(unsigned int index);
};

#endif // BVHJOINTEIDTER_H