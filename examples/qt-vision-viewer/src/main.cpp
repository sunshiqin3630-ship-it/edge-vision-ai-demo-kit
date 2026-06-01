#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLinearGradient>
#include <QMainWindow>
#include <QPainter>
#include <QRectF>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <QVector>
#include <QWidget>

#include <utility>

struct Detection {
    QString label;
    double score;
    QRectF bbox;
};

static QVector<Detection> defaultDetections()
{
    return {
        {"person", 0.92, QRectF(0.14, 0.18, 0.22, 0.50)},
        {"package", 0.81, QRectF(0.55, 0.42, 0.24, 0.26)},
    };
}

static QVector<Detection> loadDetections(const QString& path)
{
    if (path.isEmpty()) {
        return defaultDetections();
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return defaultDetections();
    }

    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray detections = doc.object().value("detections").toArray();
    QVector<Detection> result;
    result.reserve(detections.size());

    for (const QJsonValue& value : detections) {
        const QJsonObject object = value.toObject();
        const QJsonArray bbox = object.value("bbox").toArray();
        if (bbox.size() != 4) {
            continue;
        }

        result.push_back({
            object.value("label").toString("object"),
            object.value("score").toDouble(0.0),
            QRectF(
                bbox.at(0).toDouble(),
                bbox.at(1).toDouble(),
                bbox.at(2).toDouble(),
                bbox.at(3).toDouble()),
        });
    }

    return result.isEmpty() ? defaultDetections() : result;
}

class VisionCanvas final : public QWidget {
public:
    explicit VisionCanvas(QVector<Detection> detections, QWidget* parent = nullptr)
        : QWidget(parent), detections_(std::move(detections))
    {
        setMinimumSize(960, 540);
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            frameIndex_ = (frameIndex_ + 1) % 120;
            update();
        });
        timer->start(120);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(28, 30, 32));

        const QRect frame = contentsRect().adjusted(24, 24, -24, -24);
        drawSyntheticFrame(painter, frame);
        drawDetections(painter, frame);
    }

private:
    void drawSyntheticFrame(QPainter& painter, const QRect& frame)
    {
        QLinearGradient gradient(frame.topLeft(), frame.bottomRight());
        gradient.setColorAt(0.0, QColor(54, 68, 74));
        gradient.setColorAt(1.0, QColor(20, 24, 26));
        painter.fillRect(frame, gradient);

        painter.setPen(QPen(QColor(120, 132, 138, 90), 1));
        for (int x = frame.left(); x < frame.right(); x += 80) {
            painter.drawLine(x, frame.top(), x, frame.bottom());
        }
        for (int y = frame.top(); y < frame.bottom(); y += 60) {
            painter.drawLine(frame.left(), y, frame.right(), y);
        }

        const int sweep = frame.left() + (frame.width() * frameIndex_) / 120;
        painter.fillRect(QRect(sweep, frame.top(), 6, frame.height()), QColor(68, 196, 160, 130));

        painter.setPen(QPen(QColor(225, 230, 232), 1));
        painter.drawText(frame.adjusted(16, 16, -16, -16), Qt::AlignTop | Qt::AlignLeft,
                         "Synthetic frame | mock detections");
    }

    void drawDetections(QPainter& painter, const QRect& frame)
    {
        const QPen boxPen(QColor(255, 214, 102), 3);
        const QColor labelBackground(20, 24, 26, 210);

        for (const Detection& detection : detections_) {
            QRectF box(
                frame.left() + detection.bbox.x() * frame.width(),
                frame.top() + detection.bbox.y() * frame.height(),
                detection.bbox.width() * frame.width(),
                detection.bbox.height() * frame.height());

            painter.setPen(boxPen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(box);

            const QString label = QString("%1 %2%")
                                      .arg(detection.label)
                                      .arg(qRound(detection.score * 100.0));
            const QRectF labelRect(box.left(), box.top() - 28, 150, 24);
            painter.fillRect(labelRect, labelBackground);
            painter.setPen(QColor(248, 248, 246));
            painter.drawText(labelRect.adjusted(8, 0, -8, 0), Qt::AlignVCenter | Qt::AlignLeft, label);
        }
    }

    QVector<Detection> detections_;
    int frameIndex_ = 0;
};

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
    parser.addOption(detectionsOption);
    parser.process(app);

    QMainWindow window;
    window.setWindowTitle("Edge Vision AI Demo Kit - Qt Viewer");
    window.setCentralWidget(new VisionCanvas(loadDetections(parser.value(detectionsOption))));
    window.resize(1100, 660);
    window.show();

    return app.exec();
}
