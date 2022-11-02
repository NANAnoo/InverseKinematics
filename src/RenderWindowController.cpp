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
    // TODO: draw bezier curve;
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
    // TODO: initial editing model
    editing_model = current_model->snapShotModelAt(window->play_bar->getCurrentFrame() - 1);

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
    // TODO: delete current bezier curve

    if (last_motion_offset < 0)
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
void RenderWindowController::receivedMotionData(BVHJointEidtor::EditedValueType type, int x, int y, int z, int w)
{
//    DestinationType,
//    BeginControlType,
//    EndControlType
    // udpate bezier curve
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
