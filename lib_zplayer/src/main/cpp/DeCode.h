//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_DECODE_H
#define ZPLAYER_DECODE_H

#include "Resample.h"
#include "IObserver.h"
#include "list"

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct SendStatus {
    bool isSuccess = false;
    bool isRetry = false;
} SendStatus;

class DeCode : public IObserver {
public:
    virtual bool open(AVCodecParameters *avCodecParameters, bool isHard);

    virtual struct SendStatus sendPacket(XData xData);

    virtual XData receiveFrame();


protected:
    virtual void Main();

    virtual void Update(XData data);

    Resample *resample;
    std::list<XData> pktList;
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *frame = nullptr;

};


#endif //ZPLAYER_DECODE_H
