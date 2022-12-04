//
// Created by Zwj on 2022/11/29.
//


#include "Resample.h"
#include "XLog.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

void Resample::initAudioSwrContext(AVCodecContext *avCodecContext, AVRational audioTimeBase) {
    mux.lock();
    audioSwrContext = swr_alloc();
    swr_alloc_set_opts2(&audioSwrContext,
                        &avCodecContext->ch_layout,
                        AV_SAMPLE_FMT_S16,
                        avCodecContext->sample_rate,
                        &avCodecContext->ch_layout,
                        avCodecContext->sample_fmt, avCodecContext->sample_rate,
                        0, nullptr);
    this->audioTimeBase = audioTimeBase;
    swr_init(audioSwrContext);
    mux.unlock();
}

void Resample::initVideoSwsContext(AVCodecContext *avCodecContext, AVRational videoTimeBase,
                                   bool isUseGL,
                                   int videoWidth, int videoHeight) {
    mux.lock();

    this->videoWidth = toEven(videoWidth);
    this->videoHeight = toEven(videoHeight);
    videoSwsContext = sws_getContext(avCodecContext->width,
                                     avCodecContext->height,
                                     avCodecContext->pix_fmt,
                                     this->videoWidth,
                                     this->videoHeight,
                                     isUseGL ? avCodecContext->pix_fmt : AV_PIX_FMT_RGBA,
                                     SWS_BILINEAR,
                                     nullptr,
                                     nullptr,
                                     nullptr);
    XLOGE(">>>>%d", avCodecContext->pix_fmt);
    int size = av_image_get_buffer_size(isUseGL ? avCodecContext->pix_fmt : AV_PIX_FMT_RGBA,
                                        this->videoWidth,
                                        this->videoHeight, 1);
    this->isUseGL = isUseGL;
    imageBuffer = (uint8_t *) av_malloc(size * sizeof(uint8_t));
    this->videoTimeBase = videoTimeBase;
    mux.unlock();

}

XData Resample::audioResample(AVFrame *frame) {
    mux.lock();
    if (!audioSwrContext) {
        mux.unlock();
        return {};
    }
    int lastEnqueueBufferSize = av_samples_get_buffer_size(nullptr,
                                                           frame->ch_layout.nb_channels,
                                                           frame->nb_samples,
                                                           AV_SAMPLE_FMT_S16, 1);
    auto *audioBuf = (uint8_t *) av_malloc(lastEnqueueBufferSize);

    swr_convert(audioSwrContext, &audioBuf,
                lastEnqueueBufferSize, (const uint8_t **) frame->data,
                frame->nb_samples);

    XData xData;
    xData.type = AV_BUF_TYPE;
    xData.pts = av_q2d(audioTimeBase) * (double) frame->pts;
    xData.isAudio = true;
    xData.data = audioBuf;
    xData.size = lastEnqueueBufferSize;
    mux.unlock();
    return xData;
}

XData Resample::videoResample(AVFrame *frame) {
    mux.lock();
    if (!videoSwsContext) {
        mux.unlock();
        return {};
    }

    if (isUseGL) {
        mux.unlock();
        return videoResampleFormGL(frame);
    }
    AVFrame *renderFrame = av_frame_alloc();

    renderFrame->width = videoWidth;
    renderFrame->height = videoHeight;
    av_image_fill_arrays(renderFrame->data, renderFrame->linesize, imageBuffer, AV_PIX_FMT_RGBA,
                         renderFrame->width, renderFrame->height, 1);

    sws_scale(videoSwsContext, (uint8_t const *const *) frame->data,
              frame->linesize, 0, frame->height,
              renderFrame->data, renderFrame->linesize);
    XData xData;
    xData.pts = av_q2d(videoTimeBase) * (double) frame->pts;
    xData.isAudio = false;
    xData.type = AV_FRAME_TYPE;
    xData.videoExtraDelay = frame->repeat_pict / (2.0 * (1.0 / av_q2d(videoTimeBase)));
    xData.data = reinterpret_cast<unsigned char *>(renderFrame);
    xData.size = (renderFrame->linesize[0] + renderFrame->linesize[1] + renderFrame->linesize[2]) *
                 renderFrame->height;
    xData.width = renderFrame->width;
    xData.height = renderFrame->height;
    mux.unlock();
    return xData;
}

XData Resample::videoResampleFormGL(AVFrame *frame) {
    XData xData;
    AVFrame *renderFrame = av_frame_alloc();
    renderFrame->width = videoWidth;
    renderFrame->height = videoHeight;
    av_image_fill_arrays(renderFrame->data, renderFrame->linesize, imageBuffer,
                         (AVPixelFormat) frame->format,
                         renderFrame->width, renderFrame->height, 1);

    sws_scale(videoSwsContext, (uint8_t const *const *) frame->data,
              frame->linesize, 0, frame->height,
              renderFrame->data, renderFrame->linesize);

    xData.data = reinterpret_cast<unsigned char *>(renderFrame);
    xData.size =
            (renderFrame->linesize[0] + renderFrame->linesize[1] + renderFrame->linesize[2]) *
            renderFrame->height;
    xData.width = renderFrame->width;
    xData.height = renderFrame->height;

    xData.format = frame->format;
    xData.pts = av_q2d(videoTimeBase) * (double) frame->pts;
    xData.isAudio = false;
    xData.type = AV_FRAME_TYPE;
    xData.videoExtraDelay = frame->repeat_pict / (2.0 * (1.0 / av_q2d(videoTimeBase)));
    return xData;
}


int Resample::toEven(int num) {
    if (num % 2 == 1) {
        num++;
    }
    if ((num / 2) % 2 == 1) {
        num = ((num / 2) + 1) * 2;
    }
    return num;
}





