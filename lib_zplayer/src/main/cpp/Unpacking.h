//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_UNPACKING_H
#define ZPLAYER_UNPACKING_H


#include "IObserver.h"
#include "XData.h"

struct AVFormatContext;
struct AVStream;

class Unpacking : public IObserver {
public:
    Unpacking();

    virtual bool open(const char *url);

    virtual AVStream *getAudioParameter();

    virtual AVStream *getVideoParameter();
private:
    virtual XData readFrame();
    AVFormatContext *avFormatContext = nullptr;
    int audioStream = 1;
    int videoStream = 0;

    virtual void Main();
};


#endif //ZPLAYER_UNPACKING_H
