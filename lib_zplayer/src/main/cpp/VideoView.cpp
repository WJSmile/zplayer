//
// Created by Zwj on 2022/11/30.
//

#include "VideoView.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

VideoView::VideoView(ANativeWindow *aNativeWindow, int videoWidth, int videoHeight) {
    ANativeWindow_setBuffersGeometry(aNativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    nativeWindow = aNativeWindow;
}


bool VideoView::setDataToWindow(XData xData) {

    if (ANativeWindow_lock(nativeWindow, &windowBuffer, nullptr) <0) {
        return false;
    } else {
        auto *dst = (uint8_t *) windowBuffer.bits;
        auto *renderFrame = reinterpret_cast<AVFrame *>(xData.data);
        uint8_t *src = renderFrame->data[0];
        int dstStride = windowBuffer.stride * 4;
        int srcStride = renderFrame->linesize[0];
        for (int i = 0; i < xData.height; i++) {
            memcpy(dst + i * dstStride, src + i * srcStride, srcStride);
        }
        ANativeWindow_unlockAndPost(nativeWindow);
        return true;
    }
}

void VideoView::start() {
   XThread::Start();
}

void VideoView::stop() {
    XThread::Stop();
}

void VideoView::Update(XData data) {
    mux.lock();
    if (!data.isAudio){
        videoList.push_back(data);
    }
    mux.unlock();
}

void VideoView::Main() {
    mux.lock();
    while (!isExit){
        if (videoList.empty()){
            mux.unlock();
            continue;
        }
        XData xData = videoList.front();
        if (!setDataToWindow(xData)){
            videoList.pop_back();
        }
    }
    mux.unlock();
}






