#include "BVHJointViewer.h"

#include <map>
#include <QCheckBox>
#include <iostream>

BVHJointViewer::BVHJointViewer(QWidget *parent) : QWidget(parent)
{
    treeViwer = new QTreeWidget(parent);
    // layout
    int width = parent->geometry().width();
    int height = parent->geometry().height();
    setGeometry(5, 5, width / 4 - 10, height - 10);
    treeViwer->setGeometry(this->geometry());
    width = this->geometry().width();
    // set up viewer
    treeViwer->setColumnCount(2);
    treeViwer->setHeaderLabels(QStringList()<<"Name"<<"Lock");
    treeViwer->setItemsExpandable(true);
    treeViwer->setColumnWidth(0, width / 5 * 4);
    treeViwer->setColumnWidth(1, width / 5 );
    QObject::connect(treeViwer, &QTreeWidget::itemClicked, this, &BVHJointViewer::handleSelectEvent);
}

void BVHJointViewer::updateViewWith(std::vector<BVH::BVHMetaNode> nodes)
{
    // node map
    treeViwer->clear();
    std::map<std::string, QTreeWidgetItem *> items;
    for (BVH::BVHMetaNode node : nodes) {
        std::string name = node.name;
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList{QString::fromStdString(name), ""});
        // set map and level relationships
        items[name] = item;
        if (node.parent.size() == 0) {
            // set root item
            treeViwer->addTopLevelItem(item);
        } else {
            items[node.parent]->addChild(item);
        }
        // set button
        QCheckBox *lock = new QCheckBox();
        lock->connect(lock, &QCheckBox::toggled, [=](bool checked){
            handleLockEvent(checked, name);
        });
        treeViwer->setItemWidget(item, 1, lock);
    }
    treeViwer->expandAll();
}

void BVHJointViewer::handleSelectEvent(QTreeWidgetItem *item, int column)
{
    if (column == 0) {
        selectedNodeChanged(item->text(0).toStdString());
        std::cout << " Joint: " << item->text(0).toStdString() << " selected" << std::endl;
    }
}

void BVHJointViewer::handleLockEvent(bool checked, std::string name)
{
    emit lockedNodeChanged(name, checked);
    std::cout << " Joint: " << name << (checked ? " locked" : " locked") << std::endl;
}
