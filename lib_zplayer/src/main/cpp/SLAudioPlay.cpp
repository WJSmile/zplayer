//
// Created by Zwj on 2022/11/29.
//

#include "SLAudioPlay.h"
#include "XLog.h"

SLAudioPlay::SLAudioPlay() {
    buf = new unsigned char[1024 * 1024];
}

SLAudioPlay::~SLAudioPlay() {
    delete buf;
    buf = nullptr;
}

void SLAudioPlay::playCall(void *bufQueue) {
    if (!bufQueue) {
        return;
    }
    mux.lock();
    XData xData = frames.front();
    if (xData.size <= 0) {
        XLOGE("audioFrame size is 0");
        return;
    }
    if (!buf) {
        return;
    }
    memcpy(buf, xData.data, xData.size);
    if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Enqueue(pcmQue, buf, xData.size);
    }
    mux.unlock();

}

void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *context) {
    auto *ap = (SLAudioPlay *) context;
    if (!ap) {
        XLOGE("PcmCall failed contex is null!");
        return;
    }
    ap->playCall((void *) bf);
}

bool SLAudioPlay::createSL(int channels, int sample_rate) {
    SLresult re;
    re = slCreateEngine(&engineSL, 0, nullptr, 0, nullptr, nullptr);
    if (re != SL_RESULT_SUCCESS) return false;
    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return false;
    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &eng);
    if (re != SL_RESULT_SUCCESS) return false;
    if (eng) {
        XLOGI("CreateSL success！ ");
    } else {
        XLOGE("CreateSL failed！ ");
        return false;
    }
    re = (*eng)->CreateOutputMix(eng, &mix, 0, nullptr, nullptr);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("SL_RESULT_SUCCESS failed!");
        return false;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("(*mix)->Realize failed!");
        return false;
    }
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outmix, nullptr};

    //3 配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) channels,//    声道数
            (SLuint32) sample_rate * 1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN //字节序，小端
    };
    SLDataSource ds = {&que, &pcm};


    //4 创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("CreateAudioPlayer failed!");
        return false;
    } else {
        XLOGI("CreateAudioPlayer success!");
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);
    //获取player接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &slPlayer);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("GetInterface SL_IID_PLAY failed!");
        return false;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        XLOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return false;
    }

    //设置回调函数，播放队列空调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);


    return true;
}

void SLAudioPlay::start() {
    //设置为播放状态
    (*slPlayer)->SetPlayState(slPlayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
}

void SLAudioPlay::paused() {
    (*slPlayer)->SetPlayState(slPlayer, SL_PLAYSTATE_PAUSED);
}

void SLAudioPlay::Update(XData data) {
    mux.lock();
    if (data.isAudio) {
        frames.push_back(data);
    }
    mux.unlock();
}






