package com.zwj.lib.zplayer

import android.view.Surface

class ZPlayer {

    companion object {
        init {
            System.loadLibrary("zplayer")
        }
    }

    var playerId: Long = -1

    external fun open(url: String,holder: Surface, isUseGL:Boolean)

    external fun paused(boolean: Boolean)

    external fun stop()
}