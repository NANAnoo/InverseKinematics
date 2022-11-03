#include "RenderWindowController.h"

#include <QTimer>

void RenderWindowController::setup()
{
    // set up connections
    // open & save file
    QObject::connect(window->file_picker, &FilePickerWidget::openedFileChanged,
                     this, &RenderWindowController::showNewModelWithFile);
    QObject::connect(window->file_picker, &FilePickerWidget::postSavedFile,
                     this, &RenderWindowController::saveCurrentModelToFile);

    // play control
    QObject::connect(window->play_bar, &PlayBarWidget::postCurrentFrame,
                     this, &RenderWindowController::renderFrameAt);
    QObject::connect(window->play_bar, &PlayBarWidget::frameStopAt,
                     this, &RenderWindowController::renderFrameAt);

    // joint viewer
    QObject::connect(window->joint_viewer, &BVHJointViewer::selectedNodeChanged,
                     this, &RenderWindowController::selectedNode);
    QObject::connect(window->joint_viewer, &BVHJointViewer::lockedNodeChanged,
                     this, &RenderWindowController::lockedNode);

    // motion creator
    QObject::connect(window->motion_creator, &BVHMotionCreater::motionStart,
                     this, &RenderWindowController::startMotion);
    QObject::connect(window->motion_creator, &BVHMotionCreater::motionCancel,
                     this, &RenderWindowController::cancelMotion);

    // joint editor
    QObject::connect(window->joint_editor, &BVHJointEidtor::getMotionValueAtIndex,
                     this, &RenderWindowController::getMotionValueAtIndex);
    QObject::connect(window->joint_editor, &BVHJointEidtor::editDataChanged,
                     this, &RenderWindowController::receivedMotionData);
    QObject::connect(window->joint_editor, &BVHJointEidtor::previewStarted,
                     this, &RenderWindowController::startPreviewMotion);
    QObject::connect(window->joint_editor, &BVHJointEidtor::previewEnded,
                     this, &RenderWindowController::stopPreviewMotion);
    QObject::connect(window->joint_editor, &BVHJointEidtor::motionAccepted,
                     this, &RenderWindowController::insertMotion);
}

// From File Picker
void RenderWindowController::showNewModelWithFile(const std::string file_path)
{
    BVHModel *model = new BVHModel(file_path);
    if (model->isValid())
    {
        // create success
        current_model = model;
        // set up widget
        window->file_picker->changeMode(FilePickerWidget::OPEN_MODE);
        window->play_bar->setFrameCount(current_model->allFrameCount());
        window->play_bar->setFrameDuation(current_model->frame_duration);
        window->play_bar->setValid(true);
        window->joint_viewer->updateViewWith(current_model->getMetaList());
        window->motion_creator->setValid(true);
    }
    else
    {
        // open failed;
        current_model = nullptr;
        window->file_picker->changeMode(FilePickerWidget::ERROR_FILE_MODE);
        window->play_bar->setValid(false);
        window->joint_viewer->updateViewWith({});
        window->motion_creator->setValid(false);
    }
}
void RenderWindowController::saveCurrentModelToFile(const std::string file_path)
{
    BVHModel *model = is_editing ? editing_model : current_model;
    if (model != nullptr && model->isValid())
    {
        if (current_model->writeToFile(file_path))
        {
            // TODO: do someting here
            window->file_picker->changeMode(FilePickerWidget::OPEN_MODE);
        }
    }
}

// From Play Bar
void RenderWindowController::renderFrameAt(unsigned int frame_ID)
{
    // render models and lines
    BVHModel *model = is_editing ? editing_model : current_model;
    // update curve info
    if (curve->getEnabled() && window->joint_viewer->getSelectedJointName().size() > 0) {
        Eigen::Vector4f position = model->jointPositionAt(frame_ID - 1, window->joint_viewer->getSelectedJointName(), 0.3);
        curve->setStart(position.x(), position.y(), position.z());
    }
    window->bvh_render->repaintWithRenderCallback(
        [=]()
        {
            // start render
            if (model != nullptr && model->isValid())
            {
                model->renderModelWith(window->bvh_render->getBoneRender(),
                                       window->bvh_render->getJointRender(),
                                       frame_ID - 1,
                                       0.3);
            }
             // raw bezier curve
            if (curve->getEnabled()) {
                glLineWidth(1);
                glColor3f(1, 0.5, 0);
                glBegin(GL_LINE_STRIP);
                // glVertex3f(centor.x(), centor.y(), centor.z());
                curve->forEach([](Eigen::Vector3d point, unsigned int) {
                    glVertex3f(point.x(), point.y(), point.z());
                });
                glEnd();
                // render desitination
                Eigen::Vector3d last = curve->getLast();
                Eigen::Vector4f desitination(last.x(), last.y(), last.z(), 0);
                window->bvh_render->getJointRender()(desitination, BVH::DesitinationType, 0.3);
            }
        });
    if (!is_editing) {
        // reload to update information
        window->joint_editor->loadView();
    }
}

// From joint viwer
void RenderWindowController::selectedNode(std::string node_name)
{
    BVHModel *model = is_editing ? editing_model : current_model;
    BVH::BVHJoint *joint = model->getJointFromName(node_name);
    // update color
    if (joint != nullptr) {
        BVH::BVHJoint *previous_joint = model->getJointFromName(previous_selected_joint);
        if (previous_joint != nullptr) {
            previous_joint->render_type = static_cast<BVH::RenderType>(previous_joint->render_type - BVH::SelectedType);
        }
        joint->render_type = static_cast<BVH::RenderType>(joint->render_type | BVH::SelectedType);
        previous_selected_joint = node_name;
        window->joint_editor->setJoint(joint);
        if (curve->getEnabled()) {
            Eigen::Vector4f position = model->jointPositionAt(window->play_bar->getCurrentFrame() - 1, node_name, 0.3);
            curve->setStart(position.x(), position.y(), position.z());
        }
    }
    window->play_bar->reloadCurrentFrameIfStopped();
}

void RenderWindowController::lockedNode(std::string node_name, bool locked)
{
    BVHModel *model = is_editing ? editing_model : current_model;
    BVH::BVHJoint *joint = model->getJointFromName(node_name);
    // update color
    if (joint != nullptr) {
        if (locked) {
            joint->render_type = static_cast<BVH::RenderType>(joint->render_type | BVH::LockedType);
        } else {
            joint->render_type = static_cast<BVH::RenderType>(joint->render_type - BVH::LockedType);
        }
    }
    window->play_bar->reloadCurrentFrameIfStopped();
}

// From motion creator
void RenderWindowController::startMotion(double interval, double fps)
{
    // prepare for new motion
    is_editing = true;
    // initial editing model
    editing_model = current_model->snapShotModelAt(window->play_bar->getCurrentFrame() - 1);
    // update curve
    curve->setPieceSize(static_cast<unsigned int>(interval * fps));
    curve->setEnabled(true);
    // update widgets
    window->file_picker->changeMode(FilePickerWidget::SAVE_MODE);
    window->play_bar->setFrameDuation(1.0 / fps);
    window->play_bar->setFrameCount(static_cast<unsigned int>(interval * fps));
    window->play_bar->setValid(false);
    window->joint_editor->setIsAddingMotion(true);
}

void RenderWindowController::cancelMotion()
{
    // TODO: delete motion data due to recorded range
    // disable current bezier curve
    curve->setEnabled(false);

    // update widgets status
    if (last_motion_offset == 0)
        // no motion added, changed to open mode
        window->file_picker->changeMode(FilePickerWidget::OPEN_MODE);
    is_editing = last_motion_offset > 0;
    BVHModel *model = is_editing ? editing_model : current_model;
    window->joint_editor->setIsAddingMotion(false);
    window->play_bar->setFrameCount(model->allFrameCount());
    window->play_bar->setFrameDuation(model->frame_duration);
    window->play_bar->setValid(true);
}

// From joint editor
static time_t s_last_receive_time = 0;
void RenderWindowController::receivedMotionData(BVHJointEidtor::EditedValueType type, int x, int y, int z, int w)
{
    // udpate bezier curve
    if (!curve->getEnabled())
        return;
    double r_x = static_cast<double>(x) * w / 10.0;
    double r_y = static_cast<double>(y) * w / 10.0;
    double r_z = static_cast<double>(z) * w / 10.0;
    if (type == BVHJointEidtor::DestinationType) {
        curve->setEnd(r_x, r_y, r_z);
    } else if (type == BVHJointEidtor::BeginControlType) {
        curve->setStartControl(r_x, r_y, r_z);
    } else if (type == BVHJointEidtor::EndControlType) {
        curve->setEndControl(r_x, r_y, r_z);
    }
    // re paint
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tpMicro
           = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    time_t cur_time = tpMicro.time_since_epoch().count();
    if (cur_time - s_last_receive_time > 50) {
        window->play_bar->reloadCurrentFrameIfStopped();
        s_last_receive_time = cur_time;
    }
}
void RenderWindowController::startPreviewMotion()
{
    // TODO: calculate motion data from bezier curve with IK

    // TODO: insert into editing model, record the range that need be inserted

    // TODO: enable play bar, auto play current motion
}
void RenderWindowController::stopPreviewMotion()
{
    // stop play motion, return to motion edit
}
void RenderWindowController::insertMotion()
{
    // update last_motion_offset
    last_motion_offset = editing_model->allFrameCount();
    // insert added motions into current_model
    // prepare for next motion
    // update motion_creator status
}
double RenderWindowController::getMotionValueAtIndex(unsigned int index)
{
    unsigned int current_frame = window->play_bar->getCurrentFrame();
    BVHModel *model = is_editing ? editing_model : current_model;
    return model->getMotionValue(current_frame, index);
}
