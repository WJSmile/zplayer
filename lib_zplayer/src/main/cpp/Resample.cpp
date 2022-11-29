//
// Created by Zwj on 2022/11/29.
//


#include "Resample.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

void Resample::initAudioSwrContext(AVCodecContext *avCodecContext) {
    audioSwrContext = swr_alloc();
    swr_alloc_set_opts2(&audioSwrContext,
                        &avCodecContext->ch_layout,
                        AV_SAMPLE_FMT_S16,
                        avCodecContext->sample_rate,
                        &avCodecContext->ch_layout,
                        avCodecContext->sample_fmt, avCodecContext->sample_rate,
                        0, nullptr);
    swr_init(audioSwrContext);
}

void Resample::initVideoSwsContext(AVCodecContext *avCodecContext) {

    videoSwsContext = sws_getContext(avCodecContext->width,
                                     avCodecContext->height,
                                     avCodecContext->pix_fmt,
                                     avCodecContext->width,
                                     avCodecContext->height,
                                     AV_PIX_FMT_RGBA,
                                     SWS_BILINEAR,
                                     nullptr,
                                     nullptr,
                                     nullptr);
}

XData Resample::audioResample(AVFrame *frame) {

    if (!audioSwrContext) {
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
    xData.isAudio = true;
    xData.data = audioBuf;
    xData.size = lastEnqueueBufferSize;
    return xData;
}

XData Resample::videoResample(AVFrame *frame) {

    if (!videoSwsContext) {
        return {};
    }

    AVFrame *renderFrame = av_frame_alloc();

    int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA, frame->width,
                                        frame->height, 1);

    auto *buffer = (uint8_t *) av_malloc(size * sizeof(uint8_t));

    av_image_fill_arrays(renderFrame->data, renderFrame->linesize, buffer, AV_PIX_FMT_RGBA,
                         frame->width, frame->height, 1);

    sws_scale(videoSwsContext, (uint8_t const *const *) frame->data,
              frame->linesize, 0, frame->height,
              renderFrame->data, renderFrame->linesize);
    XData xData;
    xData.isAudio = false;
    xData.data = reinterpret_cast<unsigned char *>(renderFrame);
    xData.width = frame->width;
    xData.height = frame->height;
    return xData;
}





