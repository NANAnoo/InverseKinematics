#ifndef RENDERWINDOWCONTROLLER_H
#define RENDERWINDOWCONTROLLER_H

#include <QObject>
#include "RenderWindow.h"
#include "BVHModel.h"

class RenderWindowController : public QObject
{
    Q_OBJECT
private:
    RenderWindow *window;
    BVHModel *current_model;
    BVHModel *editing_model;
    bool isEditing;
    void setup();
public:
    RenderWindowController(RenderWindow *win):window(win), current_model(nullptr), editing_model(nullptr), isEditing(false) {setup();}
    
signals:

public slots:
    // From File Picker
    void showNewModelWithFile(const std::string file_path);
    void saveCurrentModeToFile(const std::string file_path);

    // From Play Bar
    void renderFrameAt(unsigned int frame_ID);
    void stopFrameAt(unsigned int frame_ID);
};

#endif // RENDERWINDOWCONTROLLER_H
