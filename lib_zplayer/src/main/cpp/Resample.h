//
// Created by Zwj on 2022/11/29.
//

#ifndef ZPLAYER_RESAMPLE_H
#define ZPLAYER_RESAMPLE_H

#include "XData.h"

struct AVCodecParameters;
struct AVCodecContext;
struct SwrContext;
struct SwsContext;
struct AVFrame;


class Resample {
public:
    virtual void initAudioSwrContext(AVCodecContext *avCodecContext);
    virtual void initVideoSwsContext(AVCodecContext *avCodecContext);
    virtual XData audioResample(AVFrame *frame);
    virtual XData videoResample(AVFrame *frame);

protected:
    SwrContext *audioSwrContext;
    SwsContext *videoSwsContext;
};


#endif //ZPLAYER_RESAMPLE_H
