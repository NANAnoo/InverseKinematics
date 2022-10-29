#ifndef RENDERWINDOWCONTROLLER_H
#define RENDERWINDOWCONTROLLER_H

#include <QObject>
#include "RenderWindow.h"

class RenderWindowController : public QObject
{
    Q_OBJECT
private:
    RenderWindow *window;

public:
    explicit RenderWindowController(RenderWindow *renderWindow):window(renderWindow){}

    void loopAnimation();

signals:

public slots:
};

#endif // RENDERWINDOWCONTROLLER_H
