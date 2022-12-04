//
// Created by Zwj on 2022/11/29.
//

#ifndef ZPLAYER_SLAUDIOPLAY_H
#define ZPLAYER_SLAUDIOPLAY_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "IObserver.h"
#include <list>

class SLAudioPlay : public IObserver {
public:
    SLAudioPlay();

    virtual ~SLAudioPlay();

    virtual bool createSL(int channels, int sample_rate);

    virtual void playCall(void *bufQueue);

    virtual void start();

    virtual void stop();

    virtual void paused();

    virtual void setTimes(XData xData);

    virtual double getPlayTime();
protected:
    void Update(XData data) override;

private:
    double pts = 0;
    long long audioTime = 0;
    //每秒播放音屏大小
    long audioSize = 0;
    int audioPlaySize = 0;
    unsigned char *buf = 0;
    SLObjectItf engineSL = nullptr;
    SLEngineItf eng = nullptr;
    SLObjectItf mix = nullptr;
    SLObjectItf player = nullptr;
    SLPlayItf slPlayer = nullptr;
    SLAndroidSimpleBufferQueueItf pcmQue = nullptr;
    std::list<XData> audioList;
    std::mutex mux;
    std::mutex timesMux;
};


#endif //ZPLAYER_SLAUDIOPLAY_H
