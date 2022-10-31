#include "PlayBarWidget.h"

#include <cstring>

PlayBarWidget::PlayBarWidget(QWidget *parent)
    : QWidget(parent)
{
    // set layout
    int width = parent->geometry().width();
    int height = parent->geometry().height();
    setGeometry(width / 12 * 3, height / 8 * 7 + height / 80, width / 2, height / 10);
    width = geometry().width();
    height = geometry().height() / 2;

    /* inner widgets initial*/
    play_preview = new QPushButton("<<", this);
    play_next = new QPushButton(">>", this);
    play_stop_begin = new QPushButton("[>", this);
    play_reset = new QPushButton("[o]", this);
    bar = new QProgressBar(this);
    progress_label = new QLabel(this);

    // layout
    bar->setGeometry(height / 10, height / 10, width - height / 5, height / 5 * 3);
    play_preview->setGeometry(height / 5, height - height / 10, height / 5 * 4, height / 5 * 4);
    play_stop_begin->setGeometry(height / 5 * 6, height - height / 10, height / 5 * 4, height / 5 * 4);
    play_next->setGeometry(width / 6 * 5 + height / 5, height - height / 10, height / 5 * 4, height / 5 * 4);
    play_reset->setGeometry(width / 6 * 5 + height / 5 * 6, height - height / 10, height / 5 * 4, height / 5 * 4);
    progress_label->setGeometry(width / 2 - height / 5 * 6, height - height / 10, height / 5 * 12 , height / 5 * 4);

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
    play_stop_begin->setText("[>");
    bar->setMinimum(1);
    bar->setValue(1);
    if (is_valid)
    {
        play_preview->setEnabled(true);
        play_next->setEnabled(true);
        play_stop_begin->setEnabled(true);
        play_reset->setEnabled(true);
        bar->setMaximum(static_cast<int>(max_frame));
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
    bar->setValue(static_cast<int>(current_frame));
}

// slots
void PlayBarWidget::startStopPlay()
{
    if (is_playing)
    {
        // stop
        play_stop_begin->setText("[>");
        if (timer != nullptr)
        {
            timer->stop();
        }
        emit frameStopAt(current_frame);
    }
    else
    {
        // start
        play_stop_begin->setText("||");
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
        play_stop_begin->setText("[>");
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
