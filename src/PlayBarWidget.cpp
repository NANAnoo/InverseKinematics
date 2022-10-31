#include "PlayBarWidget.h"

#include <cstring>

PlayBarWidget::PlayBarWidget(QWidget *parent)
    : QWidget(parent)
{
    // set layout
    setGeometry(300, 710, 600, 80);

    /* inner widgets initial*/
    play_preview = new QPushButton("⏪", this);
    play_next = new QPushButton("⏩", this);
    play_stop_begin = new QPushButton("▶️", this);
    play_reset = new QPushButton("⏹", this);
    bar = new QProgressBar(this);
    progress_label = new QLabel(this);

    // layout
    bar->setGeometry(5, 5, 590, 30);
    play_preview->setGeometry(5, 40, 40, 40);
    play_stop_begin->setGeometry(50, 40, 40, 40);
    play_next->setGeometry(500, 40, 40, 40);
    play_reset->setGeometry(550, 40, 40, 40);
    progress_label->setGeometry(270, 40, 60 ,40);

    // event bindings
    QObject::connect(play_preview, &QPushButton::released, this, &PlayBarWidget::previousFrame);
    QObject::connect(play_next, &QPushButton::released, this, &PlayBarWidget::nextFrame);
    QObject::connect(play_stop_begin, &QPushButton::released, this, &PlayBarWidget::startStopPlay);
    QObject::connect(play_reset, &QPushButton::released, this, &PlayBarWidget::resetPlay);

    // bar settings
    bar->setOrientation(Qt::Orientation::Horizontal);
    bar->setMinimum(1);
    bar->setMaximum(1);
    bar->setValue(1);
    bar->setTextVisible(false);

    // label settings
    progress_label->setAlignment(Qt::AlignCenter);
    progress_label->setText("1 : 1");

    /* frame information */
    max_frame = 0;
    current_frame = 0;
    frame_duration = 0;

    /* playing status*/
    is_playing = false;
    is_valid = false;

    /* play timer*/
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [&]()
                     {
        if (is_playing) {
            updateFrame(current_frame % max_frame + 1);
        } });

    // start layout
    loadView();
}

void PlayBarWidget::loadView()
{
    is_playing = false;
    play_stop_begin->setText("▶️");
    if (is_valid)
    {
        play_preview->setEnabled(true);
        play_next->setEnabled(true);
        play_stop_begin->setEnabled(true);
        play_reset->setEnabled(true);
        bar->setMaximum(max_frame);
        bar->setMinimum(1);
        // playing is stopped at first
        updateFrame(1);
    }
    else
    {
        play_preview->setEnabled(false);
        play_next->setEnabled(false);
        play_stop_begin->setEnabled(false);
        play_reset->setEnabled(false);
        bar->setMaximum(1);
        bar->setValue(1);
    }
}

void PlayBarWidget::setFrameDuation(double duration)
{
    if (timer != nullptr)
    {
        timer->stop();
    }
    frame_duration = duration;
}

void PlayBarWidget::setFrameCount(unsigned int count)
{
    max_frame = count;
}

void PlayBarWidget::updateFrame(unsigned int frame_ID)
{
    current_frame = frame_ID;
    if (is_playing)
    {
        emit postCurrentFrame(current_frame);
    }
    else
    {
        emit frameStopAt(current_frame);
    }
    char progress[20];
    std::snprintf(progress, 30, "%d : %d", current_frame, max_frame);
    progress_label->setText(progress);
}

// slots
void PlayBarWidget::startStopPlay()
{
    if (is_playing)
    {
        // stop
        play_stop_begin->setText("▶️");
        if (timer != nullptr)
        {
            timer->stop();
        }
        emit frameStopAt(current_frame);
    }
    else
    {
        // start
        play_stop_begin->setText("⏸");
        if (timer != nullptr)
        {
            timer->start(frame_duration);
        }
        emit postCurrentFrame(current_frame);
    }
    is_playing = !is_playing;
}

void PlayBarWidget::resetPlay()
{
    if (is_playing)
    {
        // stop first
        play_stop_begin->setText("▶️");
        if (timer != nullptr)
        {
            timer->stop();
        }
        is_playing = false;
    }
    updateFrame(1);
}
void PlayBarWidget::nextFrame()
{
    if (current_frame < max_frame)
    {
        // enable speed up
        updateFrame(current_frame + 1);
    }
    // do nothing
}
void PlayBarWidget::previousFrame()
{
    if (current_frame > 1)
    {
        // enable play back
        updateFrame(current_frame - 1);
    }
    // do nothing
}