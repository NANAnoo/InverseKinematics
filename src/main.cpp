#include "RenderWindow.h"
#include "RenderWindowController.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RenderWindow w;
    RenderWindowController controller(&w);
    w.setFixedSize(1200, 800);
    w.show();
    controller.loopAnimation();

    return a.exec();
}
