//
// Created by zwj on 2022/12/4.
//

#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"
#include <android/native_window_jni.h>

class CXTexture : public XTexture {
public:
    XShader shader;
    XTextureType type;

    bool Init(int videoWidth, int videoHeight,ANativeWindow *window, XTextureType textureType) override {
        if (!window) {
            return false;
        }
        this->type = textureType;
        if (!XEGL::Get()->Init(window)) return false;
        shader.Init(((float )videoWidth/ (float )ANativeWindow_getWidth(window)),
                    ((float )videoHeight/ (float )ANativeWindow_getHeight(window)),(XShaderType) textureType);
        return true;
    }

    void Draw(unsigned char *data[], int width, int height) override {
        shader.GetTexture(0, width, height, data[0]);  // Y
        if (type == X_TEXTURE_YUV420P) {
            shader.GetTexture(1, width / 2, height / 2, data[1]);  // U
            shader.GetTexture(2, width / 2, height / 2, data[2]);  // V
        } else {
            shader.GetTexture(1, width / 2, height / 2, data[1], true);  // UV
        }
        shader.Draw();
        XEGL::Get()->Draw();
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}