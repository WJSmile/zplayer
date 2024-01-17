//
// Created by Zwj on 2022/11/28.
//


#include "DeCode.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/mediacodec.h>
#include <libavcodec/jni.h>
}

AVBufferRef *hw_device_ctx = nullptr;
enum AVPixelFormat hw_pix_fmt;
enum AVHWDeviceType type;

static int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type) {
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      nullptr, nullptr, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}


bool
DeCode::open(AVCodecParameters *avCodecParameters, AVRational timeBase, jobject holder, bool isHard,
             bool isUseGL, int videoWidth, int videoHeight) {
    this->isHard = isHard;
    const AVCodec *avCodec;
    if (isHard) {
        switch (avCodecParameters->codec_id) {
            case AV_CODEC_ID_H264:
                avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
                type = av_hwdevice_find_type_by_name("mediacodec");
                for (int i = 0;; i++) {
                    const AVCodecHWConfig *config = avcodec_get_hw_config(avCodec, i);
                    if (!config) {
                        fprintf(stderr, "Decoder %s does not support device type %s.\n",
                                avCodec->name, av_hwdevice_get_type_name(type));
                        return -1;
                    }
                    if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                        config->device_type == type) {
                        hw_pix_fmt = config->pix_fmt;
                        break;
                    }
                }
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
    if (isHard) {
        avCodecContext->get_format = get_hw_format;
        if (hw_decoder_init(avCodecContext, type) < 0){
            return false;
        }
        AVMediaCodecContext *mediaCodecContext = av_mediacodec_alloc_context();
        av_mediacodec_default_init(avCodecContext, mediaCodecContext, holder);
    }
    int re = avcodec_open2(avCodecContext, avCodec, nullptr);

    if (re != 0) {
        XLOGE("error %s", av_err2str(re));
        return false;
    }
    resample = new Resample();
    if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        resample->initAudioSwrContext(avCodecContext, timeBase);
    }
    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO){
        resample->initVideoSwsContext(avCodecContext, timeBase, isUseGL, videoWidth, videoHeight);
    }
    return true;
}

SendStatus DeCode::sendPacket(const XData xData) {
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
    if (isHard){
        frame = av_frame_alloc();
        int re = avcodec_receive_frame(avCodecContext, frame);
        if (re != 0) {
            return {};
        }
        return resample->videoResample(frame);
    } else{
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

            XData framePtk = receiveFrame();
            if (framePtk.size > 0) {
                Notify(framePtk);
            }

            if (!sendStatus.isRetry) {
                pktList.pop_front();
                xData.release();
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

