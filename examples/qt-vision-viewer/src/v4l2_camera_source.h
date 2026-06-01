#pragma once

#include "frame_source.h"

#include <QSize>
#include <QString>

#include <atomic>
#include <thread>

class V4l2CameraSource final : public FrameSource {
    Q_OBJECT

public:
    explicit V4l2CameraSource(QString devicePath, QSize frameSize, QObject* parent = nullptr);
    ~V4l2CameraSource() override;

    QString name() const override;
    void start() override;
    void stop() override;

private:
    void captureLoop();

    QString devicePath_;
    QSize frameSize_;
    std::atomic_bool running_{false};
    std::thread worker_;
};
