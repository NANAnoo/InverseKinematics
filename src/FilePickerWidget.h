#ifndef FilePickerWidget_H
#define FilePickerWidget_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <string>

// file picker at top, show file path, select open file and save file
class FilePickerWidget : public QWidget
{
    Q_OBJECT
public:
    enum FilePikerMode {NONE_SELECTION_MODE, OPEN_MODE, SAVE_MODE, ERROR_FILE_MODE};
    FilePickerWidget(QWidget *parent);
    void loadView();
    void changeMode(FilePikerMode mode);

private:
    FilePikerMode currentMode;
    QPushButton *selectButton;
    QPushButton *saveButton;
    QLineEdit *pathViewer;
    std::string current_path;

signals:
    void openedFileChanged(const std::string file_path);
    void postSavedFile(const std::string file_path);
    
private slots:
    // file picker bindings
    void openFilePicker();
    void saveFilePicker();

};

#endif // FilePickerWidget_H