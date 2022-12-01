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

    resample = new Resample();
    if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        resample->initAudioSwrContext(avCodecContext);
        XLOGI("打开音频解码器成功");
    } else {
        resample->initVideoSwsContext(avCodecContext);
        XLOGI("打开视频解码器成功");
    }
    return true;
}

SendStatus DeCode::sendPacket(XData xData) {
    SendStatus sendStatus;
    if (xData.size <= 0) {
        sendStatus.isSuccess = false;
        sendStatus.isRetry = false;
        return sendStatus;
    }

    if (!avCodecContext) {
        sendStatus.isSuccess = false;
        sendStatus.isRetry = false;
        return sendStatus;
    }

    auto *packet = reinterpret_cast<AVPacket *>(xData.data);
    int re = avcodec_send_packet(avCodecContext, packet);
    if (AVERROR(EAGAIN) == re || AVERROR_EOF == re) {
        sendStatus.isSuccess = true;
        sendStatus.isRetry = true;
        return sendStatus;
    } else if (AVERROR(EINVAL) == re) {
        avcodec_flush_buffers(avCodecContext);
        sendStatus.isSuccess = false;
        sendStatus.isRetry = false;
        return sendStatus;
    } else if (re != 0) {
        XLOGE("%s", av_err2str(re));
        sendStatus.isSuccess = false;
        sendStatus.isRetry = false;
        return sendStatus;
    }
    sendStatus.isSuccess = true;
    sendStatus.isRetry = false;
    return sendStatus;
}

XData DeCode::receiveFrame() {
    if (!avCodecContext) {
        return {};
    }
    if (!frame) {
        frame = av_frame_alloc();
    }
    int re = avcodec_receive_frame(avCodecContext, frame);
    if (re != 0) {
        return {};
    }

    if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        return resample->audioResample(frame);

    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        return resample->videoResample(frame);
    }

    return {};
}


void DeCode::Main() {
    while (!isExit) {
        mux.lock();
        if (pktList.empty()) {
            mux.unlock();
            continue;
        }
        XData xData = pktList.front();
        SendStatus sendStatus = sendPacket(xData);
        if (sendStatus.isSuccess) {
            if (!sendStatus.isRetry) {
                pktList.pop_front();
            }
            XData framePtk = receiveFrame();
            if (framePtk.size > 0) {
                Notify(framePtk);
            }
        }
        mux.unlock();
    }
}

void DeCode::Update(XData data) {
    mux.lock();
    if (data.size > 0 && (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO && data.isAudio) ||
                          (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO && !data.isAudio)) {
        pktList.push_back(data);
    }
    mux.unlock();
}
