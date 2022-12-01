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


void Player::open(const char *url,ANativeWindow *aNativeWindow) {
    mux.lock();
    unpacking = new Unpacking();
    unpacking->open(url);

    audioDeCode = new DeCode();

    videoDeCode = new DeCode();

    slAudioPlay = new SLAudioPlay();

    AVCodecParameters *avCodecParametersAudio = unpacking->getAudioParameter();
    if (!audioDeCode->open(avCodecParametersAudio)) {
        mux.unlock();
        return;
    }
    AVCodecParameters *avCodecParametersVideo = unpacking->getVideoParameter();
    if (!videoDeCode->open(avCodecParametersVideo)) {
        mux.unlock();
        return;
    }
    slAudioPlay->createSL(avCodecParametersAudio->ch_layout.nb_channels,avCodecParametersAudio->sample_rate);

    videoView = new VideoView(aNativeWindow, avCodecParametersVideo->width, avCodecParametersVideo->height);

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
        slAudioPlay->stop();
    }
    if (videoView != nullptr) {
        videoView->stop();
    }
    mux.unlock();
}










