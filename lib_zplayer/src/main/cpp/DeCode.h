//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_DECODE_H
#define ZPLAYER_DECODE_H

#include <jni.h>
#include "Resample.h"
#include "IObserver.h"
#include "list"
#include "SendStatus.h"

extern "C" {
#include <libavutil/rational.h>
}
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;


class DeCode : public IObserver {
public:
    virtual bool open(AVCodecParameters *avCodecParameters,AVRational timeBase ,jobject holder= nullptr,bool isHard = false,bool isUseGL= false, int videoWidth =0, int videoHeight=0);

    virtual  SendStatus sendPacket(const XData xData);

    virtual XData receiveFrame();


protected:
    virtual void Main();

    virtual void Update(XData data);

    Resample *resample;
    std::list<XData> pktList;
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *frame = nullptr;
    bool isHard = false;
    std::mutex mux;
};


#endif //ZPLAYER_DECODE_H
