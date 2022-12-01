//
// Created by Zwj on 2022/12/1.
//

#ifndef ZPLAYER_VIDEOCALLBACK_H
#define ZPLAYER_VIDEOCALLBACK_H

class VideoCallback {
public:
    virtual double getAudioTime() { return 0; };
};

#endif //ZPLAYER_VIDEOCALLBACK_H
