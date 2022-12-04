package com.zwj.lib.zplayer

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class ZSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs),SurfaceHolder.Callback {

    val zPlayer = ZPlayer()

    init {
        holder.addCallback(this)
        Thread{
            zPlayer.open("https://stream7.iqilu.com/10339/upload_transcode/202002/18/20200218025702PSiVKDB5ap.mp4")
        }.start()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        zPlayer.setHolder(holder.surface,false)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
    }

}