//
// Created by zwj on 2022/12/4.
//

#ifndef ZPLAYER_XEGL_H
#define ZPLAYER_XEGL_H

class XEGL {
public:
    virtual bool Init(void *window) = 0;
    virtual void Draw() = 0;
    static XEGL *Get();
protected:
    XEGL(){};
};


#endif //ZPLAYER_XEGL_H
