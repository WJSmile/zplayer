//
// Created by Zwj on 2022/11/28.
//

#include "XData.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

void XData::release() {
    if (!data) {
        return;
    }
    if (type == AV_UNKNOWN_TYPE) {
        delete data;
    } else if (type == AV_PACK_TYPE) {
        av_packet_unref(reinterpret_cast<AVPacket *>(data));
        av_packet_free(reinterpret_cast<AVPacket **>(&data));
    } else if (type == AV_FRAME_TYPE) {
        av_frame_unref(reinterpret_cast<AVFrame *>(data));
        av_frame_free(reinterpret_cast<AVFrame **>(&data));
    } else if (type == AV_BUF_TYPE) {
        av_free(data);
    }
    data = nullptr;
    size = 0;
}
