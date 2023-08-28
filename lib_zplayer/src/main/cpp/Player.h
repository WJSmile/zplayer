//
// Created by Zwj on 2022/11/30.
//

#ifndef ZPLAYER_PLAYER_H
#define ZPLAYER_PLAYER_H

#include <mutex>
#include <jni.h>
#include "VideoCallback.h"

struct ANativeWindow;
struct Unpacking;
struct DeCode;
struct SLAudioPlay;
struct VideoView;
struct AVStream;

class Player : public VideoCallback {
public:
    virtual void open(const char *url,jobject holder,ANativeWindow *aNativeWindow,bool isUseGL);

    virtual void paused(bool isPaused);

    virtual void stop();

protected:
    double getAudioTime() override;

    Unpacking *unpacking;
    DeCode *audioDeCode;
    DeCode *videoDeCode;
    SLAudioPlay *slAudioPlay;
    VideoView *videoView;
    AVStream *avStreamAudioVideo;
    std::mutex mux;
};


#endif //ZPLAYER_PLAYER_H
