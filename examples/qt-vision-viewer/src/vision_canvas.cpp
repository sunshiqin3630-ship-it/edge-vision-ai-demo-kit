#include "vision_canvas.h"

#include <QColor>
#include <QFontMetrics>
#include <QPainter>
#include <QtGlobal>

#include <utility>

VisionCanvas::VisionCanvas(QString title, QWidget* parent)
    : QWidget(parent), title_(std::move(title))
{
    setMinimumSize(640, 360);
}

void VisionCanvas::setDetections(QVector<Detection> detections)
{
    detections_ = std::move(detections);
    update();
}

void VisionCanvas::setFrame(const VideoFrame& frame)
{
    frame_ = frame.image;
    sourceName_ = frame.sourceName;
    sequence_ = frame.sequence;
    update();
}

void VisionCanvas::setStatus(const QString& status)
{
    status_ = status;
    update();
}

void VisionCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(26, 28, 30));

    const QRect frameRect = drawFrame(painter);
    drawDetections(painter, frameRect);

    painter.setPen(QColor(232, 236, 238));
    painter.drawText(rect().adjusted(16, 12, -16, -12), Qt::AlignTop | Qt::AlignLeft,
                     QString("%1 | source=%2 | frame=%3").arg(title_, sourceName_).arg(sequence_));
    painter.setPen(QColor(185, 194, 198));
    painter.drawText(rect().adjusted(16, 34, -16, -12), Qt::AlignTop | Qt::AlignLeft, status_);
}

QRect VisionCanvas::drawFrame(QPainter& painter)
{
    const QRect target = contentsRect().adjusted(18, 58, -18, -18);
    if (frame_.isNull()) {
        painter.fillRect(target, QColor(40, 45, 48));
        painter.setPen(QPen(QColor(100, 112, 118, 90), 1));
        for (int x = target.left(); x < target.right(); x += 80) {
            painter.drawLine(x, target.top(), x, target.bottom());
        }
        for (int y = target.top(); y < target.bottom(); y += 60) {
            painter.drawLine(target.left(), y, target.right(), y);
        }
        painter.setPen(QColor(210, 218, 222));
        painter.drawText(target, Qt::AlignCenter, "No frame");
        return target;
    }

    const QSize scaledSize = frame_.size().scaled(target.size(), Qt::KeepAspectRatio);
    const QRect frameRect(
        target.left() + (target.width() - scaledSize.width()) / 2,
        target.top() + (target.height() - scaledSize.height()) / 2,
        scaledSize.width(),
        scaledSize.height());
    painter.fillRect(target, QColor(18, 20, 22));
    painter.drawImage(frameRect, frame_);
    return frameRect;
}

void VisionCanvas::drawDetections(QPainter& painter, const QRect& frameRect)
{
    const QPen boxPen(QColor(255, 214, 102), 3);
    const QColor labelBackground(20, 24, 26, 220);

    for (const Detection& detection : detections_) {
        QRectF box(
            frameRect.left() + detection.bbox.x() * frameRect.width(),
            frameRect.top() + detection.bbox.y() * frameRect.height(),
            detection.bbox.width() * frameRect.width(),
            detection.bbox.height() * frameRect.height());

        painter.setPen(boxPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(box);

        const QString label = QString("%1 %2%")
                                  .arg(detection.label)
                                  .arg(qRound(detection.score * 100.0));
        const int labelWidth = qMax(120, QFontMetrics(painter.font()).horizontalAdvance(label) + 18);
        const QRectF labelRect(box.left(), qMax(frameRect.top(), static_cast<int>(box.top()) - 28),
                               labelWidth, 24);
        painter.fillRect(labelRect, labelBackground);
        painter.setPen(QColor(248, 248, 246));
        painter.drawText(labelRect.adjusted(8, 0, -8, 0), Qt::AlignVCenter | Qt::AlignLeft, label);
    }
}
