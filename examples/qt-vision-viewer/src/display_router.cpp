#include "display_router.h"

#include <QGuiApplication>
#include <QPoint>
#include <QScreen>
#include <QWidget>
#include <QWindow>

DisplayRouter::DisplayRouter(QObject* parent) : QObject(parent) {}

QStringList DisplayRouter::screenSummary() const
{
    QStringList summary;
    const QList<QScreen*> screens = QGuiApplication::screens();
    for (int i = 0; i < screens.size(); ++i) {
        const QRect geometry = screens.at(i)->geometry();
        summary << QString("%1: %2 %3x%4+%5+%6")
                       .arg(i)
                       .arg(screens.at(i)->name())
                       .arg(geometry.width())
                       .arg(geometry.height())
                       .arg(geometry.x())
                       .arg(geometry.y());
    }
    return summary;
}

void DisplayRouter::apply(QWidget* primaryWindow, QWidget* secondaryWindow, DisplayMode mode)
{
    const QList<QScreen*> screens = QGuiApplication::screens();
    if (!primaryWindow || !secondaryWindow || screens.isEmpty()) {
        return;
    }

    primaryWindow->showNormal();
    primaryWindow->move(screens.first()->availableGeometry().topLeft() + QPoint(40, 40));

    if (mode != DisplayMode::DualScreen || screens.size() < 2) {
        secondaryWindow->hide();
        return;
    }

    secondaryWindow->createWinId();
    if (secondaryWindow->windowHandle()) {
        secondaryWindow->windowHandle()->setScreen(screens.at(1));
    }
    secondaryWindow->setGeometry(screens.at(1)->geometry());
    secondaryWindow->showFullScreen();
}
