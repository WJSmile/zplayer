//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_DECODE_H
#define ZPLAYER_DECODE_H
#include "XData.h"
#include "IObserver.h"
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct SwsContext;

class DeCode: public IObserver{
public:
    virtual bool open(AVCodecParameters *avCodecParameters,bool isHard);
    virtual bool sendPacket(XData xData);
    virtual XData receiveFrame();
    virtual bool isEven(int num);
    virtual int toEven(int num);

protected:
    virtual void Main();
    virtual  void Update(XData data);
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *frame = nullptr;
    SwsContext *swsContext;
};


#endif //ZPLAYER_DECODE_H
