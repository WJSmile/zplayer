//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_XDATA_H
#define ZPLAYER_XDATA_H


struct XData {
    unsigned char *data = 0;
    int size = 0;
    bool isAudio = false;
    int width = 0;
    int height = 0;
    int format = 0;
};


#endif //ZPLAYER_XDATA_H
