#include "RenderWindowController.h"

#include <QTimer>

void RenderWindowController::setup()
{
    // set up connections
    // open & save file
    QObject::connect(window->file_picker, &FilePickerWidget::openedFileChanged,
                     this, &RenderWindowController::showNewModelWithFile);
    QObject::connect(window->file_picker, &FilePickerWidget::postSavedFile,
                     this, &RenderWindowController::saveCurrentModeToFile);

    // play control
    QObject::connect(window->play_bar, &PlayBarWidget::postCurrentFrame,
                     this, &RenderWindowController::renderFrameAt);
    QObject::connect(window->play_bar, &PlayBarWidget::frameStopAt,
                     this, &RenderWindowController::stopFrameAt);
}

// From File Picker
void RenderWindowController::showNewModelWithFile(const std::string file_path)
{
    BVHModel *model = new BVHModel(file_path);
    if (model->isValid())
    {
        // create success
        current_model = model;
        window->file_picker->changeMode(FilePickerWidget::OPEN_MODE);
        window->play_bar->setFrameCount(current_model->allFrameCount());
        window->play_bar->setFrameDuation(current_model->frame_duration);
        window->play_bar->setValid(true);
    }
    else
    {
        // open failed;
        window->file_picker->changeMode(FilePickerWidget::ERROR_FILE_MODE);
        window->play_bar->setValid(false);
    }
    // update play_bar
    window->play_bar->loadView();
}
void RenderWindowController::saveCurrentModeToFile(const std::string file_path)
{
    if (current_model != nullptr && current_model->isValid())
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