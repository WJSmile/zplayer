//
// Created by Zwj on 2022/11/30.
//

#ifndef ZPLAYER_VIDEOVIEW_H
#define ZPLAYER_VIDEOVIEW_H


#include <list>
#include "IObserver.h"
#include "VideoCallback.h"

extern "C" {
#include <android/native_window_jni.h>
}
class XTexture;

class VideoView : public IObserver {
public:
    virtual void setWindow(ANativeWindow *aNativeWindow,bool isUseGL,bool isHard);
    virtual void start();

    virtual void stop();

    virtual bool setDataToWindow(XData xData);

    virtual bool setDataToWindowFormGL(XData xData);
    virtual bool setDataToSurface(XData xData);

    virtual void setVideoCallback(VideoCallback *callback);

protected:
    void Update(XData data) override;

    void Main() override;
    volatile void synTime(XData xData);

    long long nowTime= 0;
    ANativeWindow *nativeWindow = nullptr;
    bool isUseGL = false;
    bool isHard = false;

    ANativeWindow_Buffer windowBuffer{};

    VideoCallback *videoCallback = 0;
    int windowWidth;
    int windowHeight;

    XTexture *xTexture = nullptr;
    std::list<XData> videoList;
    std::mutex mux;
};


#endif //ZPLAYER_VIDEOVIEW_H
