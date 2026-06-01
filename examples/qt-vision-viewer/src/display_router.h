#pragma once

#include <QObject>
#include <QStringList>

class QWidget;

class DisplayRouter final : public QObject {
    Q_OBJECT

public:
    enum class DisplayMode {
        SingleWindow,
        DualScreen,
    };

    explicit DisplayRouter(QObject* parent = nullptr);

    QStringList screenSummary() const;
    void apply(QWidget* primaryWindow, QWidget* secondaryWindow, DisplayMode mode);
};
