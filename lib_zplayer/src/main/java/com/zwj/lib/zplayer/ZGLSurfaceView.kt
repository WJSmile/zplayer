package com.zwj.lib.zplayer

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.SurfaceHolder
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class ZGLSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs), GLSurfaceView.Renderer {

    val zPlayer = ZPlayer()

    init {
        setRenderer(this)

    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        Thread {
            zPlayer.open("https://stream7.iqilu.com/10339/upload_transcode/202002/18/20200218025702PSiVKDB5ap.mp4",holder.surface, true)
        }.start()
    }

    override fun onSurfaceCreated(p0: GL10?, p1: EGLConfig?) {
    }

    override fun onSurfaceChanged(p0: GL10?, p1: Int, p2: Int) {
    }

    override fun onDrawFrame(p0: GL10?) {
    }
}