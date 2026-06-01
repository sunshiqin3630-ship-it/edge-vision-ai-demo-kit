#pragma once

#include "display_router.h"
#include "vision_canvas.h"

#include <QMainWindow>
#include <QString>
#include <QVector>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class FrameSource;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QString detectionsPath, QString runtimeConfigPath, QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void startSelectedSource();
    void stopSource();
    void applyDisplayMode();
    void updateSourcePlaceholder();

private:
    void buildUi();
    void loadStartupConfig(const QString& runtimeConfigPath);
    void connectSource(FrameSource* source);
    QVector<Detection> loadDetections(const QString& path) const;

    QString detectionsPath_;
    QComboBox* sourceType_ = nullptr;
    QLineEdit* sourceValue_ = nullptr;
    QCheckBox* dualScreen_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    VisionCanvas* primaryCanvas_ = nullptr;
    VisionCanvas* secondaryCanvas_ = nullptr;
    QMainWindow* secondaryWindow_ = nullptr;
    FrameSource* source_ = nullptr;
    DisplayRouter displayRouter_;
};
