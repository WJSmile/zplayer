//
// Created by Zwj on 2022/11/30.
//

#ifndef ZPLAYER_PLAYER_H
#define ZPLAYER_PLAYER_H

#include <mutex>

struct ANativeWindow;
struct Unpacking;
struct DeCode;
struct SLAudioPlay;
struct VideoView;

class Player  {
public:
    virtual void open(const char *url,ANativeWindow *aNativeWindow);

    virtual void paused(bool isPaused);

    virtual void stop();

protected:
    Unpacking *unpacking;
    DeCode *audioDeCode;
    DeCode *videoDeCode;
    SLAudioPlay *slAudioPlay;
    VideoView *videoView;
    std::mutex mux;
};


#endif //ZPLAYER_PLAYER_H
