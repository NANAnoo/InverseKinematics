#ifndef RENDERWINDOWCONTROLLER_H
#define RENDERWINDOWCONTROLLER_H

#include <QObject>
#include "RenderWindow.h"
#include "BVHModel.h"
#include "BezierCurve.h"

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
    BezierCurve *curve;
    bool is_editing;
    void setup();
    std::string previous_selected_joint;
    unsigned int motion_offset_start;
    unsigned int motion_size;
    int last_motion_offset;
public:
    RenderWindowController(RenderWindow *win):window(win),
        current_model(nullptr),
        editing_model(nullptr),
        curve(new BezierCurve(2)),
        is_editing(false),
        motion_offset_start(0),
        motion_size(0),
        last_motion_offset(-1)
        {setup();}
    
signals:

public slots:
    // From File Picker
    void showNewModelWithFile(const std::string file_path);
    void saveCurrentModelToFile(const std::string file_path);

    // From Play Bar
    void renderFrameAt(unsigned int frame_ID);

    // From joint viwer
    void selectedNode(std::string node_name);
    void lockedNode(std::string node_name, bool locked);

    // From motion creator
    void startMotion(double interval, double fps);
    void cancelMotion();

    // From joint editor
    void changeJointWeight(std::string joint_name, double value);
    void receivedMotionData(BVHJointEidtor::EditedValueType type, int x, int y, int z, int w);
    bool startPreviewMotion();
    void stopPreviewMotion();
    void insertMotion();
    double getMotionValueAtIndex(unsigned int index);
};

#endif // RENDERWINDOWCONTROLLER_H
