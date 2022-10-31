#ifndef PlayBarWidget_H
#define PlayBarWidget_H

#include <QTimer>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QProgressBar>

class PlayBarWidget : public QWidget
{
    Q_OBJECT
private:
    /* inner widgets */
    QPushButton *play_preview;
    QPushButton *play_next;
    QPushButton *play_stop_begin;
    QPushButton *play_reset;
    // TODO impl a editable bar
    QProgressBar *bar;
    QLabel *progress_label;

    /* frame information */
    unsigned int max_frame;
    unsigned int current_frame;
    double frame_duration;

    /* playing status*/
    bool is_playing;
    bool is_valid;

    /* play timer*/
    QTimer *timer;

    // update frame info
    void updateFrame(unsigned int frame_ID);

public:
    // initial
    PlayBarWidget(QWidget * parent);
    // max frame
    void setFrameCount(unsigned int count);
    // auto play duration
    void setFrameDuation(double duation);
    // update view
    void loadView();
    // set validate
    void setValid(bool valid){ is_valid = valid; }

signals:
    // current play duation
    void postCurrentFrame(unsigned int frame_ID);
    // play stopped at id
    void frameStopAt(unsigned int frame_ID);

private slots:
    // event handlers
    void startStopPlay();
    void resetPlay();
    void nextFrame();
    void previousFrame();
};

#endif // PlayBarWidget_H
