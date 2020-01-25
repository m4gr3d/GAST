package com.google.vr.youtube.gast.app

import android.graphics.SurfaceTexture
import android.media.MediaPlayer
import android.util.Log
import android.view.Surface
import com.google.vr.youtube.gast.GastManager
import org.godotengine.godot.Godot

class MainActivity : Godot() {

    private val mediaPlayer: MediaPlayer by lazy {
        MediaPlayer.create(applicationContext, R.raw.flight)
    }

    override fun onGLGodotMainLoopStarted() {
        super.onGLGodotMainLoopStarted()

        val texId =
            GastManager.getExternalTextureId(GastManager.DEFAULT_GROUP_NAME, "sampler_texture")
        Log.d("FHK", "Retrieved $texId from GastManager.")

        runOnUiThread {
            val surfaceTexture = SurfaceTexture(texId)
            surfaceTexture.setDefaultBufferSize(640, 480)
            surfaceTexture.setOnFrameAvailableListener {
                if (mView != null) {
                    mView.queueEvent { surfaceTexture.updateTexImage() }
                }
            }
            val surface = Surface(surfaceTexture)

            mediaPlayer.setSurface(surface)
            mediaPlayer.isLooping = true
            mediaPlayer.start()
        }
    }

    override fun onStop() {
        super.onStop()

        mediaPlayer.release()
        finish()
    }
}
