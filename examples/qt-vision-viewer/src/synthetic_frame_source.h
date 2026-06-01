#pragma once

#include "frame_source.h"

#include <QSize>
#include <QTimer>

class SyntheticFrameSource final : public FrameSource {
    Q_OBJECT

public:
    explicit SyntheticFrameSource(QSize frameSize, QObject* parent = nullptr);

    QString name() const override;
    void start() override;
    void stop() override;

private slots:
    void produceFrame();

private:
    QSize frameSize_;
    QTimer timer_;
    qint64 sequence_ = 0;
};
