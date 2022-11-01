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
                     this, &RenderWindowController::stopFrameAt);

    // joint viewer
    QObject::connect(window->joint_viewer, &BVHJointViewer::selectedNodeChanged,
                     this, &RenderWindowController::selectedNode);
    QObject::connect(window->joint_viewer, &BVHJointViewer::lockedNodeChanged,
                     this, &RenderWindowController::lockedNode);
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
    }
    else
    {
        // open failed;
        current_model = nullptr;
        window->file_picker->changeMode(FilePickerWidget::ERROR_FILE_MODE);
        window->play_bar->setValid(false);
        window->joint_viewer->updateViewWith({});
    }
    // update play_bar
    window->play_bar->loadView();
}
void RenderWindowController::saveCurrentModelToFile(const std::string file_path)
{
    BVHModel *model = isEditing ? editing_model : current_model;
    if (model != nullptr && model->isValid())
    {
        if (current_model->writeToFile(file_path))
        {
            // TODO: do someting here
        }
    }
}

// From Play Bar
void RenderWindowController::renderFrameAt(unsigned int frame_ID)
{
    // render models and lines
    BVHModel *model = isEditing ? editing_model : current_model;
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
}

void RenderWindowController::stopFrameAt(unsigned int frame_ID)
{
    // render models and lines
    BVHModel *model = isEditing ? editing_model : current_model;
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
}

// From joint viwer
void RenderWindowController::selectedNode(std::string node_name)
{
    BVHModel *model = isEditing ? editing_model : current_model;
    BVH::BVHJoint *joint = model->getJointFromName(node_name);
    // update color
    if (joint != nullptr) {
        BVH::BVHJoint *previous_joint = model->getJointFromName(previous_selected_joint);
        if (previous_joint != nullptr) {
            previous_joint->render_type = static_cast<BVH::RenderType>(previous_joint->render_type - BVH::SelectedType);
        }
        joint->render_type = static_cast<BVH::RenderType>(joint->render_type | BVH::SelectedType);
        previous_selected_joint = node_name;
    }
    window->play_bar->reloadCurrentFrameIfStopped();
}

void RenderWindowController::lockedNode(std::string node_name, bool locked)
{
    BVHModel *model = isEditing ? editing_model : current_model;
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

}
void RenderWindowController::cancelMotion()
{

}

// From joint editor
void RenderWindowController::receivedMotionData(BVHJointEidtor::EditedValueType type, int x, int y, int z, int w)
{

}
void RenderWindowController::initialMotion()
{

}
void RenderWindowController::rollbackMotion()
{

}
void insertMotion()
{

}
double getMotionValueAtIndex(unsigned int index)
{

}
