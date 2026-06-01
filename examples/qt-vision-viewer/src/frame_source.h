#pragma once

#include <QImage>
#include <QMetaType>
#include <QObject>
#include <QString>

struct VideoFrame {
    QImage image;
    QString sourceName;
    qint64 sequence = 0;
};

Q_DECLARE_METATYPE(VideoFrame)

class FrameSource : public QObject {
    Q_OBJECT

public:
    explicit FrameSource(QObject* parent = nullptr) : QObject(parent) {}
    ~FrameSource() override = default;

    virtual QString name() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void frameReady(const VideoFrame& frame);
    void statusChanged(const QString& status);
};
