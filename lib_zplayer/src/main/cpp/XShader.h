//
// Created by zwj on 2022/12/4.
//

#ifndef ZPLAYER_XSHADER_H
#define ZPLAYER_XSHADER_H


enum XShaderType{
    X_SHADER_YUV420P = 0,//Y 4,U 1,V1
    X_SHADER_NV12 = 23,
    X_SHADER_NV21 = 24
};

class XShader {
public:
    virtual bool Init(float scaleX,float scale,XShaderType type = X_SHADER_YUV420P);

    virtual void GetTexture(unsigned int index, int width, int height, unsigned char *buf,bool isa = false);

    virtual void Draw();

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int  program = 0;
    unsigned int texts[100] = {0};

};


#endif //ZPLAYER_XSHADER_H
