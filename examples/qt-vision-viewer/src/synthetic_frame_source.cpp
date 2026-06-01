#include "synthetic_frame_source.h"

#include <QColor>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>

SyntheticFrameSource::SyntheticFrameSource(QSize frameSize, QObject* parent)
    : FrameSource(parent), frameSize_(frameSize)
{
    timer_.setInterval(66);
    connect(&timer_, &QTimer::timeout, this, &SyntheticFrameSource::produceFrame);
}

QString SyntheticFrameSource::name() const
{
    return "synthetic";
}

void SyntheticFrameSource::start()
{
    sequence_ = 0;
    timer_.start();
    emit statusChanged("Synthetic source running");
}

void SyntheticFrameSource::stop()
{
    timer_.stop();
    emit statusChanged("Synthetic source stopped");
}

void SyntheticFrameSource::produceFrame()
{
    QImage image(frameSize_, QImage::Format_RGB888);
    QPainter painter(&image);

    QLinearGradient gradient(image.rect().topLeft(), image.rect().bottomRight());
    gradient.setColorAt(0.0, QColor(54, 68, 74));
    gradient.setColorAt(1.0, QColor(20, 24, 26));
    painter.fillRect(image.rect(), gradient);

    painter.setPen(QPen(QColor(120, 132, 138, 90), 1));
    for (int x = 0; x < image.width(); x += 80) {
        painter.drawLine(x, 0, x, image.height());
    }
    for (int y = 0; y < image.height(); y += 60) {
        painter.drawLine(0, y, image.width(), y);
    }

    const int sweep = (image.width() * (sequence_ % 120)) / 120;
    painter.fillRect(QRect(sweep, 0, 8, image.height()), QColor(68, 196, 160, 130));
    painter.setPen(QColor(225, 230, 232));
    painter.drawText(image.rect().adjusted(18, 18, -18, -18), Qt::AlignTop | Qt::AlignLeft,
                     "Synthetic frame source");

    emit frameReady(VideoFrame{image, name(), sequence_++});
}
