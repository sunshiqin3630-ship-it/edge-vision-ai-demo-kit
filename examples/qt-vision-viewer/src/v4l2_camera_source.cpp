#include "v4l2_camera_source.h"

#include <QImage>

#include <utility>

#if defined(EDGE_VIEWER_HAS_V4L2)
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <vector>
#endif

V4l2CameraSource::V4l2CameraSource(QString devicePath, QSize frameSize, QObject* parent)
    : FrameSource(parent), devicePath_(std::move(devicePath)), frameSize_(frameSize)
{
}

V4l2CameraSource::~V4l2CameraSource()
{
    stop();
}

QString V4l2CameraSource::name() const
{
    return "v4l2";
}

void V4l2CameraSource::start()
{
#if defined(EDGE_VIEWER_HAS_V4L2)
    if (devicePath_.isEmpty()) {
        emit statusChanged("V4L2 device path is empty");
        return;
    }
    if (running_.exchange(true)) {
        return;
    }
    worker_ = std::thread(&V4l2CameraSource::captureLoop, this);
    emit statusChanged("V4L2 UVC capture starting");
#else
    emit statusChanged("V4L2 support is disabled. Reconfigure with -DEDGE_VIEWER_ENABLE_V4L2=ON.");
#endif
}

void V4l2CameraSource::stop()
{
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
}

void V4l2CameraSource::captureLoop()
{
#if defined(EDGE_VIEWER_HAS_V4L2)
    auto xioctl = [](int fd, unsigned long request, void* arg) {
        int ret = 0;
        do {
            ret = ioctl(fd, request, arg);
        } while (ret == -1 && errno == EINTR);
        return ret;
    };

    auto clamp = [](int value) {
        return std::max(0, std::min(255, value));
    };

    auto yuyvToRgb = [&](const unsigned char* data, int width, int height) {
        QImage image(width, height, QImage::Format_RGB888);
        for (int y = 0; y < height; ++y) {
            unsigned char* out = image.scanLine(y);
            const unsigned char* row = data + y * width * 2;
            for (int x = 0; x < width; x += 2) {
                const int y0 = row[x * 2 + 0];
                const int u = row[x * 2 + 1];
                const int y1 = row[x * 2 + 2];
                const int v = row[x * 2 + 3];
                const int values[2] = {y0, y1};
                for (int i = 0; i < 2; ++i) {
                    const int c = values[i] - 16;
                    const int d = u - 128;
                    const int e = v - 128;
                    out[(x + i) * 3 + 0] = static_cast<unsigned char>(clamp((298 * c + 409 * e + 128) >> 8));
                    out[(x + i) * 3 + 1] = static_cast<unsigned char>(clamp((298 * c - 100 * d - 208 * e + 128) >> 8));
                    out[(x + i) * 3 + 2] = static_cast<unsigned char>(clamp((298 * c + 516 * d + 128) >> 8));
                }
            }
        }
        return image;
    };

    const QByteArray device = devicePath_.toLocal8Bit();
    const int fd = open(device.constData(), O_RDWR);
    if (fd < 0) {
        emit statusChanged("V4L2 failed to open UVC device");
        running_ = false;
        return;
    }

    v4l2_capability capability {};
    if (xioctl(fd, VIDIOC_QUERYCAP, &capability) < 0 ||
        !(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        emit statusChanged("V4L2 device does not report video capture capability");
        close(fd);
        running_ = false;
        return;
    }

    v4l2_format format {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = static_cast<unsigned int>(frameSize_.width());
    format.fmt.pix.height = static_cast<unsigned int>(frameSize_.height());
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_NONE;
    if (xioctl(fd, VIDIOC_S_FMT, &format) < 0) {
        emit statusChanged("V4L2 failed to configure YUYV capture format");
        close(fd);
        running_ = false;
        return;
    }

    const int width = static_cast<int>(format.fmt.pix.width);
    const int height = static_cast<int>(format.fmt.pix.height);
    const size_t frameBytes = format.fmt.pix.sizeimage > 0
        ? static_cast<size_t>(format.fmt.pix.sizeimage)
        : static_cast<size_t>(width * height * 2);
    std::vector<unsigned char> buffer(frameBytes);
    qint64 sequence = 0;
    emit statusChanged("V4L2 UVC capture running");

    while (running_) {
        const ssize_t bytes = read(fd, buffer.data(), buffer.size());
        if (bytes <= 0) {
            if (errno == EAGAIN || errno == EINTR) {
                continue;
            }
            emit statusChanged("V4L2 read failed or device stopped");
            break;
        }
        emit frameReady(VideoFrame{yuyvToRgb(buffer.data(), width, height), name(), sequence++});
    }

    close(fd);
    running_ = false;
    emit statusChanged("V4L2 UVC capture stopped");
#endif
}
