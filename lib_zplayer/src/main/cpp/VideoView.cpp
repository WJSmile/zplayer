//
// Created by Zwj on 2022/11/30.
//

#include "VideoView.h"
#include "XLog.h"
#include "XTexture.h"


extern "C" {
#include "libavcodec/mediacodec.h"
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
}


void VideoView::setWindow(ANativeWindow *aNativeWindow, bool isUseGL,bool isHard) {

    windowWidth = ANativeWindow_getWidth(aNativeWindow);
    windowHeight = ANativeWindow_getHeight(aNativeWindow);
    ANativeWindow_setBuffersGeometry(aNativeWindow, windowWidth, windowHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    nativeWindow = aNativeWindow;
    this->isUseGL = isUseGL;
    this->isHard = isHard;
}


bool VideoView::setDataToWindow(XData xData) {
    if (!nativeWindow) {
        return true;
    }

    if (ANativeWindow_lock(nativeWindow, &windowBuffer, nullptr) < 0) {
        return false;
    } else {
        auto *dst = (uint8_t *) windowBuffer.bits;
        auto *renderFrame = reinterpret_cast<AVFrame *>(xData.data);
        uint8_t *src = renderFrame->data[0];
        int dstStride = windowBuffer.stride * 4;
        int srcStride = renderFrame->linesize[0];
        int moveDistance = 0;
        if (windowHeight > xData.height) {
            moveDistance = (windowHeight - xData.height) / 2;
        }
        for (int i = 0; i < xData.height; i++) {
            memcpy(dst + (i + moveDistance) * dstStride, src + i * srcStride, srcStride);
        }
        ANativeWindow_unlockAndPost(nativeWindow);
        synTime(xData);
        nowTime = av_gettime();
        return true;
    }
}

bool VideoView::setDataToWindowFormGL(XData xData) {
    if (!nativeWindow) {
        return true;
    }
    if (!xTexture) {
        xTexture = XTexture::Create();
        xTexture->Init(xData.width,xData.height,nativeWindow, (XTextureType) xData.format);
    }
    xTexture->Draw(((AVFrame *) xData.data)->data, xData.width, xData.height);
    synTime(xData);
    nowTime = av_gettime();
    return true;
}

bool VideoView::setDataToSurface(XData xData) {
    auto *buffer = (AVMediaCodecBuffer *) (((AVFrame *) xData.data)->data[3]);
    // 渲染
    int  re = av_mediacodec_release_buffer(buffer, 1);

    if (re!=0){
        return true;
    }

    synTime(xData);
    nowTime = av_gettime();
    return true;
}


volatile void VideoView::synTime(XData xData) {
    if (videoCallback) {
        double codeTimeConsuming = ((double) (av_gettime() - nowTime)) / 1000000.0;
        double time = videoCallback->getAudioTime();
        double pts = xData.pts - codeTimeConsuming;
        double videoExtraDelay = xData.videoExtraDelay - codeTimeConsuming;
        if (time < pts) {
            double sleepTime = pts - time;
            if (videoExtraDelay > sleepTime) {
                XSleep((int) (videoExtraDelay * 1000.0));
            } else {
                XSleep((int) (sleepTime * 1000.0));
            }
        }
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
    videoList.push_back(data);
    mux.unlock();
}

void VideoView::Main() {
    while (!isExit) {
        mux.lock();
        if (videoList.empty()) {
            mux.unlock();
            continue;
        }
        XData xData = videoList.front();
        if (isHard){
            if (setDataToSurface(xData)) {
                videoList.pop_front();
                xData.release();
            }
        } else{
            if (isUseGL) {
                if (setDataToWindowFormGL(xData)) {
                    videoList.pop_front();
                    xData.release();
                }
            } else {
                if (setDataToWindow(xData)) {
                    videoList.pop_front();
                    xData.release();
                }
            }
        }
        mux.unlock();
    }
}

void VideoView::setVideoCallback(VideoCallback *callback) {
    this->videoCallback = callback;
}













