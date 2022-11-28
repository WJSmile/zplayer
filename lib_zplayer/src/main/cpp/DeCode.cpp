//
// Created by Zwj on 2022/11/28.
//


#include "DeCode.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavcodec/jni.h>
}

bool DeCode::open(AVCodecParameters *avCodecParameters, bool isHard) {

    const AVCodec *avCodec;

    if (isHard) {
        switch (avCodecParameters->codec_id) {
            case AV_CODEC_ID_H264:
                avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
                break;
            default:
                avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
                break;
        }
    } else {
        avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
    }
    if (!avCodec) {
        XLOGE("avcodec_find_decoder %d failed!  %d", avCodecParameters->codec_id, isHard);
        return false;
    }

    avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
    avCodecContext->thread_count = 8;
    int re = avcodec_open2(avCodecContext, avCodec, nullptr);

    if (re != 0) {
        XLOGE("error %s", av_err2str(re));
        return false;
    }
    if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        XLOGI("打开音频解码器成功");
    } else {
        if (!isEven(avCodecContext->width) || !isEven(avCodecContext->height)) {
            swsContext = sws_getContext(avCodecContext->width,
                                        avCodecContext->height,
                                        avCodecContext->pix_fmt,
                                        toEven(avCodecContext->width),
                                        toEven(avCodecContext->height),
                                        avCodecContext->pix_fmt,
                                        SWS_BILINEAR,
                                        nullptr,
                                        nullptr,
                                        nullptr);
        }
        XLOGI("打开视频解码器成功");
    }
    return false;
}

bool DeCode::sendPacket(XData xData) {

    if (xData.size <= 0) {
        return false;
    }

    if (!avCodecContext) {
        return false;
    }

    auto *packet = reinterpret_cast<AVPacket *>(xData.data);
    int re = avcodec_send_packet(avCodecContext, packet);
    if (AVERROR(EAGAIN) == re || AVERROR_EOF == re) {
        return true;
    } else if (AVERROR(EINVAL) == re) {
        avcodec_flush_buffers(avCodecContext);
        return false;
    } else if (re != 0) {
        XLOGE("%s", av_err2str(re));
        return false;
    }

    return true;
}

XData DeCode::receiveFrame() {
    if (!avCodecContext) {
        return {};
    }
    int re = avcodec_receive_frame(avCodecContext, frame);
    if (re != 0) {
        mux.unlock();
        return {};
    }
    XData xData;
    if (avCodecContext->codec_type==AVMEDIA_TYPE_AUDIO){
        xData.data = reinterpret_cast<unsigned char *>(frame);
        xData.size =
                (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;

    } else if (avCodecContext->codec_type==AVMEDIA_TYPE_VIDEO){
        xData.data = reinterpret_cast<unsigned char *>(frame);
        xData.size =
                (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;
    }
    xData.width = frame->width;
    xData.height = frame->height;
    xData.format = frame->format;

    return {};
}

bool DeCode::isEven(int num) {
    if (num % 2 == 1 || (num / 2) % 2 == 1) {
        return false;
    }
    return true;
}

int DeCode::toEven(int num) {
    if (num % 2 == 1) {
        num++;
    }
    if ((num / 2) % 2 == 1) {
        num = ((num / 2) + 1) * 2;
    }
    return num;
}

void DeCode::Main() {

}

void DeCode::Update(XData data) {

}
