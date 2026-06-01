#include "main_window.h"

#include "ffmpeg_stream_source.h"
#include "frame_source.h"
#include "synthetic_frame_source.h"
#include "v4l2_camera_source.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>
#include <QPushButton>
#include <QtGlobal>
#include <QVBoxLayout>

#include <utility>

MainWindow::MainWindow(QString detectionsPath, QString runtimeConfigPath, QWidget* parent)
    : QMainWindow(parent), detectionsPath_(std::move(detectionsPath))
{
    qRegisterMetaType<VideoFrame>("VideoFrame");
    buildUi();
    const QVector<Detection> detections = loadDetections(detectionsPath_);
    primaryCanvas_->setDetections(detections);
    secondaryCanvas_->setDetections(detections);
    loadStartupConfig(runtimeConfigPath);
    applyDisplayMode();
}

MainWindow::~MainWindow()
{
    stopSource();
}

void MainWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    auto* controls = new QHBoxLayout();

    sourceType_ = new QComboBox(central);
    sourceType_->addItem("Synthetic", "synthetic");
    sourceType_->addItem("FFmpeg stream", "ffmpeg");
    sourceType_->addItem("V4L2 UVC", "v4l2");

    sourceValue_ = new QLineEdit(central);
    auto* startButton = new QPushButton("Start", central);
    auto* stopButton = new QPushButton("Stop", central);
    dualScreen_ = new QCheckBox("Dual-screen", central);
    statusLabel_ = new QLabel("Ready", central);

    controls->addWidget(new QLabel("Source", central));
    controls->addWidget(sourceType_);
    controls->addWidget(sourceValue_, 1);
    controls->addWidget(startButton);
    controls->addWidget(stopButton);
    controls->addWidget(dualScreen_);

    primaryCanvas_ = new VisionCanvas("Operator UI", central);
    root->addLayout(controls);
    root->addWidget(primaryCanvas_, 1);
    root->addWidget(statusLabel_);
    setCentralWidget(central);

    secondaryWindow_ = new QMainWindow(this);
    secondaryWindow_->setWindowTitle("Vision Output");
    secondaryCanvas_ = new VisionCanvas("Vision Output", secondaryWindow_);
    secondaryWindow_->setCentralWidget(secondaryCanvas_);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSelectedSource);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSource);
    connect(dualScreen_, &QCheckBox::toggled, this, &MainWindow::applyDisplayMode);
    connect(sourceType_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateSourcePlaceholder);

    updateSourcePlaceholder();
    resize(1120, 720);
    setWindowTitle("Edge Vision AI Demo Kit - Qt Viewer");
}

void MainWindow::loadStartupConfig(const QString& runtimeConfigPath)
{
    if (runtimeConfigPath.isEmpty()) {
        return;
    }

    QFile file(runtimeConfigPath);
    if (!file.open(QIODevice::ReadOnly)) {
        statusLabel_->setText("Runtime config not found; using UI defaults");
        return;
    }

    const QJsonObject root = QJsonDocument::fromJson(file.readAll()).object();
    const QString active = root.value("activeSourceProfile").toString("synthetic-demo");
    const QJsonArray profiles = root.value("sourceProfiles").toArray();
    for (const QJsonValue& value : profiles) {
        const QJsonObject profile = value.toObject();
        if (profile.value("id").toString() != active) {
            continue;
        }
        const QString type = profile.value("type").toString("synthetic");
        const int index = sourceType_->findData(type);
        if (index >= 0) {
            sourceType_->setCurrentIndex(index);
        }
        sourceValue_->setText(profile.value(type == "v4l2" ? "device" : "url").toString());
    }

    const QString displayMode = root.value("display").toObject().value("mode").toString();
    dualScreen_->setChecked(displayMode == "dual-screen");
}

void MainWindow::startSelectedSource()
{
    stopSource();

    const QString type = sourceType_->currentData().toString();
    const QString value = sourceValue_->text().trimmed();
    if (type == "ffmpeg") {
        connectSource(new FfmpegStreamSource(value, this));
    } else if (type == "v4l2") {
        connectSource(new V4l2CameraSource(value.isEmpty() ? "/dev/video0" : value, QSize(1280, 720), this));
    } else {
        connectSource(new SyntheticFrameSource(QSize(1280, 720), this));
    }
    source_->start();
}

void MainWindow::stopSource()
{
    if (!source_) {
        return;
    }
    source_->stop();
    source_->deleteLater();
    source_ = nullptr;
}

void MainWindow::applyDisplayMode()
{
    const auto mode = dualScreen_->isChecked()
        ? DisplayRouter::DisplayMode::DualScreen
        : DisplayRouter::DisplayMode::SingleWindow;
    displayRouter_.apply(this, secondaryWindow_, mode);
}

void MainWindow::updateSourcePlaceholder()
{
    const QString type = sourceType_->currentData().toString();
    if (type == "ffmpeg") {
        sourceValue_->setPlaceholderText("rtsp://example-camera.local/live");
    } else if (type == "v4l2") {
        sourceValue_->setPlaceholderText("/dev/video0");
    } else {
        sourceValue_->setPlaceholderText("sample://synthetic-frame");
    }
}

void MainWindow::connectSource(FrameSource* source)
{
    source_ = source;
    connect(source_, &FrameSource::frameReady, primaryCanvas_, &VisionCanvas::setFrame);
    connect(source_, &FrameSource::frameReady, secondaryCanvas_, &VisionCanvas::setFrame);
    connect(source_, &FrameSource::statusChanged, primaryCanvas_, &VisionCanvas::setStatus);
    connect(source_, &FrameSource::statusChanged, secondaryCanvas_, &VisionCanvas::setStatus);
    connect(source_, &FrameSource::statusChanged, statusLabel_, &QLabel::setText);
}

QVector<Detection> MainWindow::loadDetections(const QString& path) const
{
    QVector<Detection> fallback {
        {"person", 0.92, QRectF(0.14, 0.18, 0.22, 0.50)},
        {"package", 0.81, QRectF(0.55, 0.42, 0.24, 0.26)},
    };

    if (path.isEmpty()) {
        return fallback;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return fallback;
    }

    QVector<Detection> detections;
    const QJsonArray values = QJsonDocument::fromJson(file.readAll()).object().value("detections").toArray();
    for (const QJsonValue& value : values) {
        const QJsonObject object = value.toObject();
        const QJsonArray bbox = object.value("bbox").toArray();
        if (bbox.size() != 4) {
            continue;
        }
        detections.push_back({
            object.value("label").toString("object"),
            object.value("score").toDouble(0.0),
            QRectF(bbox.at(0).toDouble(), bbox.at(1).toDouble(), bbox.at(2).toDouble(), bbox.at(3).toDouble()),
        });
    }
    return detections.isEmpty() ? fallback : detections;
}
