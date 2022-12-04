//
// Created by Zwj on 2022/11/30.
//

#include "Player.h"
#include "Unpacking.h"
#include "DeCode.h"
#include "SLAudioPlay.h"
#include "VideoView.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


void Player::open(const char *url) {
    mux.lock();
    unpacking = new Unpacking();
    unpacking->open(url);

    audioDeCode = new DeCode();

    videoDeCode = new DeCode();

    slAudioPlay = new SLAudioPlay();

    AVStream *avStreamAudio = unpacking->getAudioParameter();
    if (!audioDeCode->open(avStreamAudio->codecpar, avStreamAudio->time_base)) {
        mux.unlock();
        return;
    }
    avStreamAudioVideo = unpacking->getVideoParameter();

    if (!videoDeCode->open(avStreamAudioVideo->codecpar, avStreamAudioVideo->time_base, true)) {
        mux.unlock();
        return;
    }
    slAudioPlay->createSL(avStreamAudio->codecpar->ch_layout.nb_channels,
                          avStreamAudio->codecpar->sample_rate);

    videoView = new VideoView();

    unpacking->AddObs(audioDeCode);

    unpacking->AddObs(videoDeCode);

    audioDeCode->AddObs(slAudioPlay);

    videoDeCode->AddObs(videoView);
    videoView->setVideoCallback(this);
    unpacking->Start();
    audioDeCode->Start();
    videoDeCode->Start();
    slAudioPlay->start();
    videoView->start();
    mux.unlock();

}

void Player::setWindow(ANativeWindow *aNativeWindow, bool isUseGL) {
    mux.lock();
    int windowWidth = ANativeWindow_getWidth(aNativeWindow);
    int windowHeight = (avStreamAudioVideo->codecpar->height * windowWidth) /
                       avStreamAudioVideo->codecpar->width;
    videoDeCode->initVideoResample(avStreamAudioVideo->time_base, isUseGL,windowWidth, windowHeight);
    videoView->setWindow(aNativeWindow, isUseGL);
    mux.unlock();
}

void Player::paused(bool isPaused) {

}

void Player::stop() {
    mux.lock();
    if (unpacking != nullptr) {
        unpacking->Stop();
    }
    if (audioDeCode != nullptr) {
        audioDeCode->Stop();
    }
    if (videoDeCode != nullptr) {
        videoDeCode->Stop();
    }
    if (slAudioPlay != nullptr) {
        slAudioPlay->stop();
    }
    if (videoView != nullptr) {
        videoView->stop();
    }
    mux.unlock();
}

double Player::getAudioTime() {
    return slAudioPlay->getPlayTime();
}










