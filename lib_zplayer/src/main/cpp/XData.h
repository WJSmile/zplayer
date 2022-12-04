//
// Created by Zwj on 2022/11/28.
//

#ifndef ZPLAYER_XDATA_H
#define ZPLAYER_XDATA_H
enum XDataType {
    AV_UNKNOWN_TYPE = 0,
    AV_PACK_TYPE = 1,
    AV_FRAME_TYPE = 2,
    AV_BUF_TYPE = 3
};

struct XData {
    XDataType type = AV_UNKNOWN_TYPE;
    unsigned char *data = 0;
    int size = 0;
    bool isAudio = false;
    double pts;
    double videoExtraDelay;
    int width = 0;
    int height = 0;
    int format = 0;

    virtual void release();
};


#endif //ZPLAYER_XDATA_H
