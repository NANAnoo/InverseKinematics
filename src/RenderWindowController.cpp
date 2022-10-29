#include "RenderWindowController.h"

#include <QTimer>

void RenderWindowController::loopAnimation()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        window->resetLayout();
    });
    timer->start(8);
    // window->resetLayout();
}
