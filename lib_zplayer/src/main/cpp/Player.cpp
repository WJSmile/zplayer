//
// Created by Zwj on 2022/11/30.
//

#include "Player.h"
#include "Unpacking.h"
#include "DeCode.h"
#include "SLAudioPlay.h"
#include "VideoView.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

Player::Player(const char *url, ANativeWindow *aNativeWindow) {
    this->url = url;
    this->aNativeWindow =aNativeWindow;
}

void Player::open() {
    mux.lock();
    unpacking = new Unpacking();
    unpacking->open(url);

    audioDeCode = new DeCode();

    videoDeCode = new DeCode();

    slAudioPlay = new SLAudioPlay();

    if (!audioDeCode->open(unpacking->getAudioParameter())) {
        mux.unlock();
        return;
    }
    AVCodecParameters *avCodecParameters = unpacking->getVideoParameter();
    if (!videoDeCode->open(avCodecParameters)) {
        mux.unlock();
        return;
    }

    videoView = new VideoView(aNativeWindow, avCodecParameters->width, avCodecParameters->height);

    unpacking->AddObs(audioDeCode);

    unpacking->AddObs(videoDeCode);

    audioDeCode->AddObs(slAudioPlay);

    videoDeCode->AddObs(videoView);
    unpacking->Start();
    audioDeCode->Start();
    videoDeCode->Start();
    slAudioPlay->start();
    videoView->start();
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
        slAudioPlay->Stop();
    }
    if (videoView != nullptr) {
        videoView->stop();
    }
    XThread::Stop();
    mux.unlock();
}

void Player::Main() {
    open();
}








