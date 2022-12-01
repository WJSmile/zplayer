//
// Created by Zwj on 2022/11/30.
//

#ifndef ZPLAYER_VIDEOVIEW_H
#define ZPLAYER_VIDEOVIEW_H


#include "IObserver.h"
extern "C" {
#include <android/native_window_jni.h>
}


class VideoView : public IObserver {
public:
    VideoView(ANativeWindow *aNativeWindow, int videoWidth, int videoHeight);

    virtual void start();

    virtual void stop();

    virtual bool setDataToWindow(XData xData);

protected:
    void Update(XData data) override;

    void Main() override;

    ANativeWindow *nativeWindow;

    ANativeWindow_Buffer windowBuffer{};

    std::vector<XData> videoList;
    std::mutex mux;
};


#endif //ZPLAYER_VIDEOVIEW_H
