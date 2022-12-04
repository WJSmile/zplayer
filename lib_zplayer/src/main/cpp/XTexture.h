//
// Created by zwj on 2022/12/4.
//

#ifndef ZPLAYER_XTEXTURE_H
#define ZPLAYER_XTEXTURE_H

struct ANativeWindow;

enum XTextureType{
    X_TEXTURE_YUV420P = 0,//Y 4,U 1,V1
    X_TEXTURE_NV12 = 23,
    X_TEXTURE_NV21 = 24
};

class XTexture{
public:
    static XTexture *Create();
    virtual bool Init(int videoWidth, int videoHeight,ANativeWindow *window,XTextureType textureType = X_TEXTURE_YUV420P) = 0;
    virtual void Draw(unsigned  char *data[],int width,int height)=0;
};


#endif //ZPLAYER_XTEXTURE_H
