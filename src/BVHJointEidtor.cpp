#include "BVHJointEidtor.h"

#include <QRadioButton>
#include <QMessageBox>

BVHJointEidtor::BVHJointEidtor(QWidget *parent) : QWidget(parent)
{
    // initial all properties
    joint = nullptr;
    // init labels
    for (QLabel **label : {&joint_detial, &x_silder_label, &y_silder_label, &z_silder_label, &w_silder_label, &control_silder_label}) {
        *label = new QLabel(this);
        (*label)->setAlignment(Qt::AlignCenter);
    }

    // init silders
    for (QSlider **slider : {&x_silder, &y_silder, &z_silder, &w_silder}) {
        *slider = new QSlider(this);
        (*slider)->setMaximum(200);
        (*slider)->setMinimum(1);
        (*slider)->setTickInterval(1);
        (*slider)->setSliderPosition(0);
    }
    control_silder = new QSlider(this);
    control_silder->setMaximum(100);
    control_silder->setMinimum(0);
    control_silder->setTickInterval(1);
    control_silder->setSliderPosition(0);
    control_silder->setOrientation(Qt::Horizontal);

    // translation  / rotation;
    silder_mode_group = new QButtonGroup(this);
    QRadioButton *trasilation_btn = new QRadioButton(this);
    QRadioButton *rotation_btn = new QRadioButton(this);
    trasilation_btn->setChecked(true);
    trasilation_btn->setText("trasilation");
    rotation_btn->setText("rotation");
    silder_mode_group->addButton(trasilation_btn, 1);
    silder_mode_group->addButton(rotation_btn, 2);
    silder_mode_group->setExclusive(true);

    // motion control
    motion_control_group = new QButtonGroup(this);
    QRadioButton *desitination_btn = new QRadioButton(this);
    QRadioButton *begin_control_btn = new QRadioButton(this);
    QRadioButton *end_control_btn = new QRadioButton(this);
    desitination_btn->setChecked(true);
    desitination_btn->setText("target");
    begin_control_btn->setText("begin");
    end_control_btn->setText("end");
    motion_control_group->addButton(desitination_btn, 1);
    motion_control_group->addButton(begin_control_btn, 2);
    motion_control_group->addButton(end_control_btn, 3);
    motion_control_group->setExclusive(true);

    // preview motion
    preview_btn = new QPushButton(this);
    preview_btn->setText("preview");
    // accept motion
    accept_btn = new QPushButton(this);
    accept_btn->setText("accept");

    //  flags
    type = JointTranslationType;
    is_adding_motion = false;
    is_previewing = false;
    is_loading_view = false;
    for (int i = 0; i < 4; i ++) {
        // set to (0, 0, 0, 1)
        motion_info.desitination[i] = i == 3 ? 1 : 0;
        motion_info.begin[i] = i == 3 ? 1 : 0;
        motion_info.end[i] = i == 3 ? 1 : 0;
    }

    // layout
    int width = parent->width();
    int height = parent->height();
    this->setGeometry(width / 4 * 3 + 10, 0 , width / 4 - 20, height / 10 * 9);
    joint_detial->setGeometry(5, 5, this->width() - 10, this->height() / 9 - 10);
    // left side
    width = this->width() / 3 * 2;
    height = this->height() / 9 * 8;
    x_silder->setGeometry(5, height / 4 + 5, width / 4 - 10, height / 4 * 3 - 5);
    y_silder->setGeometry(width / 4 + 5, height / 4 + 5, width / 4 - 10, height / 4 * 3 - 5);
    z_silder->setGeometry(width / 2 + 5, height / 4 + 5, width / 4 - 10, height / 4 * 3 - 5);
    w_silder->setGeometry(width / 4 * 3 + 5, height / 4 + 5, width / 4 - 10, height / 4 * 3 - 5);
    control_silder->setGeometry(width / 8, height / 32 * 5 , width / 4 * 3 - 10, height / 16);
    x_silder_label->setGeometry(0, height, width / 4, height / 8);
    y_silder_label->setGeometry(width / 4, height, width / 4, height / 8);
    z_silder_label->setGeometry(width / 2, height, width / 4, height / 8);
    w_silder_label->setGeometry(width / 4 * 3, height, width / 4, height / 8);
    control_silder_label->setGeometry(width / 4 * 3 + 5, height / 8 + 5, width / 4 * 3 - 10, height / 10);
    control_silder_label->setText("Weight:");
    // right side
    width = this->width() / 3;
    int x = width * 2 + 10;
    int height_from = height / 8;
    trasilation_btn->setGeometry(x, height_from + height / 16 * 2, width - 20, height / 16);
    rotation_btn->setGeometry(x, height_from + height / 16 * 3, width - 20, height / 16);
    desitination_btn->setGeometry(x, height_from + height / 16 * 6, width - 20, height / 16);
    begin_control_btn->setGeometry(x, height_from + height / 16 * 8, width - 20, height / 16);
    end_control_btn->setGeometry(x, height_from + height / 16 * 10, width - 20, height / 16);
    preview_btn->setGeometry(x, height_from + height / 16 * 13, width - 20, height / 16);
    accept_btn->setGeometry(x, height_from + height / 16 * 15, width - 20, height / 16);

    // set bindings
    x_silder->connect(x_silder, &QSlider::valueChanged, [=](int value){
        if (!is_loading_view)
            this->sliderdataChanged(XSlider, getValueFromSlider(x_silder, value));
    });
    y_silder->connect(y_silder, &QSlider::valueChanged, [=](int value){
        if (!is_loading_view)
            this->sliderdataChanged(YSlider, getValueFromSlider(y_silder, value));
    });
    z_silder->connect(z_silder, &QSlider::valueChanged, [=](int value){
        if (!is_loading_view)
            this->sliderdataChanged(ZSlider, getValueFromSlider(z_silder, value));
    });
    w_silder->connect(w_silder, &QSlider::valueChanged, [=](int value){
        if (!is_loading_view)
            this->sliderdataChanged(WSlider, getValueFromSlider(w_silder, value));
    });
    control_silder->connect(control_silder, &QSlider::valueChanged, [=](int value) {
        if (joint != nullptr) {
            jointWeightChanged(joint->name, static_cast<double>(value) / 100);
            setSliderText(CSlider, static_cast<double>(value) / 100);
        }
    });
    trasilation_btn->connect(trasilation_btn, &QRadioButton::toggled, [=](bool){
        this->type = JointTranslationType;
        controlTypeChanged(false);
    });
    rotation_btn->connect(rotation_btn, &QRadioButton::toggled, [=](bool){
        this->type = JointRotationType;
        controlTypeChanged(true);
    });
    desitination_btn->connect(desitination_btn, &QRadioButton::toggled, [=](bool){
        motionControlTypeChanged(DestinationType);
    });
    begin_control_btn->connect(begin_control_btn, &QRadioButton::toggled, [=](bool){
        motionControlTypeChanged(BeginControlType);
    });
    end_control_btn->connect(end_control_btn, &QRadioButton::toggled, [=](bool){
        motionControlTypeChanged(EndControlType);
    });
    preview_btn->connect(preview_btn, &QPushButton::released, [=](){
        // start or stoppreview
        is_previewing = !is_previewing;
        if (is_previewing) {
            if (!emit previewStarted()) {
                // fail
                is_previewing = false;
                QMessageBox::critical(this, "Motion Information", "Motion generation failed");
            }
        } else {
            emit previewEnded();
         }
        loadView();
    });
    accept_btn->connect(accept_btn, &QPushButton::released, [=](){
        is_previewing = false;
        loadView();
        emit motionAccepted();
    });
    loadView();
}

void BVHJointEidtor::loadView()
{
    is_loading_view = true;
    if (joint == nullptr) {
        this->joint_detial->setText("Unknown Joint");
    } else {
        // setup information, name $(is_locked) $(is_site)
        this->joint_detial->setText(joint->name.c_str());
    }
    double x = 0, y = 0, z = 0, w = 1;
    switch (type) {
        case JointTranslationType:
            if (joint != nullptr)
            {
                x =  joint->offset[0];
                y = joint->offset[1];
                z = joint->offset[2];
            }
            break;
        case JointRotationType:
            if (joint != nullptr)
            {
                for (unsigned int i = 0; i < joint->channels.size(); i++)
                {
                    double value = emit getMotionValueAtIndex(joint->channel_index_offset + i);
                    if (joint->channels[i] == BVH::X_ROTATION) {
                        x = value;
                    } else if (joint->channels[i] == BVH::Y_ROTATION) {
                        y = value;
                    } else if (joint->channels[i] == BVH::Z_ROTATION) {
                        z = value;
                    }
                }
            }
            break;
        case DestinationType:
            x = motion_info.desitination[0];
            y = motion_info.desitination[1];
            z = motion_info.desitination[2];
            w = motion_info.desitination[3];
            break;
        case BeginControlType:
            x = motion_info.begin[0];
            y = motion_info.begin[1];
            z = motion_info.begin[2];
            w = motion_info.begin[3];
            break;
        case EndControlType:
            x = motion_info.end[0];
            y = motion_info.end[1];
            z = motion_info.end[2];
            w = motion_info.end[3];
            break;
    }
    double max_XYZ = std::max(std::abs(x), std::max(std::abs(y), std::abs(z)));
    if (max_XYZ > 100) {
        // normalize
        w = max_XYZ / 100;
        x /= w;
        y /= w;
        z /= w;
    }
    // set position of silders
    x_silder->setSliderPosition(static_cast<int>(x + 100));
    y_silder->setSliderPosition(static_cast<int>(y + 100));
    z_silder->setSliderPosition(static_cast<int>(z + 100));
    w_silder->setSliderPosition(static_cast<int>(w + 100));
    control_silder->setSliderPosition(joint == nullptr ? 0 : static_cast<int>(joint->weight * 100));
    setSliderText(CSlider, joint == nullptr ? 0 : joint->weight);

    // set up labels
    setSliderText(XSlider, x);
    setSliderText(YSlider, y);
    setSliderText(ZSlider, z);
    setSliderText(WSlider, w);

    // set enable in different status
    preview_btn->setText(is_previewing ? "stop" : "preview");
    // enable slider only if while motion editing
    x_silder->setEnabled(!is_previewing && is_adding_motion);
    y_silder->setEnabled(!is_previewing && is_adding_motion);
    z_silder->setEnabled(!is_previewing && is_adding_motion);
    w_silder->setEnabled(!is_previewing && is_adding_motion);
    control_silder->setEnabled(!is_previewing && joint != nullptr && !joint->is_end);

    for(QAbstractButton* button : silder_mode_group->buttons()){
        button->setEnabled(!is_previewing && !is_adding_motion);
        if (joint && button->text() == "rotation" && joint->is_end) {
            button->setEnabled(false);
        }
    }
    for(QAbstractButton* button : motion_control_group->buttons()){
        button->setEnabled(!is_previewing && is_adding_motion);
    }
    preview_btn->setEnabled(is_adding_motion);
    accept_btn->setEnabled(is_previewing && is_adding_motion);

    is_loading_view = false;
}


void BVHJointEidtor::setSliderText(SliderType type, double value)
{
    char buffer[20];
    switch(type) {
        case XSlider:
            std::snprintf(buffer, 20, "X \n %.2f", value);
            x_silder_label->setText(buffer);
            break;
        case YSlider:
            std::snprintf(buffer, 20, "Y \n %.2f", value);
            y_silder_label->setText(buffer);
            break;
        case ZSlider:
            std::snprintf(buffer, 20, "Z \n %.2f", value);
            z_silder_label->setText(buffer);
            break;
        case WSlider:
            std::snprintf(buffer, 20, "W \n %.2f", value);
            w_silder_label->setText(buffer);
            break;
        case CSlider:
            std::snprintf(buffer, 20, "Weight: %.2f", value);
            control_silder_label->setText(buffer);
            break;
    };
}

#include <iostream>
void BVHJointEidtor::sliderdataChanged(SliderType type, int value)
{
    // change moption info
    int index = static_cast<int>(type);
    if (index < 0 || index > 3) return;
    int *arr = nullptr;
    value = value - 100;
    if (this->type == DestinationType) {
        motion_info.desitination[index] = value;
        arr = motion_info.desitination;
    } else if (this->type == BeginControlType) {
        motion_info.begin[index] = value;
        arr = motion_info.begin;
    } else if (this->type == EndControlType) {
        motion_info.end[index] = value;
        arr = motion_info.end;
    }
    loadView();
    std::cout << "Data changed at " << this->type
              << " x:"<< arr[0]
              << " y:"<< arr[1]
              << " z:"<< arr[2]
              << " w:"<< arr[3] << std::endl;
    emit editDataChanged(this->type, arr[0], arr[1], arr[2], arr[3]);

}

void BVHJointEidtor::controlTypeChanged(bool is_rotation)
{
    if (is_rotation) {
        this->type = JointRotationType;
    } else {
        this->type = JointTranslationType;
    }
    loadView();
}

void BVHJointEidtor::motionControlTypeChanged(EditedValueType type)
{
    this->type = type;
    loadView();
}

// utils
int BVHJointEidtor::getValueFromSlider(QSlider *slider, int value)
{
    return value - slider->minimum();
}

