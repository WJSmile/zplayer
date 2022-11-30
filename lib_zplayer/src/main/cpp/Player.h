//
// Created by Zwj on 2022/11/30.
//

#ifndef ZPLAYER_PLAYER_H
#define ZPLAYER_PLAYER_H

#include "XThread.h"

struct ANativeWindow;
struct Unpacking;
struct DeCode;
struct SLAudioPlay;
struct VideoView;

class Player : public XThread {
public:
    Player(const char *url, ANativeWindow *aNativeWindow);
    virtual void paused(bool isPaused);

    virtual void stop();

protected:
    virtual void open();
    void Main() override;
    Unpacking *unpacking;
    DeCode *audioDeCode;
    DeCode *videoDeCode;
    SLAudioPlay *slAudioPlay;
    VideoView *videoView;
    const char *url;
    ANativeWindow *aNativeWindow;

};


#endif //ZPLAYER_PLAYER_H
