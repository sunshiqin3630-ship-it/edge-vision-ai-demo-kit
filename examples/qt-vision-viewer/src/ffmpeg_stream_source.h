#pragma once

#include "frame_source.h"

#include <QString>

#include <atomic>
#include <thread>

class FfmpegStreamSource final : public FrameSource {
    Q_OBJECT

public:
    explicit FfmpegStreamSource(QString url, QObject* parent = nullptr);
    ~FfmpegStreamSource() override;

    QString name() const override;
    void start() override;
    void stop() override;

private:
    void captureLoop();

    QString url_;
    std::atomic_bool running_{false};
    std::thread worker_;
};
