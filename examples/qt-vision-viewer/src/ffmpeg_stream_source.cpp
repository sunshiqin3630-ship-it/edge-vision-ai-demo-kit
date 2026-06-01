#include "ffmpeg_stream_source.h"

#include <QByteArray>
#include <QImage>

#include <utility>

#if defined(EDGE_VIEWER_HAS_FFMPEG)
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
}
#endif

FfmpegStreamSource::FfmpegStreamSource(QString url, QObject* parent)
    : FrameSource(parent), url_(std::move(url))
{
}

FfmpegStreamSource::~FfmpegStreamSource()
{
    stop();
}

QString FfmpegStreamSource::name() const
{
    return "ffmpeg";
}

void FfmpegStreamSource::start()
{
#if defined(EDGE_VIEWER_HAS_FFMPEG)
    if (url_.isEmpty()) {
        emit statusChanged("FFmpeg source URL is empty");
        return;
    }
    if (running_.exchange(true)) {
        return;
    }
    worker_ = std::thread(&FfmpegStreamSource::captureLoop, this);
    emit statusChanged("FFmpeg stream capture starting");
#else
    emit statusChanged("FFmpeg support is disabled. Reconfigure with -DEDGE_VIEWER_ENABLE_FFMPEG=ON.");
#endif
}

void FfmpegStreamSource::stop()
{
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
}

void FfmpegStreamSource::captureLoop()
{
#if defined(EDGE_VIEWER_HAS_FFMPEG)
    const QByteArray url = url_.toUtf8();
    AVFormatContext* format = nullptr;
    int ret = avformat_open_input(&format, url.constData(), nullptr, nullptr);
    if (ret < 0) {
        emit statusChanged("FFmpeg failed to open source");
        running_ = false;
        return;
    }

    ret = avformat_find_stream_info(format, nullptr);
    if (ret < 0) {
        emit statusChanged("FFmpeg failed to read stream info");
        avformat_close_input(&format);
        running_ = false;
        return;
    }

    int videoStream = -1;
    for (unsigned int i = 0; i < format->nb_streams; ++i) {
        if (format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = static_cast<int>(i);
            break;
        }
    }
    if (videoStream < 0) {
        emit statusChanged("FFmpeg source has no video stream");
        avformat_close_input(&format);
        running_ = false;
        return;
    }

    const AVCodecParameters* params = format->streams[videoStream]->codecpar;
    const AVCodec* decoder = avcodec_find_decoder(params->codec_id);
    AVCodecContext* codec = decoder ? avcodec_alloc_context3(decoder) : nullptr;
    if (!decoder || !codec || avcodec_parameters_to_context(codec, params) < 0 ||
        avcodec_open2(codec, decoder, nullptr) < 0) {
        emit statusChanged("FFmpeg failed to initialize video decoder");
        avcodec_free_context(&codec);
        avformat_close_input(&format);
        running_ = false;
        return;
    }

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    SwsContext* scaler = nullptr;
    qint64 sequence = 0;
    emit statusChanged("FFmpeg stream capture running");

    while (running_ && av_read_frame(format, packet) >= 0) {
        if (packet->stream_index == videoStream && avcodec_send_packet(codec, packet) == 0) {
            while (running_ && avcodec_receive_frame(codec, frame) == 0) {
                scaler = sws_getCachedContext(
                    scaler,
                    frame->width,
                    frame->height,
                    static_cast<AVPixelFormat>(frame->format),
                    frame->width,
                    frame->height,
                    AV_PIX_FMT_RGB24,
                    SWS_BILINEAR,
                    nullptr,
                    nullptr,
                    nullptr);
                if (!scaler) {
                    continue;
                }

                QImage image(frame->width, frame->height, QImage::Format_RGB888);
                uint8_t* dst[] = {image.bits(), nullptr, nullptr, nullptr};
                int dstStride[] = {image.bytesPerLine(), 0, 0, 0};
                sws_scale(scaler, frame->data, frame->linesize, 0, frame->height, dst, dstStride);
                emit frameReady(VideoFrame{image.copy(), name(), sequence++});
            }
        }
        av_packet_unref(packet);
    }

    sws_freeContext(scaler);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    running_ = false;
    emit statusChanged("FFmpeg stream capture stopped");
#endif
}
