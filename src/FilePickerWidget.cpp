#include "FilePickerWidget.h"
#include <QFileDialog>


FilePickerWidget::FilePickerWidget(QWidget *parent)
    :QWidget(parent)
{
    int width = parent->width();
    int height = parent->height();
    setGeometry(width / 4, height / 32 , width / 2 , height / 16);
    width = this->width();
    height = this->height();
    selectButton = new QPushButton("...", this);
    saveButton = new QPushButton("save", this);
    pathViewer = new QLineEdit("select a file ...", this);

    // disable editing
    pathViewer->setFocusPolicy(Qt::NoFocus);

    // set layout
    pathViewer->setGeometry(height / 10, height / 10, width / 6 * 5, height / 5 * 4);
    selectButton->setGeometry(width / 6 * 5 + height / 2 , height / 5, height / 5 * 2, height / 5 * 3);
    saveButton->setGeometry(width / 6 * 5 + height , height / 5, height, height / 5 * 3);
    currentMode = NONE_SELECTION_MODE;
    
    // set bindings:
    QObject::connect(selectButton, &QPushButton::released, this, &FilePickerWidget::openFilePicker);
    QObject::connect(saveButton, &QPushButton::released, this, &FilePickerWidget::saveFilePicker);

    // update
    loadView();
}

void FilePickerWidget::loadView()
{
    if (currentMode == NONE_SELECTION_MODE) {
        // need select file
        pathViewer->setAlignment(Qt::AlignLeft);
        saveButton->setEnabled(false);
        selectButton->setEnabled(true);
        pathViewer->setText(" Select a file ...");
    } else if (currentMode == OPEN_MODE) {
        // display the file path
        pathViewer->setAlignment(Qt::AlignRight);
        saveButton->setEnabled(false);
        selectButton->setEnabled(true);
        pathViewer->setText(current_path.c_str());
    } else if (currentMode == SAVE_MODE) {
        // need a path to save
        pathViewer->setAlignment(Qt::AlignLeft);
        saveButton->setEnabled(true);
        selectButton->setEnabled(false);
        pathViewer->setText(" Select a file to save");
    } else if (currentMode == ERROR_FILE_MODE) {
        // require a new path
        pathViewer->setAlignment(Qt::AlignLeft);
        saveButton->setEnabled(false);
        selectButton->setEnabled(true);
        pathViewer->setText(" Wrong file, select again ...");
        current_path.clear();
    }
}

void FilePickerWidget::changeMode(FilePikerMode mode)
{
    currentMode = mode;
    loadView();
}

// signal handlers
#include <iostream>
void FilePickerWidget::openFilePicker()
{
    // open a new file picker to open file
    QFileDialog opener(this);
    opener.setFileMode(QFileDialog::ExistingFile);
    opener.setNameFilter(tr("BVH files (*.bvh)"));
    opener.setViewMode(QFileDialog::Detail);
    opener.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList filePaths;
    if (opener.exec()) {
        filePaths = opener.selectedFiles();
        if (filePaths.size() > 0) {
            // post file path to out
            current_path.assign(filePaths[0].toStdString());
            emit openedFileChanged(filePaths[0].toStdString());
            std::cout << "Select open file: " << filePaths[0].toStdString() << std::endl;
        }
    }
}

void FilePickerWidget::saveFilePicker()
{
    // open a new file picker to save file
    QFileDialog opener(this);
    opener.setFileMode(QFileDialog::AnyFile);
    opener.setNameFilter(tr("BVH files (*.bvh)"));
    opener.setViewMode(QFileDialog::List);
    opener.setAcceptMode(QFileDialog::AcceptSave);

    QStringList filePaths;
    if (opener.exec()) {
        filePaths = opener.selectedFiles();
        if (filePaths.size() > 0) {
            // post file path out
            emit postSavedFile(filePaths[0].toStdString());
            std::cout << "Select saved file: " << filePaths[0].toStdString() << std::endl;
        }
    }
}
