#include "main_window.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Qt Vision Viewer");

    QCommandLineParser parser;
    parser.setApplicationDescription("Sanitized edge vision Qt viewer example");
    parser.addHelpOption();
    const QCommandLineOption detectionsOption(
        "detections",
        "Path to a sanitized detections JSON file.",
        "path");
    const QCommandLineOption runtimeConfigOption(
        "runtime-config",
        "Path to a sanitized runtime profile JSON file.",
        "path");
    parser.addOption(detectionsOption);
    parser.addOption(runtimeConfigOption);
    parser.process(app);

    MainWindow window(parser.value(detectionsOption), parser.value(runtimeConfigOption));
    window.show();
    return app.exec();
}
