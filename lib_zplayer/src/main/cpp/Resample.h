//
// Created by Zwj on 2022/11/29.
//

#ifndef ZPLAYER_RESAMPLE_H
#define ZPLAYER_RESAMPLE_H

#include <mutex>
#include "XData.h"
extern "C" {
#include <libavutil/rational.h>
}
struct AVCodecParameters;
struct AVCodecContext;
struct SwrContext;
struct SwsContext;
struct AVFrame;


class Resample {
public:
    virtual void initAudioSwrContext(AVCodecContext *avCodecContext,AVRational audioTimeBase);
    virtual void initVideoSwsContext(AVCodecContext *avCodecContext,AVRational videoTimeBase,bool isUseGL,int videoWidth, int videoHeight);
    virtual XData audioResample(AVFrame *frame);
    virtual XData videoResample(AVFrame *frame);

protected:
    virtual XData videoResampleFormGL(AVFrame *frame);
    virtual int toEven(int num);

    SwrContext *audioSwrContext;
    SwsContext *videoSwsContext;
    AVRational audioTimeBase;
    AVRational videoTimeBase;
    bool isTransformation = false;
    bool isUseGL = false;
    int videoWidth;
    int videoHeight;
    uint8_t *imageBuffer;
    std::mutex mux;
};


#endif //ZPLAYER_RESAMPLE_H
