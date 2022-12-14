#ifndef BVHJOINTVIEWER_H
#define BVHJOINTVIEWER_H

#include <QWidget>
#include <QTreeWidget>
#include <vector>
#include "BVHModelMacros.h"

// use tree view to show the whole skeleton
class BVHJointViewer : public QWidget
{
    Q_OBJECT
private:
    QTreeWidget *treeViwer;
    std::string selected_joint;

public:
    BVHJointViewer(QWidget *parent);

    void updateViewWith(std::vector<BVH::BVHMetaNode> nodes);
    void handleLockEvent(bool checked, std::string name);

    std::string getSelectedJointName() {return selected_joint;}

    void setValid(bool valid);

signals:
    void selectedNodeChanged(std::string node_name);
    void lockedNodeChanged(std::string node_name, bool locked);

private slots:
    void handleSelectEvent(QTreeWidgetItem *item, int column);
};

#endif // BVHJOINTVIEWER_H
