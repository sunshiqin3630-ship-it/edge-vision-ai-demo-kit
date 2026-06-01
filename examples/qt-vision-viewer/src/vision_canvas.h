#pragma once

#include "frame_source.h"

#include <QRectF>
#include <QString>
#include <QVector>
#include <QWidget>

struct Detection {
    QString label;
    double score = 0.0;
    QRectF bbox;
};

class VisionCanvas final : public QWidget {
    Q_OBJECT

public:
    explicit VisionCanvas(QString title, QWidget* parent = nullptr);

    void setDetections(QVector<Detection> detections);

public slots:
    void setFrame(const VideoFrame& frame);
    void setStatus(const QString& status);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QRect drawFrame(QPainter& painter);
    void drawDetections(QPainter& painter, const QRect& frameRect);

    QString title_;
    QString status_ = "Waiting for source";
    QImage frame_;
    QString sourceName_ = "none";
    qint64 sequence_ = 0;
    QVector<Detection> detections_;
};
