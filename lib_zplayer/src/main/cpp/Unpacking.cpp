//
// Created by Zwj on 2022/11/28.
//
extern "C" {
#include <libavformat/avformat.h>
}

#include "Unpacking.h"
#include "XLog.h"

Unpacking::Unpacking() {
    //初始化网络
    avformat_network_init();
}

bool Unpacking::open(const char *url) {
    int result;
    //打开文件
    result = avformat_open_input(&avFormatContext, url, nullptr, nullptr);
    if (result != 0) {
        XLOGE("open error:%s", av_err2str(result));
        return false;
    }
    //探测文件
    result = avformat_find_stream_info(avFormatContext, nullptr);
    if (result != 0) {
        XLOGE("open error:%s", av_err2str(result));
        return false;
    }
    return true;
}

AVCodecParameters *Unpacking::getAudioParameter() {

    audioStream = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (audioStream < 0) {
        XLOGE("get audio error%s", av_err2str(audioStream));
        return nullptr;
    }

    return avFormatContext->streams[audioStream]->codecpar;
}

AVCodecParameters *Unpacking::getVideoParameter() {

    videoStream = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoStream < 0) {
        XLOGE("get video error%s", av_err2str(videoStream));
        return nullptr;
    }

    return avFormatContext->streams[videoStream]->codecpar;
}

XData Unpacking::readFrame() {

    AVPacket *avPacket = av_packet_alloc();
    int result = av_read_frame(avFormatContext, avPacket);
    if (result != 0) {
        XLOGE("get avPacket error%s", av_err2str(result));
        av_packet_free(&avPacket);
        return {};
    }
    XData xData;
    xData.data = reinterpret_cast<unsigned char *>(avPacket);
    xData.size = avPacket->size;
    if (avPacket->stream_index == audioStream) {
        xData.isAudio = true;
    } else if (avPacket->stream_index == videoStream) {
        xData.isAudio = false;
    } else {
        av_packet_free(&avPacket);
        return {};
    }
    return xData;
}

void Unpacking::Main() {
    while (!isExit) {
        XData xData = readFrame();
        if (xData.size > 0) {
            Notify(xData);
        }
    }
}








