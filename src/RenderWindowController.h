#ifndef RENDERWINDOWCONTROLLER_H
#define RENDERWINDOWCONTROLLER_H

#include <QObject>
#include "RenderWindow.h"
#include "BVHModel.h"

class RenderWindowController : public QObject
{
    Q_OBJECT

    enum CurrentMode {
        NormalMode,
        JointEditingMode,
        MotionEditingMode,
        MotionPreviewMode
    };

private:
    RenderWindow *window;
    BVHModel *current_model;
    BVHModel *editing_model;
    bool isEditing;
    void setup();
    std::string previous_selected_joint;
public:
    RenderWindowController(RenderWindow *win):window(win), current_model(nullptr), editing_model(nullptr), isEditing(false) {setup();}
    
signals:

public slots:
    // From File Picker
    void showNewModelWithFile(const std::string file_path);
    void saveCurrentModelToFile(const std::string file_path);

    // From Play Bar
    void renderFrameAt(unsigned int frame_ID);
    void stopFrameAt(unsigned int frame_ID);

    // From joint viwer
    void selectedNode(std::string node_name);
    void lockedNode(std::string node_name, bool locked);

    // From motion creator
    void startMotion(double interval, double fps);
    void cancelMotion();

    // From joint editor
    void receivedMotionData(BVHJointEidtor::EditedValueType type, int x, int y, int z, int w);
    void initialMotion();
    void rollbackMotion();
    void insertMotion();
    double getMotionValueAtIndex(unsigned int index);
};

#endif // RENDERWINDOWCONTROLLER_H
